#!/usr/bin/python
""" Main script to setup the basic LHCb environment """

import sys
import os

#============================================================================
# bootstrapping the location of the file
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
    _ia_dir = os.path.join(os.path.dirname(_base_dir), "InstallArea")
    _iapy_dir = os.path.join(_ia_dir, "python")
    if os.path.isdir(_iapy_dir) :
        sys.path.insert(0, _iapy_dir)

_lbs_home_dir = os.path.dirname(_ia_dir)

# updating the sys.path for the bare minimum of the available scripts
sys.path.insert(0, _pyconf_dir)
sys.path.insert(0, _py_dir)


# needed for the cache use
_scripts_dir = os.path.join(_base_dir, "scripts")
#============================================================================


from SetupProject import SetupProject

from LbConfiguration.Platform import getBinaryDbg, getBinaryOpt
from LbConfiguration.Platform import getCompiler, getPlatformType, getArchitecture
from LbConfiguration.Platform import isBinaryDbg, NativeMachine
from LbConfiguration.External import CMT_version, CMake_version
from LbConfiguration.Version import sortStrings, ParseSvnVersion
from LbUtils.Script import SourceScript
from LbUtils.Path import multiPathGet, multiPathGetFirst, multiPathJoin
from LbUtils.Path import envPathPrepend, pathAdd
import logging
import shutil

__version__ = ParseSvnVersion("$Id$", "$URL$")
#-----------------------------------------------------------------------------------
# Helper functions

def getLoginCacheName(cmtconfig=None, shell="csh", location=None):
    name = ".LbLoginCache"
    if cmtconfig :
        name += "-" + cmtconfig
    name += "." + shell
    if location :
        name = os.path.join(location, name)
    return name


def getLbLoginEnv(optionlist=None):
    if not optionlist :
        optionlist = []
    s = LbLoginScript()
    s.parseOpts(optionlist)
    return s.setEnv()[0]

#-----------------------------------------------------------------------------------
# Option callbacks

def _setCMTVersionCb(_option, _opt_str, value, parser):
    if parser.values.cmtvers != value :
        parser.values.use_cache = False
        parser.values.cmtvers = value

def _setCMakeVersionCb(_option, _opt_str, value, parser):
    if parser.values.cmakevers != value :
        parser.values.use_cache = False
        parser.values.cmakevers = value

def _noPythonCb(_option, _opt_str, _value, parser):
    parser.values.get_python = False
    parser.values.use_cache = False

def _userAreaScriptsCb(_option, _opt_str, _value, parser):
    parser.values.user_area_scripts = True
    parser.values.use_cache = False

def _useDevCb(_option, _opt_str, _value, parser):
    parser.values.usedevarea = True
    parser.values.use_cache = False

def _pythonVerCb(_option, _opt_str, value, parser):
    parser.values.pythonvers = value
    parser.values.get_python = True
    parser.values.use_cache = False

#-----------------------------------------------------------------------------------

class LbLoginScript(SourceScript):
    _version = __version__
    _description = __doc__
    def __init__(self, usage=None, version=None):
        SourceScript.__init__(self, usage, version)
        self.platform = ""
        self.binary = ""
        self.compdef = ""
        self._nativemachine = None
        self._currentcmtroot = os.environ.get("CMTROOT", None)
        self._triedlocalsetup = False
        self._triedAFSsetup = False

#-----------------------------------------------------------------------------------
# Option definition

    def defineOpts(self):
        """ define commandline options """
        parser = self.parser
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
        parser.set_defaults(wcmtconfig=None)
        parser.add_option("-w", "--wildcard-cmtconfig",
                          dest="wcmtconfig",
                          help="choose the first CMTCONFIG that match the string in the list of supported ones")
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
                          callback=_setCMTVersionCb,
                          type="string",
                          help="set CMT version")
        parser.set_defaults(cmakevers=CMake_version)
        parser.add_option("--cmakevers",
                          action="callback",
                          callback=_setCMakeVersionCb,
                          type="string",
                          help="set CMake version")
        parser.set_defaults(scriptsvers=None)
        parser.add_option("--scripts-version",
                          dest="scriptsvers",
                          help="version of LbScripts to be setup [default: %default]")
        parser.set_defaults(usedevarea=False)
        parser.add_option("--dev",
                          dest="usedevarea",
                          action="callback",
                          callback=_useDevCb,
                          help="add the LHCBDEV area for the LbScripts setup [default: %default]")
        parser.set_defaults(pythonvers=None)
        parser.add_option("--python-version",
                          dest="pythonvers",
                          action="callback",
                          callback=_pythonVerCb,
                          help="version of python to be setup [default: %default]")
        parser.set_defaults(get_python=True)
        parser.add_option("--no-python",
                          action="callback",
                          callback=_noPythonCb,
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
                          help="prevent the usage of the Compat project in SetupProject[default: %default]")
        parser.add_option("--use-compat",
                          dest="no_compat",
                          action="store_false",
                          help="add the usage of the Compat project in SetupProject")
        parser.set_defaults(compat_prepend=None)
        parser.add_option("--compat-prepend",
                          dest="compat_prepend",
                          action="store_true",
                          help="(internal) Prepend the Compat env to the script env[default: %default]")
        parser.add_option("--no-compat-prepend",
                          dest="compat_prepend",
                          action="store_false",
                          help="(internal) prevent the usage of the Compat env in the script")
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
                          callback=_userAreaScriptsCb,
                          help="Enable the usage of the user release area for the setup of the scripts. Use with care. [default: %default]")
        parser.set_defaults(use_cmtextratags=False)
        parser.add_option("--dont-use-cmtextratags",
                          dest="use_cmtextratags",
                          action="store_false",
                          help="prevent the usage of CMTEXTRATAGS during the LbScripts setup [default]")
        parser.add_option("--use-cmtextratags",
                          dest="use_cmtextratags",
                          action="store_true",
                          help="use CMTEXTRATAGS during the LbScripts setup")

#-----------------------------------------------------------------------------------

    def setPath(self):
        ev = self.Environment()
        opts = self.options
        log = logging.getLogger()
        log.debug("%s is set to %s" % ("PATH", ev["PATH"]) )
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
                log.debug("Initial MANPATH is %s" % ev["MANPATH"])
            for m in manp :
                if m not in manl :
                    manl.append(m)
            if manl :
                ev["MANPATH"] = os.pathsep.join(manl)
                log.debug("Updated MANPATH to %s" % ev["MANPATH"])


    def setCVSEnv(self):
        """ CVS base setup """
        log = logging.getLogger()
        ev = self.Environment()
        if sys.platform == "win32" :
            if ev.has_key("CVS_RSH") :
                log.info("The CVS_RSH environment variable is set to %s" % self._env["CVS_RSH"])
            else :
                log.warning("The CVS_RSH environment variable is not set")
                log.warning("You should set it to the plink executable from your putty installation")
                log.warning("in your System Properties")
                ev["CVS_RSH"] = "C:\\Program Files\\PuTTY\\plink.exe"
                log.warning("the CVS_RSH environment variable is defaulted to %s" % self._env["CVS_RSH"])
        else :
            ev["CVS_RSH"] = "ssh"



    def setSite(self):
        """ Site massaging """
        log = logging.getLogger()
        opts = self.options
        ev = self.Environment()
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
                    cernroot = os.path.join(afsroot + os.sep, cernbase)
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
                if not ev.has_key( "EOS_HOME" ) and ev.has_key( "HOME" ) :
                    if ev["HOME"].startswith( "/afs/" ) :
                        ev["EOS_HOME"] = ev["HOME"].replace( "/afs/cern.ch/", "/eos/lhcb/" )

#-----------------------------------------------------------------------------------
# Core CMT business

    def setCMTBin(self):
        log = logging.getLogger()
        ev = self.Environment()
        if not self._nativemachine :
            self._nativemachine = NativeMachine()
        ev["CMTBIN"] = self._nativemachine.CMTSystem()
        log.debug("CMTBIN is set to %s" % ev["CMTBIN"])

    def hasCommand(self, cmd):
        hascmd = False
        f = os.popen("which %s >& /dev/null" % cmd)
        f.read()
        if f.close() is None :
            hascmd = True
        return hascmd

    def setCMTSystem(self):
        log = logging.getLogger()
        ev = self.Environment()
        if sys.platform != "win32" :
            system = ev["CMTBIN"]
        else :
            if ev.has_key("CMTCONFIG") :
                system = ev["CMTCONFIG"]
            else :
                system = ev["CMTBIN"]
        log.debug("CMT system is set to %s" % system)
        return system

    def setCMTInternals(self):
        ev = self.Environment()
        al = self.Aliases()

        self.setCMTBin()

        if self.targetShell() == "csh" and ev.has_key("SHELL") :
            if os.path.basename(ev["SHELL"]) == "tcsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.tcsh"), "r") :
                    self.addExtra(l)

        if self.targetShell() == "sh" and ev.has_key("ZSH_NAME") :
            if ev["ZSH_NAME"] == "zsh" :
                for l in open(os.path.join(ev["CMTROOT"], "src", "setup.zsh"), "r") :
                    self.addExtra(l)

        newpath = []
        for p in ev["PATH"].split(os.pathsep) :
            if p.find(os.sep + "CMT" + os.sep) == -1 :
                newpath.append(p)
        newpath.insert(0, os.path.join(ev["CMTROOT"], ev["CMTBIN"]))
        ev["PATH"] = os.pathsep.join(newpath)

        al["cmt"] = os.path.join(ev["CMTROOT"], ev["CMTBIN"], "cmt.exe")

        if ev.has_key("CLASSPATH") :
            newpath = []
            for p in ev["CLASSPATH"].split(os.pathsep) :
                if p.find(os.sep + "CMT" + os.sep) == -1 :
                    newpath.append(p)
            newpath.append(os.path.join(ev["CMTROOT"], "java"))
            ev["CLASSPATH"] = os.pathsep.join(newpath)
        else :
            ev["CLASSPATH"] = os.path.join(ev["CMTROOT"], "java")

        al["jcmt"] = "(java cmt_parser)"

        ev["CMTCONFIG"] = self.setCMTSystem()

    def setCMT(self):
        log = logging.getLogger()
        opts = self.options
        ev = self.Environment()
        if opts.cmtsite == "CERN" :
            ev["CONTRIBDIR"] = os.path.join(ev["SITEROOT"], "sw", "contrib")
        else :
            if ev.has_key("SITEROOT") :
                ev["CONTRIBDIR"] = multiPathJoin(ev["SITEROOT"], "contrib")

        if opts.cmtvers in [ "v1r20p20090520", "v1r21", "v1r22" ] :
            ev["CMTSTRUCTURINGSTYLE"] = "without_version_directory"
            ev["VERBOSE"] = "1"
        else :
            if sys.platform == "darwin" :
                opts.cmtvers = "v1r20p20070524"

        if sys.platform == "darwin" :
            opts.use_cache = False

        if ev.has_key("CONTRIBDIR") :
            ev["CMT_DIR"] = ev["CONTRIBDIR"]
            ev["CMTROOT"] = multiPathGetFirst(ev["CMT_DIR"], os.path.join("CMT", opts.cmtvers))
        if not os.path.isdir(ev["CMTROOT"]) :
            log.error("Directory %s doesn't exist" % ev["CMTROOT"])
            ev["CMTROOT"] = self._currentcmtroot

        ev["CMTVERS"] = opts.cmtvers
        log.debug("The CMT version is %s" % opts.cmtvers)
        self.setCMTInternals()

    def setCMake(self):
        from os.path import join, pathsep, isdir

        log = self.log
        opts = self.options

        log.debug("Looking for CMake %s", opts.cmakevers)

        # The subdirectory containing CMake depends on the version and on the platform.
        if sys.platform == 'darwin':
            # on MacOSX, the application directory has version 'x.y-z' instead of
            # 'x.y.z'
            lastdot = opts.cmakevers.rindex('.')
            appversion = opts.cmakevers[:lastdot-1] + '-' + opts.cmakevers[lastdot+1:]
            subdir = "CMake/%s/Darwin/CMake %s.app/Contents/bin" % (opts.cmakevers, appversion)
        elif sys.platform.startswith('linux'):
            subdir = "CMake/%s/Linux-i386/bin" % opts.cmakevers
        else:
            log.debug('CMake is not provided for platform %s', sys.platform)
            return

        ev = self.Environment()

        def searchpath():
            '''
            Generator for all the possible locations of CMake.
            '''
            if opts.mysiteroot:
                for r in opts.mysiteroot.split(pathsep):
                    yield join(r, "contrib", subdir)
            if 'CONTRIBDIR' in ev:
                for r in ev['CONTRIBDIR'].split(pathsep):
                    yield join(r, subdir)
            if 'SITEROOT' in ev:
                for r in ev['SITEROOT'].split(pathsep):
                    yield join(r, 'sw', 'lcg', 'contrib', subdir)
                    yield join(r, 'sw', 'contrib', subdir)
                    yield join(r, 'contrib', subdir)

        if opts.log_level == 'DEBUG':
            # wrap the generator to print all the searched directories
            _searchpath = searchpath
            def searchpath():
                for d in _searchpath():
                    log.debug("... trying %s", d)
                    yield d
        try:
            # look for the CMake directory
            cmakeDir = (d for d in searchpath() if isdir(d)).next()
            log.debug('Found %s', cmakeDir)
            # if found, we remove all previous entries from the PATH...
            substring = os.sep + 'CMake' + os.sep
            path = [p for p in ev['PATH'].split(os.pathsep)
                    if substring not in p]
            # ... prepend the new one...
            path.insert(0, cmakeDir)
            # ... and set the environment variable
            ev['PATH'] = os.pathsep.join(path)
        except StopIteration:
            log.debug('Cannot find CMake %s directory', opts.cmakevers)

#-----------------------------------------------------------------------------------
    def setSoftLocations(self):
        ev = self.Environment()
        al = self.Aliases()
        opts = self.options
        log = logging.getLogger()
        if opts.cmtsite != "standalone" :
            if opts.cmtsite == "LOCAL" :
                ev["LHCBHOME"] = opts.mysiteroot.split(os.pathsep)[0]
                ev["LHCB_CONF_DIR"] = os.path.join(ev["LHCBHOME"], "conf")
                ev["LHCB_USERLOGS"] = os.path.join(ev["LHCBHOME"], "log", "users")
                ev["DIM_release_area"] = ev["CONTRIBDIR"]
                ev["XMLRPC_release_area"] = ev["CONTRIBDIR"]
                ev["LCG_release_area"] = multiPathJoin(opts.mysiteroot, os.path.join("lcg" , "releases"),
                                                       exist_check=True)
                ev["LCG_release_area"] = pathAdd(ev["LCG_release_area"],
                                                 multiPathJoin(opts.mysiteroot, os.path.join("lcg" , "app", "releases"),
                                                       exist_check=True))
                ev["LCG_release_area"] = pathAdd(ev["LCG_release_area"],
                                                 multiPathJoin(opts.mysiteroot, os.path.join("lcg", "external")))
                ev["LCG_external_area"] = multiPathJoin(opts.mysiteroot, os.path.join("lcg" , "external"))
                ev["LHCBRELEASES"] = multiPathJoin(opts.mysiteroot, "lhcb")
                ev["GAUDISOFT"] = ev["LHCBRELEASES"]
                ev["LHCBPROJECTPATH"] = pathAdd(ev["LHCBRELEASES"], ev["LCG_release_area"])
                emacsdir = multiPathGetFirst(ev["LHCBRELEASES"],
                                             os.path.join("TOOLS", "Tools", "Emacs", "pro"))
                if emacsdir :
                    ev["EMACSDIR"] = emacsdir
                lhcbstyle = multiPathGetFirst(ev["LHCBRELEASES"],
                                              os.path.join("TOOLS", "Tools", "Styles", "pro"))
                if lhcbstyle :
                    ev["LHCBSTYLE"] = lhcbstyle
                if opts.nightlies_dir :
                    ev["LHCBNIGHTLIES"] = opts.nightlies_dir
            else :
                ev["LHCBHOME"] = os.path.join(ev["SITEROOT"], "lhcb")
                ev["LHCB_USERLOGS"] = os.path.join(ev["LHCBHOME"], "project", "logfiles")
                ev["DIM_release_area"] = os.path.join(ev["LHCBHOME"], "online", "control")
                ev["XMLRPC_release_area"] = os.path.join(ev["LHCBHOME"], "project", "web", "online")
                ev["LCG_release_area"] = os.pathsep.join([os.path.join(ev["SITEROOT"], "sw", "lcg", "releases"),
                                                          os.path.join(ev["SITEROOT"], "sw", "lcg", "app", "releases")])
                ev["LCG_external_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "external")
                ev["LCG_nightlies_area"] = os.path.join(ev["SITEROOT"], "sw", "lcg", "app", "nightlies")
                ev["SOFTWARE"] = os.path.join(ev["LHCBHOME"], "software")
                ev["LHCBRELEASES"] = os.path.join(ev["SOFTWARE"], "releases")
                ev["GAUDISOFT"] = os.path.join(ev["SITEROOT"], "sw", "Gaudi", "releases")
                ev["LHCBPROJECTPATH"] = os.pathsep.join([ev["LHCBRELEASES"], ev["GAUDISOFT"], ev["LCG_release_area"]])
                ev["LHCBDEV"] = os.path.join(ev["SITEROOT"], "lhcb", "software", "DEV")
                ev["LHCBTAR"] = os.path.join(ev["SITEROOT"], "lhcb", "distribution")
                ev["LHCB_CONF_DIR"] = os.path.join(ev["LHCBTAR"], "conf")
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
                if self.targetShell() == "csh" :
                    al["GangaEnv"] = "source %s/setup-lhcb.csh" % gangasetupdir
                elif self.targetShell() == "sh" :
                    al["GangaEnv"] = ". %s/setup-lhcb.sh" % gangasetupdir

            ev["OSC_release_area"] = ev["CONTRIBDIR"]
            ev["Gaudi_release_area"] = ev["GAUDISOFT"]
            ev["LHCb_release_area"] = ev["LHCBRELEASES"]
            log.debug("LHCBPROJECTPATH is set to %s" % ev["LHCBPROJECTPATH"])
        # Hack to allow for switching version of LbScripts between AFS and cvmfs
        if opts.cmtsite == "CERN" :
            afsLbLoginDir = "/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/prod/InstallArea/scripts"
            afsDevLbLoginDir = "/afs/cern.ch/lhcb/software/releases/LBSCRIPTS/dev/InstallArea/scripts"
            cvmfsLbLoginDir = "/cvmfs/lhcb.cern.ch/lib/lhcb/LBSCRIPTS/prod/InstallArea/scripts"
            if self.targetShell() == "csh" :
                al["afsLbLogin"] = "source %s/LbLogin.csh" % afsLbLoginDir
                al["afsDevLbLogin"] = "source %s/LbLogin.csh" % afsDevLbLoginDir
                al["cvmfsLbLogin"] = "source %s/LbLogin.csh" % cvmfsLbLoginDir
            elif self.targetShell() == "sh" :
                al["afsLbLogin"] = ". %s/LbLogin.sh" % afsLbLoginDir
                al["afsDevLbLogin"] = ". %s/LbLogin.sh" % afsDevLbLoginDir
                al["cvmfsLbLogin"] = ". %s/LbLogin.sh" % cvmfsLbLoginDir

#-----------------------------------------------------------------------------------

    def setHomeDir(self):
        ev = self.Environment()
        opts = self.options
        log = logging.getLogger()
        if sys.platform == "win32" and not ev.has_key("HOME") :
            ev["HOME"] = os.path.join(ev["HOMEDRIVE"], ev["HOMEPATH"])
            log.debug("Setting HOME to %s" % ev["HOME"])
        homedir = ev["HOME"]

        if sys.platform != "win32" :
            username = ev["USER"]
        else :
            username = ev["USERNAME"]
        log.debug("User name is %s" % username)

        if not ev.has_key("LD_LIBRARY_PATH") :
            ev["LD_LIBRARY_PATH"] = ""
            log.debug("Setting a default LD_LIBRARY_PATH")

        if not ev.has_key("ROOTSYS") :
            ev["ROOTSYS"] = ""
            log.debug("Setting a default ROOTSYS")

        self.setUserArea()

    def setUserArea(self):
        log = logging.getLogger()
        opts = self.options
        ev = self.Environment()
        al = self.Aliases()
        if not opts.remove_userarea :
            newdir = False
            if not opts.userarea :
                opts.userarea = os.path.join(ev["HOME"], "cmtuser") 
            ev["User_release_area"] = opts.userarea
            log.debug("User_release_area is set to %s" % ev["User_release_area"])
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
        ev = self.Environment()
        log = logging.getLogger()
        if sys.platform != "win32" :
            if platform == "amd64" :
                platform = "x86_64"
            if platform == "ia32" :
                platform = "i686"
            log.debug("Getting gcc main location")
            for d in multiPathGet(ev["LCG_external_area"], "gcc", alloccurences=True) :
                for v in os.listdir(d) :
                    gccversloc = os.path.join(d, v)
                    for p in os.listdir(gccversloc) :
                        if p.find(os.sep + "%s-%s" % (binary, platform) + os.sep) != -1 :
                            compilers.append(os.path.join(gccversloc, p))
                            log.debug("Found compiler: %s" % os.path.join(gccversloc, p))
        return compilers

    def selectCompiler(self, platform, binary):
        log = logging.getLogger()
        local_compilers = self.getLocalCompilers(platform, binary)
        selected_compilers = []
        for c in local_compilers :
            if platform == "slc5" :
                if c.find(os.sep + "4.3") :
                    selected_compilers.append(c)
        if selected_compilers :
            selected_compilers.sort()
            selected_compilers.reverse()
        if selected_compilers and platform == "slc5" :
            log.debug("Selected gcc 4.3 @ %s" % selected_compilers[0])
        return selected_compilers


    def getWildCardCMTConfig(self, wildcard=None, debug=False):
        """
        returns the best matched CMTCONFIG for the wilcard string
        @param wildcard: text to be look for in the CMTCONFIG
        @type wildcard: string
        @param debug: if the searched list includes also the dbg CMTCONFIG
        @type debug: boolean
        """
        opts = self.options
        log = logging.getLogger()
        theconf = None
        supported_configs = self._nativemachine.CMTSupportedConfig(debug=debug)
        if opts.cmtsite == "CERN" :
            # every platform with a descent python at CERN
            supconf = self._nativemachine.CMTCompatibleConfig(debug=debug)
        else :
            # restriction on supported CONFIG for LOCAL use
            supconf = supported_configs

        if wildcard :
            log.debug("Looking for %s in the list of selected CMTCONFIGs." % wildcard)
            supconf = [ c for c in supconf if wildcard in c ]

        if supconf :
            theconf = supconf[0]
            if theconf not in supported_configs :
                log.warning("%s is not in the list of distributed configurations" % theconf)
                if supported_configs :
                    log.warning("Please switch to a supported one with 'LbLogin -c' before building")
                    log.warning("Supported configs: %s" % ", ".join(supported_configs))

        return theconf

    def setCMTConfig(self, debug=False):
        ev = self.Environment()
        opts = self.options
        log = logging.getLogger()
        self.binary = None
        self.platform = None
        self.compdef = None
        if not opts.wcmtconfig :
            if opts.cmtconfig :
                log.debug("Using provided CMTCONFIG %s" % opts.cmtconfig)
                theconf = opts.cmtconfig
                opts.use_nocache = False
            else :
                log.debug("Guessing CMTCONFIG")
                theconf = self.getWildCardCMTConfig(debug=debug)
                if not theconf :
                    log.debug("Falling back on the native CMTCONFIG")
                    theconf = self._nativemachine.CMTNativeConfig(debug=debug)
        else :
            theconf = self.getWildCardCMTConfig(wildcard=opts.wcmtconfig, debug=True)
            if not theconf :
                if opts.cmtconfig :
                    log.debug("Falling back on the previous CMTCONFIG")
                    theconf = opts.cmtconfig
                else :
                    log.debug("Falling back on the native CMTCONFIG")
                    theconf = self._nativemachine.CMTNativeConfig(debug=debug)

        if theconf :
            if isBinaryDbg(theconf) :
                debug = True
            self.binary = getArchitecture(theconf)
            self.platform = getPlatformType(theconf)
            self.compdef = getCompiler(theconf)
            opts.cmtconfig = theconf


        ev["PYTHON_BINOFFSET"] = os.sep + "bin"

        if self.platform == "win32" :
            ev["PYTHON_BINOFFSET"] = ""

        ev["CMTOPT"] = getBinaryOpt(theconf)
        log.debug("CMTOPT is set to %s" % ev["CMTOPT"])

        ev["CMTDEB"] = getBinaryDbg(theconf)
        log.debug("CMTDEB is set to %s" % ev["CMTDEB"])


        if debug or sys.platform == "win32" :
            ev["CMTCONFIG"] = ev["CMTDEB"]
            log.debug("CMTDEB is set to %s" % ev["CMTDEB"])
        else :
            ev["CMTCONFIG"] = ev["CMTOPT"]
        log.debug("CMTCONFIG is set to %s" % ev["CMTCONFIG"])

    def setLCGhostos(self):
        '''
        Set the environment variable LCG_hostos, used by the 'lcg-' compiler wrappers.
        '''
        ev = self.Environment()
        nm = self._nativemachine
        # we take only the first two elements of the native CMTCONFIG, e.g.
        #  x86_64-slc5-gcc46-opt -> x86_64-slc5
        ev['LCG_hostos'] = '-'.join(nm.CMTNativeConfig().split('-')[:2])

    def setCMTPath(self):
        ev = self.Environment()
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

    def setExtraTags(self):
        """
        setup the canonical CMTEXTRATAGS environment variable that has to be used
        by "SetupProject LbScripts". By default and unless specified on the command line
        only "NO_PYTHON_LIBPATH" is passed.
        """
        opts = self.options
        log  = logging.getLogger()
        if opts.use_cmtextratags :
            if "CMTEXTRATAGS" in os.environ.keys() :
                extra_args_list = os.environ["CMTEXTRATAGS"].replace(",", " ").split()
                extra_args_list.append("NO_PYTHON_LIBPATH")
                # according to the documentation tags are space separated.
                # but "," do work as well.
                os.environ["CMTEXTRATAGS"] = " ".join(extra_args_list)
            else :
                os.environ["CMTEXTRATAGS"] = "NO_PYTHON_LIBPATH"
        else :
            os.environ["CMTEXTRATAGS"] = "NO_PYTHON_LIBPATH"

        log.debug("CMTEXTRATAGS is set to %s" % os.environ.get("CMTEXTRATAGS", None))

    def setupCompat(self):
        """ preset the compat entries before the call to SetupProject """
        log = logging.getLogger()
        ev = self.Environment()
        opts = self.options
        log.debug("Trying to prepend the Compat project")

        if ev.has_key("LHCBRELEASES") :

            compat_dir = multiPathGetFirst(ev["LHCBRELEASES"], "COMPAT")

            if compat_dir :
                lastver = None
                if (not opts.compat_version) or opts.compat_version == "v*" :
                    compat_lst = [ x for x in os.listdir(compat_dir) if x.startswith("COMPAT_") ]
                    if compat_lst :
                        lastver = sortStrings(compat_lst, safe=True)[-1]
                else :
                    lastver = "COMPAT_%s" % opts.compat_version
                if lastver :
                    compat_rel = os.path.join(compat_dir, lastver)
                    compat_lib = os.path.join(compat_rel, "CompatSys", ev["CMTOPT"], "lib")
                    if sys.platform != "win32" :
                        compat_bin = os.path.join(compat_rel, "CompatSys", ev["CMTOPT"], "bin")
                        envPathPrepend("PATH", compat_bin)
                        envPathPrepend("LD_LIBRARY_PATH", compat_lib)
                        log.debug("Internal %s is set to %s" % ("LD_LIBRARY_PATH", os.environ["LD_LIBRARY_PATH"]))
                    else :
                        envPathPrepend("PATH", compat_lib)
                    log.debug("Internal %s is set to %s" % ("PATH", os.environ["PATH"]))


    def setupLbScripts(self):
        """ Call to SetupProject with LbScripts and python """
        log = logging.getLogger()
        opts = self.options
        ev = self.Environment()
        log.debug("Setting up LbScripts and appending to the output")
        for var in ev.keys() :
            os.environ[var] = ev[var]

        log.debug("%s is set to %s" % ("PATH", ev.get("PATH", "")))
        if sys.platform != "win32" :
            log.debug("%s is set to %s" % ("LD_LIBRARY_PATH", ev.get("LD_LIBRARY_PATH", "")))

        cachefile = getLoginCacheName(shell=self.targetShell(), location=_scripts_dir)
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

            setupprojargs = []
            # needed for the deployment: otherwise the cache is not generated
            setupprojargs.append("--ignore-not-ready")
            if opts.log_level == "DEBUG" :
                setupprojargs.append("--debug")
            if opts.log_level == "CRITICAL" :
                setupprojargs.append("--silent")
            if not opts.user_area_scripts :
                setupprojargs.append("--no-user-area")
            if opts.usedevarea :
                setupprojargs.append("--dev")
            setupprojargs.append("--disable-CASTOR")
            setupprojargs.append("--no-touch-logfile")
            if self.output_name :
                setupprojargs.append("--append=%s" % self.output_name)
            setupprojargs.append("--shell=%s" % self.targetShell())
            setupprojargs.append("LbScripts")
            if opts.scriptsvers :
                setupprojargs.append(opts.scriptsvers)
            if opts.get_python :
                setupprojargs.append("--runtime-project")
                setupprojargs.append("LCGCMT")
                if ev["CMTCONFIG"].startswith("win32_vc71") :
                    setupprojargs.append("5[0-8]*")
                if "slc5" in ev["CMTCONFIG"]:
                    setupprojargs.append("6[0-6]*")
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
#            self.setExtraTags()


            if opts.compat_prepend is None :
                try :
                    SetupProject().main(setupprojargs)
                except ImportError:
                    log.debug("SetupProject failed. Retrying with Compat prepended")
                    self.setupCompat()
                    SetupProject().main(setupprojargs)
            else :
                if opts.compat_prepend :
                    self.setupCompat()
                SetupProject().main(setupprojargs)

        log.debug("%s is set to %s" % ("PATH", ev.get("PATH", "")))
        if sys.platform != "win32" :
            log.debug("%s is set to %s" % ("LD_LIBRARY_PATH", ev.get("LD_LIBRARY_PATH", "")))

    def copyEnv(self):
        ev = self.Environment()
        retenv = dict(ev.env)
        al = self.Aliases()
        retaliases = dict(al.env)
        retextra = self.extra()
        return retenv, retaliases, retextra

    def setEnv(self, debug=False):
        log = logging.getLogger()
        log.debug("Entering the environment setup")
        self.setPath()
        self.setCVSEnv()
        self.setSite()
        self.setCMT()
        self.setCMake()
        self.setSoftLocations()
        self.setSharedArea()

        self.setCMTConfig(debug)
        self.setCMTPath()
        self.setupCompat()

        # this is use internally by the 'lcg-' compiler wrapper.
        self.setLCGhostos()

        # return a copy otherwise the environment gets restored
        # at the destruction of the instance
        return self.copyEnv()

    def manifest(self, debug=False):
        ev = self.Environment()
        opts = self.options
        if opts.log_level != "CRITICAL" :
            self.addEcho("*" * 80)
            if opts.scriptsvers :
                self.addEcho("*" + ("---- LHCb Login %s ----" % opts.scriptsvers).center(78) + "*")
            else :
                self.addEcho("*" + "---- LHCb Login ----".center(78) + "*")
            if self.binary :
                self.addEcho("*" + ("Building with %s on %s %s system (%s)" % (self.compdef, self.platform, self.binary, ev["CMTCONFIG"])).center(78) + "*")
            else : # for windows
                self.addEcho("*" + ("Building with %s on %s system (%s)" % (self.compdef, self.platform, ev["CMTCONFIG"])).center(78) + "*")
            self.addEcho("*" * 80)
            if ev.has_key("CMTPATH") :
                self.addEcho(" --- CMTPATH is set to %s" % ev["CMTPATH"])
            else :
                if ev.has_key("User_release_area") :
                    self.addEcho(" --- User_release_area is set to %s" % ev["User_release_area"])
                if ev.has_key("LHCBPROJECTPATH") :
                    self.addEcho(" --- LHCBPROJECTPATH is set to:")
                    for p in ev["LHCBPROJECTPATH"].split(os.pathsep) :
                        if p :
                            self.addEcho("    %s" % p)


            self.addEcho("-" * 80)


    def main(self):
        debug = False
        for a in self.args :
            if a == "debug" :
                debug = True

        self.setEnv(debug)
        self.manifest(debug)

        self.flush()

        self.setupLbScripts()

        return 0


if __name__ == '__main__':
    sys.exit(LbLoginScript(usage="%prog [options] [debug]").run())

