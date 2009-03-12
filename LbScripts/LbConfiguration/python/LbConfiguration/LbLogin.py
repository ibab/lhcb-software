#!/usr/bin/env python
""" Main script to setup the basic LHCb environment """

# from imp import find_module
from SetupProject import SetupProject

import sys
import os

# bootstraping the location of the file
# _this_file = find_module(os.path.splitext(os.path.basename(__file__))[0])[1]
_this_file = __file__
_pyconf_dir = os.path.dirname(_this_file)
_py_dir = os.path.dirname(_pyconf_dir)
_base_dir = os.path.dirname(_py_dir)


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
from LbUtils.Script import Script
from LbUtils.Env import Environment, Aliases
from LbUtils.CVS import CVS2Version
from tempfile import mkstemp
from optparse import OptionValueError
import logging
import re
import shutil

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.20 $")


def getLoginCacheName(cmtconfig=None, shell="csh", location=None):
    name = ".LbLoginCache"
    if cmtconfig :
        name += "-" + cmtconfig
    name += "." + shell
    if location :
        name = os.path.join(location, name)
    return name
    

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
    def _write_script(self, env):
        """ select the ouput stream according to the cmd line options """
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
                          help="(internal) output the command to set up the environment ot the given file instead of stdout")
        parser.set_defaults(mysiteroot=None)
        parser.add_option("-m", "--mysiteroot",
                          dest="mysiteroot",
                          help="set MYSITEROOT", 
                          fallback_env="MYSITEROOT")
        parser.set_defaults(cmtconfig=None)
        parser.add_option("-c", "--cmtconfig",
                          dest="cmtconfig",
                          help="set CMTCONFIG", 
                          fallback_env="CMTCONFIG")
        parser.set_defaults(userarea=None)
        parser.add_option("-u", "--userarea",
                          dest="userarea",
                          help="set User_release_area", 
                          fallback_env="User_release_area")
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
        parser.set_defaults(cmtvers="v1r20p20070208")
        parser.add_option("--cmtvers", action="callback",
                          callback= _setCMTVersion_cb,
                          type="string",
                          help="set CMT version")
        parser.set_defaults(scriptsvers=None)
        parser.add_option("--scripts-version",
                          dest="scriptsvers",
                          help="version of LbScripts to be setup [default: %default")
        parser.set_defaults(pythonvers="2.5")
        parser.add_option("--python-version",
                          dest="pythonvers",
                          help="version of python to be setup [default: %default]")
        parser.set_defaults(sharedarea=None)
        parser.add_option("-s", "--shared",
                          dest="sharedarea",
                          help="set the shared area", 
                          fallback_env="VO_LHCB_SW_DIR")
        parser.set_defaults(cmtsite=None)
        parser.add_option("--cmtsite",
                          dest="cmtsite",
                          help="set the CMTSITE", 
                          fallback_env="CMTSITE")

#-----------------------------------------------------------------------------------

    def setPath(self):
        ev = self._env
        if sys.platform != "win32" :
            if ev.has_key("SAVEPATH") :
                if ev["PATH"] != ev["SAVEPATH"] :
                    ev["PATH"] = ev["SAVEPATH"]
                else :
                    ev["SAVEPATH"] = ev["PATH"]

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
        opts = self.options
        ev = self._env
        if opts.mysiteroot :
            ev["MYSITEROOT"] = opts.mysiteroot
            ev["SITEROOT"] = opts.mysiteroot
            ev["CMTSITE"] = "LOCAL"
            opts.cmtsite = "LOCAL"
        else :
            if sys.platform != "win32" :
                afsroot = "/afs"
            else :
                if ev.has_key("AFSROOT") :
                    afsroot = ev["AFSROOT"]
                else : 
                    afsroot = "Z:"
            cernbase = "cern.ch"
            if sys.platform != "win32" :
                cernroot = os.path.join(afsroot, cernbase)
            else :
                cernroot = os.path.join(afsroot+os.sep, cernbase)
            if opts.cmtsite == "LOCAL" :
                opts.mysiteroot = os.getcwd()
                self.setSite()
            elif opts.cmtsite == "CERN" :
                ev["CMTSITE"] = "CERN"
                ev["AFSROOT"] = afsroot 
                ev["SITEROOT"] = cernroot
                if ev.has_key("MYSITEROOT") :
                    del ev["MYSITEROOT"]
            elif os.path.exists(cernroot) and os.path.isdir(cernroot) :
                opts.cmtsite = "CERN"
                self.setSite()
            else : 
                self._add_echo(' the MYSITEROOT variable is not defined')
                self._add_echo(' we suggest you install all software under $MYSITEROOT')
                self._add_echo(' then LHCb software will be installed under $MYSITEROOT/lhcb')
                self._add_echo('      LCG software will be installed under $MYSITEROOT/lcg/external')
                self._add_echo('      CMT and OpenScientist will be installed under $MYSITEROOT/contrib')
                self._add_echo( ' as an example ')
                self._add_echo(' setenv $MYSITEROOT /home/ranjard/Install')

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
            m1 = os.popen("uname").read()[:-1]
            m2 = os.popen("uname -m").read()[:-1].strip()
            natbin = "%s-%s" % (m1, m2)
        else : 
            natbin = "VisualC"        
        return natbin

    def setCMTBin(self):
        ev = self._env
        ev["CMTBIN"] = self.getNativeBin()
        
    def hasCommand(self, cmd):
        hasafs = False
        f = os.popen("which %s >& /dev/null" % cmd)
        f.read()
        if f.close() is None :
            hasafs = True
        return hasafs

    def setCMTSystem(self):
        ev = self._env
        if sys.platform != "win32" :
            if ev.has_key("UNAME") :
                uname = ev["UNAME"]
            else :
                uname = os.popen("uname").read()[:-1]
            may_use_afs = True
            if uname == "Darwin" :
                may_use_afs = False
            if uname == "Linux" or uname == "LynxOS" :
                uname2 = os.popen("uname -m").read()[:-1].strip()
                system = "%s-%s" % (uname, uname2)
            elif uname == "Darwin" or uname.startswith("CYGWIN") :
                system = uname
            if may_use_afs and self.hasCommand("fs"):
                f = os.popen("fs sysname")
                a = f.read()
                if f.close() is None :
                    a = a.replace(" ","")
                    a = a.replace(":","")
                    a = a.replace("'","")
                    a = a.replace("Currentsysnameis","")
                    a = a.replace("Currentsysnamelistis","")
                    system = a.split()[0]
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
            ev["CONTRIBDIR"] = _multiPathJoin(ev["SITEROOT"], "contrib")            

        if sys.platform == "darwin" :
            opts.cmtvers = "v1r20p20070524"
            if opts.mysiteroot :
                opts.use_cache = False
            
        ev["CMT_DIR"] = ev["CONTRIBDIR"]
        ev["CMTROOT"] = _multiPathGet(ev["CMT_DIR"], os.path.join("CMT", opts.cmtvers))
        if not os.path.isdir(ev["CMTROOT"]) :
            ev["CMTROOT"] = self._currentcmtroot
        
        log.debug("The CMT version is %s" % opts.cmtvers)
        self.setCMTInternals()
#-----------------------------------------------------------------------------------
    def setSoftLocations(self):
        ev = self._env
        opts = self.options
        if opts.cmtsite == "LOCAL" :
            ev["LHCBHOME"] = ev["MYSITEROOT"].split(os.pathsep)[0]
            ev["LHCB_USERLOGS"] =  os.path.join(ev["LHCBHOME"], "log", "users")
            ev["DIM_release_area"] = ev["CONTRIBDIR"]
            ev["XMLRPC_release_area"] = ev["CONTRIBDIR"]
            ev["LCG_release_area"] = _multiPathJoin(ev["MYSITEROOT"], os.path.join("lcg" ,"external"))
            ev["LHCBRELEASES"] = _multiPathJoin(ev["MYSITEROOT"], "lhcb")
            ev["GAUDISOFT"] = ev["LHCBRELEASES"]
            ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"],ev["LCG_release_area"]])
        else :
            ev["LHCBHOME"] = os.path.join(ev["SITEROOT"], "lhcb")
            ev["LHCB_USERLOGS"] =  os.path.join(ev["LHCBHOME"], "project", "logfiles")
            ev["DIM_release_area"] = os.path.join(ev["LHCBHOME"], "online", "control")
            ev["XMLRPC_release_area"] = os.path.join(ev["LHCBHOME"], "project", "web", "online" )
            ev["LCG_release_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "app", "releases" )
            ev["SOFTWARE"] = os.path.join(ev["LHCBHOME"], "software" )
            ev["LHCBRELEASES"] = os.path.join(ev["SOFTWARE"], "releases")
            ev["GAUDISOFT"] = os.path.join(ev["SITEROOT"], "sw", "Gaudi", "releases")
            ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"], ev["GAUDISOFT"], ev["LCG_release_area"]])
            ev["LHCBDEV"] = os.path.join(ev["SITEROOT"], "lhcb", "software", "DEV" )
            ev["LHCBTAR"] = os.path.join(ev["SITEROOT"], "lhcb", "distribution" )
            ev["LHCBDOC"] = os.path.join(ev["LHCBRELEASES"], "DOC")
            ev["EMACSDIR"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Emacs", "pro")
            ev["LHCBSTYLE"] = os.path.join(ev["LHCBRELEASES"], "TOOLS", "Tools", "Styles", "pro")

        ev["OSC_release_area"] = ev["CONTRIBDIR"]
        ev["Gaudi_release_area"] = ev["GAUDISOFT"]
        ev["LHCb_release_area"] = ev["LHCBRELEASES"]

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
        if not os.path.exists(rootrcfile) :
            if opts.cmtsite == "CERN" :
                srcrootrcfile = os.path.join(ev["AFSROOT"], "cern.ch", "lhcb", "scripts", ".rootauthrc")
            elif opts.cmtsite == "LOCAL" :
                srcrootrcfile = os.path.join(ev["MYSITEROOT"].split(os.pathsep)[0], "lhcb", "scripts", ".rootauthrc")
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
            
    def setSharedArea(self):
        opts = self.options
        if opts.sharedarea :
            if opts.cmtsite == "LOCAL" :
                opts.mysiteroot = os.pathsep.join(opts.sharedarea.split(os.pathsep))

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
        
    def setCMTConfig(self, debug=False):
        ev = self._env
        opts = self.options
        
        if opts.cmtconfig :
            self.binary, self.platform, self.compdef = self.getTargetPlatformComponents()
        else :
            self.binary, self.platform, self.compdef = self.getNativePlatformComponents()
            if self.platform == "slc5" :
                if self.hasCommand("gcc34") :
                    self.compdef = "gcc34"
            if self.platform == "slc3" :
                self.binary = "ia32"

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

        
        ev["PYTHON_BINOFFSET"] = os.sep+"bin"

        if self.platform == "win32" :
            ev["PYTHON_BINOFFSET"] = ""

                    
        ev["CMTOPT"] = getConfig(self.binary, self.platform, self.compdef)               
        ev["CMTDEB"] = getBinaryDbg(ev["CMTOPT"])
        ev["CMTCONFIG"] = ev["CMTOPT"]
        if debug or sys.platform == "win32":
            ev["CMTCONFIG"] = ev["CMTDEB"]

            
    def setCMTPath(self):
        ev = self._env
        opts = self.options
        
        self.setHomeDir()
        if ev.has_key("CMTPROJECTPATH") :
            if ev.has_key("CMTPATH") :
                del ev["CMTPATH"]
        else :    
            if opts.cmtvers.find("v1r20") == -1 :
                if not opts.remove_userarea:
                    ev["CMTPATH"] = ev["User_release_area"]
                if ev.has_key("CMTPROJECTPATH") :
                    del ev["CMTPROJECTPATH"]
            else :
                if ev.has_key("CMTPATH") :
                    del ev["CMTPATH"]
                if not opts.remove_userarea :
                    ev["CMTPROJECTPATH"] = os.pathsep.join([ev["User_release_area"], ev["LHCBPROJECTPATH"]])
                else :
                    ev["CMTPROJECTPATH"] = ev["LHCBPROJECTPATH"]
    
    def setupLbScripts(self):
        log = logging.getLogger()
        opts = self.options

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
            setupprojargs.append("--no-user-area")
            setupprojargs.append("--disable-CASTOR")
            if self.output_name :
                setupprojargs.append("--append=%s" % self.output_name)
            setupprojargs.append("--shell=%s" % opts.targetshell)
            setupprojargs.append("LbScripts")
            if opts.scriptsvers :
                setupprojargs.append(opts.scriptsvers)
            setupprojargs.append("--runtime-project")            
            setupprojargs.append("LCGCMT")
            setupprojargs.append("Python")
            if opts.pythonvers :
                setupprojargs.append("-v")
                setupprojargs.append(opts.pythonvers)
    
    
            log.debug("Arguments to SetupProject: %s" % " ".join(setupprojargs))
    
            SetupProject().main(setupprojargs)
    
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
#        self.setupLbScripts()

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
            toprint = "*" + " " * 11 + "DEVELOPMENT SCRIPT"
            self._add_echo( toprint + " " * (80-len(toprint)-1) + "*" )
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
                           +self._aliases.gen_script(opts.targetshell)+self._extra)


        self.setupLbScripts()
                    
        return 0


if __name__ == '__main__':
    sys.exit(LbLoginScript(usage="%prog [options]").run())


