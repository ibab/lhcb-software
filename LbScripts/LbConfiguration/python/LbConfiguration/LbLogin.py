#!/usr/bin/env python
""" Main script to setup the basic LHCb environment """

# from imp import find_module
from SetupProject import SetupProject

import sys
import os

# bootstraping the location of the file
try:
    _this_file = __file__
except NameError :
    # special procedure to handle the situation when __file__ is not defined.
    # It happens typically when trying to use pdb.
    from imp import find_module, load_module
    _ff, _filename, _desc = find_module("LbConfiguration")
    try :
        lbconf_package = load_module('LbConfiguration', _ff, _filename, _desc)
        _ff, _filename, _desc = find_module('LbLogin', lbconf_package.__path__)
        _this_file = _filename
    finally :
        _ff.close()


_pyconf_dir = os.path.dirname(_this_file)
_py_dir = os.path.dirname(_pyconf_dir)
_base_dir = os.path.dirname(_py_dir)
_ia_dir = _base_dir
_iapy_dir = os.path.join(_ia_dir, "python")

# added the installarea if I am called from the local package
if os.path.basename(_base_dir) != "InstallArea" :
    _ia_dir = os.path.join(os.path.dirname(_base_dir), "InstallArea" )
    _iapy_dir = os.path.join(_ia_dir, "python")
    if os.path.isdir(_iapy_dir) :
        sys.path.insert(0, _iapy_dir)

# updating the sys.path for the bare minimum of the available scripts
sys.path.insert(0,_pyconf_dir)
sys.path.insert(0,_py_dir)


# needed for the cache use
_scripts_dir = os.path.join(_base_dir, "scripts")


from LbConfiguration.Platform import getBinaryDbg, getConfig
from LbConfiguration.Platform import getCompiler, getPlatformType, getArchitecture
from LbConfiguration.External import CMT_version
from LbUtils.Script import Script
from LbUtils.Env import Environment, Aliases
from LbUtils.CVS import CVS2Version
from tempfile import mkstemp
from optparse import OptionValueError
import logging
import re
import shutil

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.55 $")


def getLoginCacheName(cmtconfig=None, shell="csh", location=None):
    name = ".LbLoginCache"
    if cmtconfig :
        name += "-" + cmtconfig
    name += "." + shell
    if location :
        name = os.path.join(location, name)
    return name
    
def generateSetupProjectCache(cachefn, project, version=None, 
                              cmtconfig=None, setupextra=None, env=None):
    pass


def getLbLoginEnv(optionlist = None):
    if not optionlist :
        optionlist = [] 
    s = LbLoginScript()
    s.defineOpts()
    s.parseOpts(optionlist)

    return s.setEnv()[0]

def _multiPathJoin(path, subdir):
    pathlist = []
    for d in path.split(os.pathsep) :
        pathlist.append(os.path.join(d,subdir))
    return os.pathsep.join(pathlist)

def _multiPathGet(path, subdir):
    for d in path.split(os.pathsep) :
        sd = os.path.join(d, subdir)
        if os.path.exists(sd) :
            break
    return sd

def _check_output_options_cb(option, opt_str, value, parser):
    if opt_str == "--mktemp":
        if parser.values.output:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.mktemp = True
    elif opt_str == "--output":
        if parser.values.mktemp:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.output = value

def _setCMTVersion_cb(option, opt_str, value, parser):
    if parser.values.cmtvers != value :
        parser.values.use_cache = False
        parser.values.cmtvers = value

def _noPython_cb(option, opt_str, value, parser):
    parser.values.get_python = False
    parser.values.use_cache = False

def _userAreaScripts_cb(option, opt_str, value, parser):
    parser.values.user_area_scripts = True
    parser.values.use_cache = False

class LbLoginScript(Script):
    _version = __version__
    _description = __doc__
    def __init__(self, usage=None, version=None):
        Script.__init__(self, usage, version, help_output=sys.stderr)
        self._env = Environment()
        self._aliases = Aliases()
        self._extra = ""
        self.platform = ""
        self.binary   = ""
        self.compdef  = ""
        self.output_file = None
        self.output_name = None
        self._currentcmtroot = os.environ.get("CMTROOT", None)
        self._triedlocalsetup = False
        self._triedAFSsetup = False
    def _write_script(self, env):
        """ select the ouput stream according to the cmd line options """
        log = logging.getLogger()
        close_output = False
        if self.options.output:
            self.output_file = open(self.options.output,"w")
            self.output_name = self.options.output
            self.options.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.options.mktemp:
            fd, outname = mkstemp()
            self.output_name = outname
            self.output_file = os.fdopen(fd,"w")
            print outname
            self.options.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        else :
            self.output_file = sys.stdout
            close_output = False
        # write the data
        if self.output_name :
            log.debug("Writing output to %s" % self.output_name)
        self.output_file.write(env)
        self.output_file.write("\n") # @todo: this may be avoided
        if close_output: 
            self.output_file.close()
    def _add_echo(self, line):
        if line[-1] == "\n" :
            line = line[:-1]           
        outline = "echo '%s'\n" % line
        
        self._extra += outline
            
    def defineOpts(self):
        """ define commandline options """
        parser = self.parser
        parser.set_defaults(targetshell="csh")
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          dest = "targetshell",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")
        parser.set_defaults(mktemp=False)
        parser.add_option("--mktemp", action="callback",
                          callback = _check_output_options_cb,
                          help="(internal) send the output to a temporary file and print on stdout the file name (like mktemp)")
        parser.set_defaults(output=None)
        parser.add_option("--output", action="callback", metavar="FILE",
                          type = "string", callback = _check_output_options_cb,
                          help="(internal) output the command to set up the environment to the given file instead of stdout")
        parser.set_defaults(mysiteroot=None)
        parser.add_option("-m", "--mysiteroot",
                          dest="mysiteroot",
                          help="set MYSITEROOT.")
        parser.set_defaults(cmtsite=None)
        parser.add_option("--cmtsite",
                          dest="cmtsite",
                          help="set the CMTSITE.", 
                          fallback_env="CMTSITE")
        parser.set_defaults(cmtconfig=None)
        parser.add_option("-c", "--cmtconfig",
                          dest="cmtconfig",
                          help="set CMTCONFIG.", 
                          fallback_env="CMTCONFIG")
        parser.set_defaults(userarea=None)
        parser.add_option("-u", "--userarea",
                          dest="userarea",
                          help="set User_release_area.", 
                          fallback_env="User_release_area")
        parser.set_defaults(nightlies_dir=None)
        parser.add_option("-n", "--nightlies-dir",
                          dest="nightlies_dir",
                          help="set nightlies directory.", 
                          fallback_env="LHCBNIGHTLIES")
        parser.set_defaults(remove_userarea=False)
        parser.add_option("--no-userarea",
                          dest="remove_userarea",
                          action="store_true",
                          help="prevent the addition of a user area [default: %default]")
        parser.set_defaults(use_cache=True)
        parser.add_option("--no-cache",
                          dest="use_cache",
                          action="store_false",
                          help="prevent the usage of the cached setup of LbScripts")
        parser.set_defaults(cmtvers=CMT_version)
        parser.add_option("--cmtvers", 
                          action="callback",
                          callback= _setCMTVersion_cb,
                          type="string",
                          help="set CMT version")
        parser.set_defaults(scriptsvers=None)
        parser.add_option("--scripts-version",
                          dest="scriptsvers",
                          help="version of LbScripts to be setup [default: %default]")
        parser.set_defaults(pythonvers=None)
        parser.add_option("--python-version",
                          dest="pythonvers",
                          help="version of python to be setup [default: %default]")
        parser.set_defaults(get_python=True)
        parser.add_option("--no-python",
                          action="callback",
                          callback = _noPython_cb,
                          help="prevents the python setup")
        parser.set_defaults(sharedarea=None)
        parser.add_option("-s", "--shared",
                          dest="sharedarea",
                          help="set the shared area", 
                          fallback_env="VO_LHCB_SW_DIR")
        parser.set_defaults(no_compat=False)
        parser.add_option("--no-compat",
                          dest="no_compat",
                          action="store_true",
                          help="prevent the usage detection of the compat libraries [default: %default]")
        parser.set_defaults(compat_version="v*")
        parser.add_option("--compat-version",
                          dest="compat_version",
                          help="Set the vesion of the Compat project to use [default %default")
        parser.set_defaults(strip_path=True)
        parser.add_option("--no-strip-path",
                          dest="strip_path",
                          action="store_false",
                          help="prevent the cleanup of invalid entries in pathes")
        parser.add_option("--strip-path",
                          dest="strip_path",
                          action="store_true",
                          help="activate the cleanup of invalid entries in pathes [default: %default]")
        parser.set_defaults(user_area_scripts=False)
        parser.add_option("--user-area-scripts",
                          action="callback",
                          callback = _userAreaScripts_cb,
                          help="Enable the usage of the user release area for the setup of the scripts. Use with care. [default: %default]")

#-----------------------------------------------------------------------------------

    def setPath(self):
        ev = self._env
        if sys.platform != "win32" :
            if ev.has_key("SAVEPATH") :
                if ev["PATH"] != ev["SAVEPATH"] :
                    ev["PATH"] = ev["SAVEPATH"]
                else :
                    ev["SAVEPATH"] = ev["PATH"]
        opts = self.options
        log = logging.getLogger()
        if not opts.strip_path :
            log.debug("Disabling the path stripping")
            ev["LB_NO_STRIP_PATH"] = "1"
        else :
            if ev.has_key("LB_NO_STRIP_PATH") :
                log.debug("Reenabling the path stripping")
                del ev["LB_NO_STRIP_PATH"]
        # add the system MANPATH because the CMT setup removes the leading ":"
        if sys.platform != "win32" :
            manp = ["/usr/share/man",
                    "/usr/man",
                    "/usr/local/share/man",
                    "/usr/local/man",
                    "/usr/X11R6/man"]
            manl = []
            if ev.has_key("MANPATH") :
                manl = ev["MANPATH"].split(os.pathsep)
            for m in manp :
                if m not in manl :
                    manl.append(m)
            if manl :
                ev["MANPATH"] = os.pathsep.join(manl)
                

    def setCVSEnv(self):
        """ CVS base setup """
        log = logging.getLogger()
        if sys.platform == "win32" :
            if self._env.has_key("CVS_RSH") :
                log.info("The CVS_RSH environment variable is set to %s" % self._env["CVS_RSH"])
            else :
                log.warning("The CVS_RSH environment variable is not set" )
                log.warning("You should set it to the plink executable from your putty installation" )
                log.warning("in your System Properties" )
                self._env["CVS_RSH"] = "C:\\Program Files\\PuTTY\\plink.exe"
                log.warning("the CVS_RSH environment variable is defaulted to %s" % self._env["CVS_RSH"])
        else :
            self._env["CVS_RSH"] = "ssh"



    def setSite(self):
        """ Site massaging """
        log = logging.getLogger()
        opts = self.options
        ev = self._env
        if ev.has_key("MYSITEROOT") :
            del ev["MYSITEROOT"]
        if opts.mysiteroot :
            # use the mysiteroot value as the top priority choice for the site guessing
            ev["SITEROOT"] = opts.mysiteroot
            opts.cmtsite = "LOCAL"
            ev["CMTSITE"] = opts.cmtsite
            log.debug("CMTSITE is set to %s" % ev["CMTSITE"])
            ev["MYSITEROOT"] = opts.mysiteroot
            log.debug("MYSITEROOT is set to %s" % ev["MYSITEROOT"])
        else :
            # if no mysiteroot has been passed to the script, use CMTSITE for the guessing
            log.debug("No MYSITEROOT defined. Trying CMTSITE (%s) setting." % opts.cmtsite)
            if opts.cmtsite == "LOCAL" :
                self._triedlocalsetup = True
                log.debug("CMTSITE set to LOCAL. Guessing MYSITEROOT")                    
                thismysiteroot = None
                log.debug("IA dir: %s" % _ia_dir)
                thisprojectversdir = os.path.dirname(_ia_dir)
                log.debug("proj vers dir: %s" % thisprojectversdir)
                if os.path.isdir(thisprojectversdir) :
                    thisprojectdir = os.path.dirname(thisprojectversdir)
                    if os.path.isdir(thisprojectdir) :
                        thislhcbdir = os.path.dirname(thisprojectdir)
                        if os.path.isdir(thislhcbdir) :
                            thismysiteroot = os.path.dirname(thislhcbdir)
                log.debug("CMTSITE set to LOCAL. Guessed MYSITEROOT: %s" % thismysiteroot)
                if os.path.isdir(thismysiteroot) and "install_project.py" in os.listdir(thismysiteroot) :
                    log.debug("this mysiteroot: %s" % thismysiteroot)
                    opts.mysiteroot = thismysiteroot                    
                else :
                    if self._triedAFSsetup :
                        log.debug("No valid MYSITEROOT. Trying CMTSITE=standalone")
                        opts.cmtsite = "standalone"
                    else :
                        log.debug("No valid MYSITEROOT. Trying CMTSITE=CERN")
                        opts.cmtsite = "CERN"
                self.setSite()
            elif opts.cmtsite == "CERN" :
                self._triedAFSsetup = True
                log.debug("CMTSITE is set to CERN.")                                    
                cernbase = "cern.ch"
                afsroot = "/afs"
                cernroot = os.path.join(afsroot, cernbase)
                if sys.platform == "win32" :
                    if ev.has_key("AFSROOT") :
                        afsroot = ev["AFSROOT"]
                    else : 
                        afsroot = "Z:"
                    cernroot = os.path.join(afsroot+os.sep, cernbase)
                if os.path.isdir(cernroot) :
                    ev["CMTSITE"] = "CERN"
                    ev["AFSROOT"] = afsroot
                    ev["AFS"] = afsroot 
                    ev["SITEROOT"] = cernroot
                else :
                    if self._triedlocalsetup :
                        # try standalone mode if AFS is not present
                        log.debug("No AFS directory found. Going to standalone mode.")
                        opts.cmtsite = "standalone"
                    else :
                        log.debug("No AFS directory found. Going to local mode.")
                        opts.cmtsite = "LOCAL"
                    self.setSite()
            elif opts.cmtsite == "standalone" :
                ev["CMTSITE"] = "standalone"
            elif not opts.cmtsite :
                # try LOCAL, CERN, standalone in that order
                log.debug("Neither MYSITEROOT nor CMTSITE are set")
                if self._triedlocalsetup :
                    if self._triedAFSsetup :
                        opts.cmtsite = "standalone"
                    else :
                        opts.cmtsite = "CERN"
                else :
                    opts.cmtsite = "LOCAL"
                self.setSite()
            else :
                log.error("Unknown CMTSITE %s" % opts.cmtsite)
            
# sites defaults

        if opts.cmtsite == "CERN" :
            ev["STAGE_HOST"] = "castorlhcb"
            ev["RFIO_USE_CASTOR_V2"] = "YES"
            # set CASTOR_HOME if it has not been defined
            if sys.platform != "win32" :
                if not ev.has_key("CASTOR_HOME") and ev.has_key("HOME") :
                    if ev["HOME"].startswith("/afs/") :
                        ev["CASTOR_HOME"] = ev["HOME"].replace("/afs/", "/castor/")


#-----------------------------------------------------------------------------------
# Core CMT business

    def getNativeBin(self):
        if sys.platform != "win32" :
            ev = self._env
            if ev.has_key("UNAME") :
                m1 = ev["UNAME"]
            else :
                m1 = os.popen("uname").read()[:-1]
            if m1 in ["Linux", "LynxOS", "Darwin"] :
                m2 = os.popen("uname -m").read()[:-1].strip()
                natbin = "%s-%s" % (m1, m2)
            elif m1.startswith("CYGWIN") :
                natbin = m1
        else : 
            natbin = "VisualC"        
        return natbin

    def setCMTBin(self):
        log = logging.getLogger()
        ev = self._env
        ev["CMTBIN"] = self.getNativeBin()
        log.debug("CMTBIN is set to %s" % ev["CMTBIN"])
        
    def hasCommand(self, cmd):
        hascmd = False
        f = os.popen("which %s >& /dev/null" % cmd)
        f.read()
        if f.close() is None :
            hascmd = True
        return hascmd

    def setCMTSystem(self):
        ev = self._env
        if sys.platform != "win32" :
            system = self.getNativeBin()
        else :
            if ev.has_key("CMTCONFIG") :
                system = ev["CMTCONFIG"]
            else :
                system = ev["CMTBIN"]
        return system

    def setCMTInternals(self):
        ev = self._env
        opts = self.options
        
        self.setCMTBin()

        if opts.targetshell == "csh" and ev.has_key("SHELL") :
            if os.path.basename(ev["SHELL"]) == "tcsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.tcsh"),"r") :
                    self._extra += l
                    
        if opts.targetshell == "sh" and ev.has_key("ZSH_NAME") :
            if ev["ZSH_NAME"] == "zsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.zsh"),"r") :
                    self._extra += l
                
        newpath = []    
        for p in ev["PATH"].split(os.pathsep) :
            if p.find(os.sep + "CMT" + os.sep) == -1 :
                newpath.append(p)
        newpath.insert(0, os.path.join(ev["CMTROOT"], ev["CMTBIN"]))
        ev["PATH"] = os.pathsep.join(newpath)

        self._aliases["cmt"] = os.path.join(ev["CMTROOT"], ev["CMTBIN"], "cmt.exe")

        if ev.has_key("CLASSPATH") :
            newpath = []
            for p in ev["CLASSPATH"].split(os.pathsep) :
                if p.find(os.sep + "CMT" + os.sep) == -1 :
                    newpath.append(p)
            newpath.append(os.path.join(ev["CMTROOT"], "java"))
            ev["CLASSPATH"] = os.pathsep.join(newpath)
        else :
            ev["CLASSPATH"] = os.path.join(ev["CMTROOT"], "java")

        self._aliases["jcmt"] = "(java cmt_parser)"

        ev["CMTCONFIG"] = self.setCMTSystem()

    def setCMT(self):
        log = logging.getLogger()
        opts = self.options
        ev = self._env
        if opts.cmtsite == "CERN" :
            ev["CONTRIBDIR"] = os.path.join(ev["SITEROOT"], "sw", "contrib")
        else :
            if ev.has_key("SITEROOT") :
                ev["CONTRIBDIR"] = _multiPathJoin(ev["SITEROOT"], "contrib")            

        if opts.cmtvers == "v1r20p20090520" :
            ev["CMTSTRUCTURINGSTYLE"] = "without_version_directory"
            ev["VERBOSE"] = "1"
        else :
            if sys.platform == "darwin" :
                opts.cmtvers = "v1r20p20070524"

        if sys.platform == "darwin" :
            opts.use_cache = False
        
        if ev.has_key("CONTRIBDIR") :
            ev["CMT_DIR"] = ev["CONTRIBDIR"]
            ev["CMTROOT"] = _multiPathGet(ev["CMT_DIR"], os.path.join("CMT", opts.cmtvers))
        if not os.path.isdir(ev["CMTROOT"]) :
            log.error("Directory %s doesn't exist" % ev["CMTROOT"])
            ev["CMTROOT"] = self._currentcmtroot
        
        ev["CMTVERS"] = opts.cmtvers
        log.debug("The CMT version is %s" % opts.cmtvers)
        self.setCMTInternals()
#-----------------------------------------------------------------------------------
    def setSoftLocations(self):
        ev = self._env
        al = self._aliases
        opts = self.options
        log = logging.getLogger()
        if opts.cmtsite != "standalone" :
            if opts.cmtsite == "LOCAL" :
                ev["LHCBHOME"] = opts.mysiteroot.split(os.pathsep)[0]
                ev["LHCB_USERLOGS"] =  os.path.join(ev["LHCBHOME"], "log", "users")
                ev["DIM_release_area"] = ev["CONTRIBDIR"]
                ev["XMLRPC_release_area"] = ev["CONTRIBDIR"]
                ev["LCG_release_area"] = _multiPathJoin(opts.mysiteroot, os.path.join("lcg" ,"external"))
                ev["LCG_external_area"] = _multiPathJoin(opts.mysiteroot, os.path.join("lcg" ,"external"))
                ev["LHCBRELEASES"] = _multiPathJoin(opts.mysiteroot, "lhcb")
                ev["GAUDISOFT"] = ev["LHCBRELEASES"]
                ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"],ev["LCG_release_area"]])
                if opts.nightlies_dir :
                    ev["LHCBNIGHTLIES"] = opts.nightlies_dir
            else :
                ev["LHCBHOME"] = os.path.join(ev["SITEROOT"], "lhcb")
                ev["LHCB_USERLOGS"] =  os.path.join(ev["LHCBHOME"], "project", "logfiles")
                ev["DIM_release_area"] = os.path.join(ev["LHCBHOME"], "online", "control")
                ev["XMLRPC_release_area"] = os.path.join(ev["LHCBHOME"], "project", "web", "online" )
                ev["LCG_release_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "app", "releases" )
                ev["LCG_external_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "external" )
                ev["SOFTWARE"] = os.path.join(ev["LHCBHOME"], "software" )
                ev["LHCBRELEASES"] = os.path.join(ev["SOFTWARE"], "releases")
                ev["GAUDISOFT"] = os.path.join(ev["SITEROOT"], "sw", "Gaudi", "releases")
                ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"], ev["GAUDISOFT"], ev["LCG_release_area"]])
                ev["LHCBDEV"] = os.path.join(ev["SITEROOT"], "lhcb", "software", "DEV" )
                ev["LHCBTAR"] = os.path.join(ev["SITEROOT"], "lhcb", "distribution" )
                ev["LHCBDOC"] = os.path.join(ev["LHCBRELEASES"], "DOC")
                ev["EMACSDIR"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Emacs", "pro")
                ev["LHCBSTYLE"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Styles", "pro")
                newpath = ev["PATH"].split(os.pathsep)
                newpath.insert(0, os.path.join(ev["LHCBHOME"], "bin"))
                ev["PATH"] = os.pathsep.join(newpath)
                if opts.nightlies_dir :
                    ev["LHCBNIGHTLIES"] = opts.nightlies_dir
                else :
                    ev["LHCBNIGHTLIES"] = os.path.join(ev["SOFTWARE"], "nightlies")
                    
                gangasetupdir = os.path.join(ev["SITEROOT"], "sw", "ganga", "install", "etc")
                if opts.targetshell == "csh" :
                    al["GangaEnv"] = "source %s/setup-lhcb.csh" % gangasetupdir
                elif opts.targetshell == "sh" :
                    al["GangaEnv"] = ". %s/setup-lhcb.sh" % gangasetupdir
    
            ev["OSC_release_area"] = ev["CONTRIBDIR"]
            ev["Gaudi_release_area"] = ev["GAUDISOFT"]
            ev["LHCb_release_area"] = ev["LHCBRELEASES"]
            log.debug("LHCBPROJECTPATH is set to %s" % ev["LHCBPROJECTPATH"])
#-----------------------------------------------------------------------------------
                      
    def setHomeDir(self):
        ev = self._env
        opts = self.options
        if sys.platform == "win32" and not ev.has_key("HOME") :
            ev["HOME"] = os.path.join(ev["HOMEDRIVE"], ev["HOMEPATH"])
        homedir = ev["HOME"]
        rhostfile = os.path.join(homedir,".rhosts")
        if sys.platform != "win32" :
            username = ev["USER"]
        else :
            username = ev["USERNAME"]
        if not os.path.exists(rhostfile) and sys.platform != "win32" :
            self._add_echo( "Creating a %s file to use CMT" % rhostfile ) 
            self._add_echo("Joel.Closier@cern.ch")
            f = open(rhostfile, "w")
            f.write("+ %s\n" % username )
            f.close()
        # remove any .cmtrc file stored in the $HOME directory
        cmtrcfile = os.path.join(homedir, ".cmtrc")
        if os.path.exists(cmtrcfile) :
            os.remove(cmtrcfile)
        # to work with rootd the .rootauthrc file is required
        rootrcfile = os.path.join(homedir, ".rootauthrc")
        if not os.path.exists(rootrcfile) and opts.cmtsite != "standalone" :
            if opts.cmtsite == "CERN" :
                srcrootrcfile = os.path.join(ev["AFSROOT"], "cern.ch", "lhcb", "scripts", ".rootauthrc")
            elif opts.cmtsite == "LOCAL" :
                srcrootrcfile = os.path.join(opts.mysiteroot.split(os.pathsep)[0], "lhcb", "scripts", ".rootauthrc")
            if os.path.exists(srcrootrcfile) :                
                shutil.copy(srcrootrcfile, rootrcfile)

        if not ev.has_key("LD_LIBRARY_PATH") :
            ev["LD_LIBRARY_PATH"] = ""

        if not ev.has_key("ROOTSYS") :
            ev["ROOTSYS"] = ""

        self.setUserArea()  

    def setUserArea(self):
        log = logging.getLogger()
        opts = self.options
        ev = self._env
        al = self._aliases
        if not opts.remove_userarea :
            newdir = False
            if not opts.userarea :
                opts.userarea = os.path.join(ev["HOME"], "cmtuser") # @todo: use something different for window
            ev["User_release_area"] = opts.userarea
            log.debug("User_release_area is set to %s" % ev["User_release_area"])
    
            if os.path.exists(opts.userarea) :
                if not os.path.isdir(opts.userarea) :
                    os.rename(opts.userarea, opts.userarea + "_bak")
                    log.warning("Renamed file %s into %s" % (opts.userarea, opts.userarea + "_bak"))
                    os.mkdir(opts.userarea)
                    newdir = True
                    self._add_echo(" --- a new cmtuser directory has been created in your HOME directory")
            else :
                os.mkdir(opts.userarea)
                newdir = True
            if opts.cmtsite == "CERN" and sys.platform != "win32" and self.hasCommand("fs"):
                if newdir :
                    os.system("fs setacl %s system:anyuser rl" % opts.userarea )
                    self._add_echo( " --- with public access (readonly)" )
                    self._add_echo( " --- use mkprivate to remove public access to the current directory" )
                    self._add_echo( " --- use mkpublic to give public access to the current directory" )
                al["mkprivate"] = "find . -type d -print -exec fs setacl {} system:anyuser l \\;"
                al["mkpublic"] = "find . -type d -print -exec fs setacl {} system:anyuser rl \\;"
        elif ev.has_key("User_release_area") :
            del ev["User_release_area"]
            log.debug("Removed User_release_area from the environment")
            
    def setSharedArea(self):
        opts = self.options
        if opts.sharedarea :
            if opts.cmtsite == "LOCAL" :
                opts.mysiteroot = os.pathsep.join(opts.sharedarea.split(os.pathsep))

    def getLocalCompilers(self, platform, binary):
        compilers = []
        ev = self._env
        if sys.platform != "win32" :
            if platform == "amd64" :
                platform = "x86_64"
            if platform == "ia32" :
                platform = "i686"
            if "gcc" in os.listdir(ev["LCG_external_area"]) :
                gccmainloc = os.path.join(ev["LCG_external_area"], "gcc")
                for v in os.listdir(gccmainloc) :
                    gccversloc = os.path.join(gccmainloc, v)
                    for p in os.listdir(gccversloc) :
                        if p.find(os.sep+"%s-%s" % (binary, platform)+os.sep) != -1 :
                            compilers.append(os.path.join(gccversloc, p))
        return compilers

    def selectCompiler(self, platform, binary):
        log = logging.getLogger()
        local_compilers = self.getLocalCompilers(platform, binary)
        selected_compilers = []
        for c in local_compilers :
            if platform == "slc5" :
                if c.find(os.sep+"4.3") :
                    selected_compilers.append(c)
        if selected_compilers :
            selected_compilers.sort()
            selected_compilers.reverse()
        if selected_compilers and platform == "slc5" :
            log.debug("Found gcc 4.3 @ %s" % selected_compilers[0])
        return selected_compilers

    def getNativePlatformComponents(self):
        platform = None 
        binary = None
        compdef = None
        ev = self._env
        gcclist = []
        islinux = False
        if sys.platform.find("linux") != -1 :
            islinux = True
        if ev.has_key("OSTYPE") :
            if ev["OSTYPE"] == "linux" or ev["OSTYPE"] == "linux-gnu" :
                islinux = True
        if islinux :
            if self.hasCommand("gcc"):
                for l in os.popen("gcc --version") :
                    if l.find("gcc") != -1 :
                        gcclist = l[:-1].split()[2]
                        gcclist = gcclist.split(".")
                        gccvers = int("".join(gcclist[:2]))
                        if gccvers >= 34 :
                            compdef = "gcc%s" % "".join(gcclist[:2])
                        else :
                            compdef = "gcc%s" % "".join(gcclist[:3])
                        break
                
                hwdict = {"ia32" : ["i386"], "amd64" : ["x86_64"] }
                nathw = os.popen("uname -i").read()[:-1]
                for h in hwdict :
                    for l in hwdict[h] :
                        if l == nathw :
                            binary = h
                            break
                relfiles = [ "/etc/redhat-release" , "/etc/system-release" ]
                nbre = re.compile("[0-9]")
                for r in relfiles :
                    if os.path.exists(r) :
                        firstl = open(r, "r").read()[:-1]
                        distrib = firstl.split()[0]
                        rhw = nbre.search(firstl).group()
                        if distrib == "Scientific" :
                            platform = "slc%s" % rhw
                        elif distrib == "Fedora" :
                            platform = "fc%s" % rhw
                        else:
                            platform = "rh%s" % rhw
                        break
                if platform == "slc5" :
                    compdef = "gcc43"
                if platform == "slc3" :
                    binary= "ia32"
                    
        elif sys.platform.find("darwin") != -1 :
            for l in os.popen("gcc --version") :
                if l.find("gcc") != -1 :
                    gcclist = l[:-1].split()[2]
                    gcclist = gcclist.split(".")
                    gccvers = int("".join(gcclist))
                    compdef = "gcc%s" % "".join(gcclist)
                    break
                
            nathw = os.popen("uname -p").read()[:-1]
            if nathw == "powerpc" :
                binary = "ppc"
            else :
                binary = "ia32"
            
            for l in os.popen("sw_vers") :
                if l.find("ProductVersion") != -1 :
                    platlist = l[:-1].split()[1]
                    platlist = platlist.split(".")
                    platform = "osx%s" % "".join(platlist[:2])
        elif sys.platform == "win32":
            platform = "win32"
            compdef = "vc71"
        return binary, platform, compdef    
        
    def getTargetPlatformComponents(self):
        platform = None 
        binary = None
        compdef = None
        opts = self.options
        if opts.cmtconfig :
            platform = getPlatformType(opts.cmtconfig)
            binary = getArchitecture(opts.cmtconfig)
            compdef = getCompiler(opts.cmtconfig)
        return binary, platform, compdef    
        
    def needsCompat(self):
        log = logging.getLogger()
        needs = False
        native_platform = self.getNativePlatformComponents()[1]
        if ( native_platform == "slc5" or native_platform == "sl5" ) and self.platform == "slc4" :
            log.debug("Using slc4 compatibility layer")
            needs = True
        return needs
        
    def setCMTConfig(self, debug=False):
        ev = self._env
        opts = self.options
        log = logging.getLogger()
        if opts.cmtconfig :
            self.binary, self.platform, self.compdef = self.getTargetPlatformComponents()
            opts.use_nocache = False
            if not opts.no_compat and self.needsCompat() :
                opts.no_compat = False
        else :
            self.binary, self.platform, self.compdef = self.getNativePlatformComponents()

        if self.compdef == "gcc323" :
            if opts.cmtsite == "CERN":
                compiler_path = "/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323"
                if not os.path.isdir(compiler_path) or not self.hasCommand("gcc32") :
                    self._add_echo( "%s compiler is not available on this node" % self.compdef )
                else :
                    if ev.has_key("PATH") :
                        pthlist = ev["PATH"].split(os.pathsep)
                    else :
                        pthlist = []
                    pthlist.append(compiler_path)
                    ev["PATH"] = os.pathsep.join(pthlist)
                    if ev.has_key("LD_LIBRARY_PATH") :
                        lpthlist = ev["LD_LIBRARY_PATH"].split(os.path.sep)
                    else :
                        lpthlist = []
                    lpthlist.append(compiler_path)
                    ev["LD_LIBRARY_PATH"] = os.pathsep.join(lpthlist)

        if self.platform == "slc5" and self.compdef == "gcc43" :
            self.selectCompiler(self.platform, self.binary)
        
        ev["PYTHON_BINOFFSET"] = os.sep+"bin"

        if self.platform == "win32" :
            ev["PYTHON_BINOFFSET"] = ""

                    
        ev["CMTOPT"] = getConfig(self.binary, self.platform, self.compdef)
        log.debug("CMTOPT is set to %s" % ev["CMTOPT"])               
        ev["CMTDEB"] = getBinaryDbg(ev["CMTOPT"])
        log.debug("CMTDEB is set to %s" % ev["CMTDEB"])               
        ev["CMTCONFIG"] = ev["CMTOPT"]
        if debug or sys.platform == "win32":
            ev["CMTCONFIG"] = ev["CMTDEB"]
        log.debug("CMTCONFIG is set to %s" % ev["CMTCONFIG"])               

            
    def setCMTPath(self):
        ev = self._env
        opts = self.options
        log = logging.getLogger()
        
        self.setHomeDir()
        
        use_project_path = False
        
        if ev.has_key("CMTPROJECTPATH") :
            use_project_path = True
        elif opts.cmtvers.find("v1r20") != -1 :
            use_project_path = True
            
        if opts.cmtsite != "standalone" :
            if use_project_path :
                if ev.has_key("CMTPATH") :
                    del ev["CMTPATH"]
                if not opts.remove_userarea and ev.has_key("User_release_area") :
                    ev["CMTPROJECTPATH"] = os.pathsep.join([ev["User_release_area"], ev["LHCBPROJECTPATH"]])
                else :
                    ev["CMTPROJECTPATH"] = ev["LHCBPROJECTPATH"]
                log.debug("CMTPROJECTPATH is set to %s" % ev["CMTPROJECTPATH"])
            else :
                if ev.has_key("CMTPROJECTPATH") :
                    del ev["CMTPROJECTPATH"]
                if not opts.remove_userarea and ev.has_key("User_release_area"):
                    ev["CMTPATH"] = ev["User_release_area"]
                    log.debug("CMTPATH is set to %s" % ev["CMTPATH"])
        
    
    def setupLbScripts(self):
        log = logging.getLogger()
        opts = self.options

        log.debug("Setting up LbScripts and appending to the output")
        for var in self._env.keys() :
            os.environ[var] = self._env[var]
        
        cachefile = getLoginCacheName(shell=opts.targetshell, location=_scripts_dir)
        if opts.use_cache and os.path.exists(cachefile):
            log.debug("Using the cache file %s" % cachefile)
            if self.output_name :
                outf = open(self.output_name, "a")
            else :
                outf = sys.stdout
            for l in open(cachefile, "r") :
                outf.write(l)
            if self.output_name :
                outf.close()
        else :
            if not os.path.exists(cachefile) :
                log.debug("Cache file %s doesn't exist" % cachefile)
                log.debug("Calling SetupProject directly")

            setupprojargs=[]
            if opts.loglevel=="DEBUG" :
                setupprojargs.append("--debug")
            if opts.loglevel=="CRITICAL" :
                setupprojargs.append("--silent")
            if not opts.user_area_scripts :
                setupprojargs.append("--no-user-area")
            setupprojargs.append("--disable-CASTOR")
            setupprojargs.append("--no-touch-logfile")
            if self.output_name :
                setupprojargs.append("--append=%s" % self.output_name)
            setupprojargs.append("--shell=%s" % opts.targetshell)
            setupprojargs.append("LbScripts")
            if opts.scriptsvers :
                setupprojargs.append(opts.scriptsvers)
            if opts.get_python :
                setupprojargs.append("--runtime-project")            
                setupprojargs.append("LCGCMT")
                setupprojargs.append("Python")
                if opts.pythonvers :
                    setupprojargs.append("-v")
                    setupprojargs.append(opts.pythonvers)
            if not opts.no_compat :
                setupprojargs.append("--runtime-project")            
                setupprojargs.append("COMPAT")
                setupprojargs.append("--use")
                setupprojargs.append("CompatSys %s" % opts.compat_version)
                
    
            log.debug("Arguments to SetupProject: %s" % " ".join(setupprojargs))
    
            SetupProject().main(setupprojargs)
    
    def setupSystem(self):
        log  = logging.getLogger()
        opts = self.options
        ev   = self._env
        if opts.mysiteroot and sys.platform != "win32":
            libdir = os.path.join(opts.mysiteroot, ev["CMTOPT"])
            ldlist = ev["LD_LIBRARY_PATH"].split(os.pathsep)
            ldlist.append(libdir)
            log.debug("Appending %s to the LD_LIBRARY_PATH" % libdir)
            ev["LD_LIBRARY_PATH"] = os.pathsep.join(ldlist)
    
    def setEnv(self, debug=False):
        log = logging.getLogger()
        log.debug("Entering the environment setup")
        self.setPath()
        self.setCVSEnv()
        self.setSite()
        self.setCMT()
        self.setSoftLocations()
        self.setSharedArea()

        self.setCMTConfig(debug)
        self.setCMTPath()
        self.setupSystem()


        return self._env.env, self._aliases.env, self._extra

    def Manifest(self, debug=False):
        ev = self._env
        opts = self.options
        if opts.loglevel != "CRITICAL" :
            self._add_echo( "*" * 80 )
            if opts.scriptsvers :
                toprint = "*" + " " * 27 + "---- LHCb Login %s ----" % opts.scriptsvers
            else :
                toprint = "*" + " " * 27 + "---- LHCb Login ----"
            self._add_echo(toprint + " " * (80-len(toprint)-1) + "*")
            if self.binary :
                toprint = "*" + " " * 11 + "Building with %s on %s_%s system" % (self.compdef, self.platform, self.binary)
            else : # for windows
                toprint = "*" + " " * 11 + "Building with %s on %s system" % (self.compdef, self.platform)
            self._add_echo(toprint + " " * (80-len(toprint)-1) + "*")
#            toprint = "*" + " " * 11 + "DEVELOPMENT SCRIPT"
#            self._add_echo( toprint + " " * (80-len(toprint)-1) + "*" )
            self._add_echo( "*" * 80 )
            self._add_echo( " --- CMTROOT is set to %s " % ev["CMTROOT"] )
            self._add_echo( " --- CMTCONFIG is set to %s " % ev["CMTCONFIG"] )
            if debug :
                self._add_echo( " --- to compile and link in debug mode : setenv CMTCONFIG $CMTDEB ; gmake" )
            if ev.has_key("CMTPATH") :
                self._add_echo( " --- CMTPATH is set to %s" % ev["CMTPATH"]) 
            else :
                if ev.has_key("User_release_area") :
                    self._add_echo( " --- User_release_area is set to %s" % ev["User_release_area"])
                    self._add_echo( " --- CMTPROJECTPATH is set to $User_release_area:$LHCb_release_area:$Gaudi_release_area:$LCG_release_area")
                else :
                    self._add_echo( " --- CMTPROJECTPATH is set to $LHCb_release_area:$Gaudi_release_area:$LCG_release_area")                    
                self._add_echo( " --- projects will be searched in $CMTPROJECTPATH ")
            self._add_echo( "-" * 80)



    def main(self):
        opts = self.options
        debug = False
        for a in self.args :
            if a == "debug" :
                debug = True

        self.setEnv(debug)
        self.Manifest(debug)

        self._write_script(self._env.gen_script(opts.targetshell)
                           + self._aliases.gen_script(opts.targetshell)
                           + self._extra)


        self.setupLbScripts()
                    
        return 0


if __name__ == '__main__':
    sys.exit(LbLoginScript(usage="%prog [options]").run())

