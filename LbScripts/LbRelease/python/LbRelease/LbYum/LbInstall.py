#!/usr/bin/env python
"""
LHCb Software installer using RPM and YUM
Ben Couturier <ben.couturier@cern.ch>
2012/04/20

Version 1.0

"""
import inspect
import logging
import optparse
import os
import re
import subprocess
import sys
import time
import traceback

# Constants for the dir names
SVAR = "var"
SLIB = "lib"
SRPM = "rpm"
SETC = "etc"
STMP = "tmp"
SUSR = "usr"
SBIN = "bin"

__RCSID__ = "$Id$"


# Checking whether the MYSITEROOT is set correctly
###############################################################################
class LbInstallConfig(object): #IGNORE:R0903
    """ Configuration object for the installer. All options and defaults
    should be kept in an instance of this class """

    def __init__(self, configType):
        """ Constructor for the config object """
        # Get the default siteroot
        self.siteroot = os.environ.get("MYSITEROOT", None)
        # And the default repository URL
        self.repourl = None
        # Debug mode defaults to false
        self.debug = False
        # Use install by default
        self.rpmupdate = False
        # Version of the scripts
        self.script_version = "080812"
        # Default log width
        self.line_size = 120
        # Checking python versions
        self.python_version = sys.version_info[:3]
        self.txt_python_version = ".".join([str(k) for k in self.python_version])
        # Simple logger by default
        self.log = logging.getLogger()
        # Keeping the config type
        self.configType = configType
        # Now importing the config module
        self.configInst = None
        try:
            self.configMod = __import__(self.configType)
            self.configInst = self.configMod.Config()
        except ImportError:
            None
            
# Utility to run a command
###############################################################################
def call(command):
    """ Wraps up subprocess call and return caches and returns rc, stderr, stdout """
    pc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    out, err = pc.communicate()
    rc = pc.returncode
    return (rc, out, err)

# Utility to run a command
###############################################################################
def callSimple(command):
    """ Simpler wrapper for subprocess """
    rc = subprocess.call(command, shell=True)
    return rc

# Check for binary in path
###############################################################################
def checkForCommand(command):
    """ Check whether a command is in the path using which """
    whichcmd = "which %s" % command
    rc, out, err = call(whichcmd) #@UnusedVariable IGNORE:W0612
    return rc, out

# Utilities for log printout
###############################################################################
def printHeader(config):
    """ Prints the standard header as in install_project """
    # Start banner
    thelog = config.log
    start_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    thelog.info('=' * config.line_size)
    thelog.info(('<<< %s - Start of lb-install.py %s with python %s >>>' \
                 % (start_time, config.script_version, config.txt_python_version)).center(config.line_size))
    thelog.info('=' * config.line_size)
    thelog.debug("Command line arguments: %s" % " ".join(sys.argv))

def printTrailer(config):
    """ Prints the standard trailer as in install_project """
    # Trailer
    thelog = config.log
    end_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    thelog.info('=' * config.line_size)
    thelog.info(('<<< %s - End of lb-install.py %s >>>' % (end_time, config.script_version)).center(config.line_size))
    thelog.info('=' * config.line_size)

# Class representing the repository
###############################################################################
class InstallArea(object): # IGNORE:R0902
    """ Class representing the software InstallArea,
    with all related actions"""

    # Initialization of the area
    ##########################################################################
    def __init__(self, config):
        """ Init of the InstallArea, check that all directories and config files
        are present.
        """
        self.config = config
        self.log = logging.getLogger(__name__)

        # Setting the siteroot
        self.siteroot = config.siteroot
        self.log.info("Siteroot is: %s" % self.siteroot)
        if (self.siteroot == None):
            self.log.error("Please specify a site root with the --root option or via the MYSITEROOT env variable")
            sys.exit(1)

        # Setting the main repository URL
        self.repourl = config.repourl
        
        # Making sure the db is initialized
        self.dbpath = os.path.join(self.siteroot, SVAR, SLIB, SRPM)
        self._initRPMDB()

        # Initializing yum config
        self.etc = os.path.join(self.siteroot, SETC)
        self.yumconf = os.path.join(self.etc, "yum.conf")
        self.yumreposd = os.path.join(self.etc, "yum.repos.d")
        
        # tmp directory
        self.tmpdir = os.path.join(self.siteroot, STMP)
        if not os.path.exists(self.tmpdir):
            os.makedirs(self.tmpdir)

        # Local bin directory
        self.usrbin =  os.path.join(self.siteroot, SUSR, SBIN)
        if not os.path.exists(self.usrbin):
            os.makedirs(self.usrbin)
        # Add the local bin to the path
        os.environ['PATH'] = os.pathsep.join([os.environ['PATH'], self.usrbin])

        # Local lib directory
        self.lib =  os.path.join(self.siteroot, SLIB)
        if not os.path.exists(self.lib):
            os.makedirs(self.lib)
        # Add the local bin to the path
        sys.path.append(self.lib)
        # We keep config files compatible with YUM,
        # even though we use the DependencyManager client
        self._initYUM()
        from DependencyManager import LbYumClient
        self.lbYumClient = LbYumClient(self.siteroot)

        # Defining structures and
        # Checking if all needed tools are available
        self.externalStatus = {}
        self.requiredExternals = [ 'rpm' ]
        self.externalFix = {}
        self._checkPrerequisites()

        # And if all the software is there
        self.initfile = None
        self._checkRepository()

    def _initRPMDB(self):
        """ Initialize RPM database """
        log = self.log
        log.info("RPM DB in %s" % self.dbpath)
        if not os.path.exists(self.dbpath):
            log.info("Creating directory %s for RPM db" % self.dbpath)
            os.makedirs(self.dbpath)

        if not os.path.exists(os.path.join(self.dbpath, "Packages")):
            log.info("Initializing RPM db")
            cmd = "rpm --dbpath %s --initdb" % self.dbpath
            log.debug("Command: %s" % cmd)
            rc, stdout, stderr = call(cmd)
            log.debug(stdout)
            log.debug(stderr)
            if rc != 0:
                raise Exception("Error initializing RPM DB: %s" % stderr)

    def _initYUM(self):
        """ Initializes yum configuration.
        Still in use as LbYum uses a compatible configuration """
        if not os.path.exists(self.etc):
            os.makedirs(self.etc)
        if not os.path.exists(self.yumconf):
            ycf = open(self.yumconf, 'w')
            ycf.write(InstallArea._getYumConf(self.siteroot))
            ycf.close()

        if not os.path.exists(self.yumreposd):
            os.makedirs(self.yumreposd)

        # Now calling the configuration method from the specific config module...
        self.config.configInst.initYUM(self)

    def _checkPrerequisites(self):
        """ Checks that external tools required by this tool to perform
        the installation """
        # Flag indicating whether a crucial external is missing and we cannot run
        externalMissing = False

        for e in self.requiredExternals:
            rc, out = checkForCommand(e)
            self.externalStatus[e] = (rc, out)

        for key, val in self.externalStatus.items():
            rc, exefile = val
            if rc == 0:
                self.log.info("%s: Found %s", key, exefile.strip())
            else:
                self.log.info("%s: Missing - trying compensatory measure", key)
                fix = self.externalFix[key]
                if fix != None:
                    fix()
                    rc2, out2 = checkForCommand(key)
                    self.externalStatus[key] = (rc2, out2)
                    if rc2 == 0:
                        self.log.info("%s: Found %s", key, out2)
                    else:
                        self.log.error("%s: missing", key)
                        externalMissing = True
                else:
                    externalMissing = True
        return externalMissing

    def _checkRepository(self):
        """ Checks whether the repository was initialized """
        self.initfile = os.path.join(self.etc, "repoinit")
        if not os.path.exists(self.initfile):
            #self.installRpm("LBSCRIPTS")
            fini = open(self.initfile, "w")
            fini.write(time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime()))
            fini.close()
        # BC: Remove auto update and add specific command instead
        #else:
        #    self._checkUpdates()

    # Pass through commands to RPM and lbYum
    ##########################################################################
    def rpm(self, args):
        """ Wrapper for invocation of RPM """
        install_mode = False
        query_mode = False
        for arg in args:
            if arg.startswith("-i") or arg.startswith("-U"):
                install_mode = True
            if arg.startswith("-q"):
                query_mode = True

        rpmcmd = "rpm --dbpath %s " % self.dbpath
        if not query_mode and install_mode :
            rpmcmd += self.config.configInst.getRelocateCommand(self.siteroot)
        rpmcmd += " ".join(args)

        self.log.info("RPM command:")
        self.log.info(rpmcmd)

        rc = callSimple(rpmcmd)
        return rc

    def listpackages(self, args):
        """ Wrapper for invocation of RPM """
        nameRegexp = None
        if len(args) > 0:
            nameRegexp = args[0]

        totalMatching = 0
        for pa in self.lbYumClient.listPackages(nameRegexp):
            print pa.rpmName()
            totalMatching += 1
        print "Total Matching: %d" % totalMatching

    # Various utility methods to download/check RPMs
    ##########################################################################
    def _checkRpmFile(self, filename):
        """ Checks the integrity of an RPM file """
        ret = False
        rpmcmd = "rpm -K %s " % filename
        self.log.debug("RPM command:")
        self.log.debug(rpmcmd)
        rc, stdout, stderr = call(rpmcmd)
        self.log.debug("Call returned %s %s" % (stdout, stderr))
        if rc == 0:
            ret = True
        return ret

    def _findpackage(self, name, version, cmtconfig):
        """ Find all the packages matching a triplet name, version, config """
        pname = name.upper()
        pnameVer = None
        pnameVerConfig = None
        if version != None:
            pnameVer = pname + "_" + version
        else:
            pnameVer = pname
        if cmtconfig != None:
            pnameVerConfig = pnameVer + "_" + cmtconfig.replace("-", "_")
        else:
            pnameVerConfig = pnameVer

        from DependencyManager import Requires
        req = Requires(pnameVerConfig, None, None, None, "GE", None)
        pack = self.lbYumClient.findLatestMatchingRequire(req)
        res = []
        if pack != None:
            res.append(pack)
        return res

    def _filterUrlsAlreadyInstalled(self, packages):
        """ Filter out RPMs already installed """
        toinstall = []
        for pack  in packages:
            # Establishing the filename
            self.log.debug("Checking for installation of: %s", pack.rpmName())
            if not self._isRpmInstalled(pack.rpmName()):
                toinstall.append(pack)
            else:
                self.log.debug("Already installed: %s will not download and install again" % pack.rpmName())
        return toinstall

    def _downloadfiles(self, installlist, location):
        """ Downloads a list of files """
        import urllib
        files = []
        for pack in installlist:
            filename = pack.rpmFileName()
            full_filename = os.path.join(location, filename)
            files.append(filename)

            # Checking if file is there and is ok
            needs_download = True
            if os.path.exists(full_filename):
                fileisok = self._checkRpmFile(full_filename)
                if fileisok:
                    needs_download = False

            # Now doing the download
            if not needs_download:
                self.log.debug("%s already exists, will not download" % filename)
            else:
                self.log.info("Downloading %s to %s" % (pack.url(), full_filename))
                urllib.urlretrieve (pack.url(), full_filename)
        return files

    def _installfiles(self, files, rpmloc, forceInstall=False, update=False, nodeps=False):
        """ Install some rpm files given the location of the RPM DB """
        fulllist = [ os.path.join(rpmloc, f) for f in set(files) ]
        args = [ "-ivh --oldpackage " ]
        if update or self.config.rpmupdate:
            args = [ "-Uvh" ]
        if forceInstall:
            args.append("--force ")
        if nodeps:
            args.append("--nodeps ")
        args = args + fulllist
        rc = self.rpm(args)
        if rc != 0:
            raise Exception("Error installing rpms")

    def _isRpmInstalled(self, rpmname, rpmversion=None):
        """ Checks whether a given RPM apckage is already installed """
        installed = False
        fullname = rpmname
        if rpmversion != None:
            fullname = rpmname + "." + rpmversion
        rpmcmd = [ 'rpm',  '--dbpath', self.dbpath, '-q', fullname  ]
        self.log.debug("RPM command:" + " ".join(rpmcmd))
        rc, stdout, stderr = call(" ".join(rpmcmd))
        self.log.debug("rpm -q out/err: %s / %s" % ( stdout, stderr))
        if rc == 0:
            installed = True
        return installed

    def _listInstalledPackages(self):
        """ Checks whether a given RPM package is already installed """
        rpmcmd = [ 'rpm',  '--dbpath', self.dbpath, '-qa',  '--queryformat',  '%{NAME} %{VERSION} %{RELEASE}\n'  ]
        self.log.debug("RPM command:" + " ".join(rpmcmd))

        # Shold be improved to yield line per line instead of reading it all in block
        pc = subprocess.Popen(rpmcmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = pc.communicate() #@UnusedVariable IGNORE:W0612
        rc = pc.returncode

        if rc != 0:
            raise LbInstallException("Error reading the list of packages from RPM DB")
        return [ l.split(" ") for l in out.splitlines() ] #IGNORE:E1103

    def _checkUpdates(self, checkOnly):
        """ Check whether packages could be updated in the repository """
        from DependencyManager import Requires, Provides
        self.log.info("Checking for updates")
        packageList = {}
        # First grouping by key/version to make sure we only try to update
        # the newest installed
        for (name, version, release) in self._listInstalledPackages():
            prov = Provides(name, version, release, None, "EQ")
            key = "%s-%s" % (name, version)
            namevers = packageList.get(key, list([]))
            namevers.append(prov)
            packageList[key] = namevers

        updateCount=0
        for key in packageList.keys():
            # Only checking for updates of the last installed version
            newest = sorted(packageList[key])[-1]
            (name, version, release) = (newest.name, newest.version, newest.release)
            # Creating a RPM requirement and checking whether we have a match...
            req = Requires(newest.name, newest.version, None, None, "EQ", None)
            update = self.lbYumClient.findLatestMatchingRequire(req)
            if update != None and update > newest:
                updateCount += 1
                if checkOnly:
                    self.log.warning("%s.%s-%s could be updated to %s"
                                     % (name, version, release, update.rpmName()))
                else:
                    self.log.warning("Updating %s.%s-%s to %s" % (name, version, release, update.rpmName()))
                    self.installRpm(update.name, update.version, update.release, False, True)

        # Writing summary
        if checkOnly:
            self.log.warning("Number of packages to update: %d" % updateCount)
        else:
            self.log.warning("Updated %d packages" % updateCount)

    # Methods to update/check for updates
    ##########################################################################
    def update(self, checkOnly):
        """ Checks whether updates are available and installs them if requested"""
        self._checkUpdates(checkOnly)


    # Methods to download/install RPMs (replacement for yum install)
    ##########################################################################
    def install(self, project, version, cmtconfig):
        """ Perform the whole download/install procedure (equivalent to
        yum install """
        # Looking for the package
        self.log.info("Installing %s/%s/%s" % (project, version, cmtconfig))
        matches = self._findpackage(project, version, cmtconfig)
        self.log.info("Found %d matches" % len(matches))
        for ma in matches:
            self.log.info("Found RPM: %s" % ma.rpmName())
        if len(matches) == 0:
            self.log.error("No matching file found - EXITING")
            raise LbInstallException("No matching file found")
        if len(matches) > 1:
            self.log.error("Multiple files found - EXITING")
            raise LbInstallException("Too many matching file found")

        # Taking the first (and only) entry from the list
        package = matches[0]
        # Now installing the RPM
        if self._isRpmInstalled(package.rpmName()):
            self.log.debug("%s already installed" % package.rpmName())
        else:
            self.installPackage(package)

    def installRpm(self, rpmname, version=None, release=None, forceInstall=False,
                   update=False, nodeps=False):
        """ Install an RPM by name """
        pack = self.lbYumClient.findLatestMatchingName(rpmname, version, release)
        if pack == None:
            raise Exception("InstallRPM: Package %s.%s-%s not found" % (rpmname, version, release))

        self.installPackage(pack, forceInstall, update, nodeps=nodeps)

    def installPackage(self, package, forceInstall = False, update = False, nodeps=False):
        """ install a specific RPM, checking if not installed already """
        self.log.info("Installing %s and dependencies" % package.rpmName())

        # Checking what files should be downloaded
        if nodeps:
            installlist = set([ package])
            self.log.info("Installing %s " % package.rpmName())
            forceInstall = True
        else:
            installlist = set(self.lbYumClient.getAllPackagesRequired(package))
            rpmNames = set([p.rpmName() for p in installlist])
            self.log.info("Found %d RPMs to install:" % len(rpmNames))
            self.log.info(" ".join(rpmNames))

        if len(installlist) == 0:
            raise Exception("Error: No files to download")

        # Now filtering the urls to only keep the onles not already
        # installed. This shouldn't happen but it seems that this
        # happens sometimes with yum...
        finstalllist = self._filterUrlsAlreadyInstalled(installlist)

        self.log.info("Found %d RPMs to install:" % len(finstalllist))
        rpmNames = set([p.rpmName() for p in finstalllist])
        self.log.info(" ".join(rpmNames))

        # Check how many RPMs should be installed
        # if non left, just log and exit
        if len(finstalllist) == 0:
            self.log.info("All packages already installed")
            return

        # Now getting the files...
        files = self._downloadfiles(finstalllist, self.tmpdir)

        # And installing
        self._installfiles(files, self.tmpdir, forceInstall, update, nodeps)

        # And removing installed files
        #XXX
    # Generators for the YumConfigurations
    ##########################################################################
    @classmethod
    def _getYumConf(cls, siteroot):
        """ Builds the Yum configuration from template """
        cfile = """
[main]
#CONFVERSION 0001
cachedir=/var/cache/yum
debuglevel=2
logfile=/var/log/yum.log
pkgpolicy=newest
distroverpkg=redhat-release
tolerant=1
exactarch=1
obsoletes=1
plugins=1
gpgcheck=0
installroot=%(siteroot)s
reposdir=/etc/yum.repos.d
    """ % { 'siteroot':siteroot }
        return cfile

    @classmethod
    def _getYumRepo(cls, name, url):
        """ Builds the Yum repository configuration from template """
        cfile = """
[%(name)s]
#REPOVERSION 0001
name=%(name)s
baseurl=%(url)s
enabled=1
""" % { 'url' : url, 'name': name }
        return cfile

# Class for known install exceptions
###############################################################################
class LbInstallException(Exception):
    """ Custom exception for lb-install """

    def __init__(self, msg):
        """ Constructor for the exception """
        #super(LbInstallException, self).__init__(msg)
        Exception.__init__(self, msg)

# Classes and method for command line parsing
###############################################################################
class LbInstallOptionParser(optparse.OptionParser): #IGNORE:R0904
    """ Custom OptionParser to intercept the errors and rethrow
    them as lbInstallExceptions """

    def error(self, msg):
        raise LbInstallException("Error parsing arguments: " + str(msg))

    def exit(self, status=0, msg=None):
        raise LbInstallException("Error parsing arguments: " + str(msg))

class MainClient(object):
    """ Ancestor for both clients, the new one and the one
    compatible with install project arguments """
    def __init__(self, configType, arguments=None, dryrun=False, prog = "LbInstall"):
        """ Common setup for both clients """
        self.config = LbInstallConfig(configType)
        self.log = logging.getLogger(__name__)
        self.arguments = arguments
        self.dryrun = dryrun
        self.installArea = None
        self.runMethod = None
        self.runArgs = None
        self.prog = prog

        parser = LbInstallOptionParser(usage=usage(self.prog))
        parser.add_option('-d', '--debug',
                            dest="debug",
                            default=False,
                            action="store_true",
                            help="Show debug information")
        parser.add_option('--repo',
                            dest="repourl",
                            default=None,
                            action="store",
                            help="Specify repository URL")
        parser.add_option('--root',
                            dest="siteroot",
                            default=None,
                            action="store",
                            help="Specify MYSITEROOT on the command line")
        parser.add_option('--dryrun',
                            dest="dryrun",
                            default=False,
                            action="store_true",
                            help="Only print the command that will be run")
        parser.add_option('--rpmupdate',
                            dest="rpmupdate",
                            default=False,
                            action="store_true",
                            help="Install with rpm -U instead of -i")

        self.parser = parser

    def main(self):
        """ Main method for the ancestor:
        call parse and run in sequence """
        rc = 0
        try:
            opts, args = self.parser.parse_args(self.arguments)
            # Checkint the siteroot and URL
            # to choose the siteroot
            if opts.siteroot != None:
                self.config.siteroot = opts.siteroot
                os.environ['MYSITEROOT'] = self.config.siteroot

            if opts.repourl != None:
                self.config.repourl = opts.repourl

            # Checking the update options
            self.config.rpmupdate = opts.rpmupdate

            # Now setting the logging depending on debug mode...
            if opts.debug:
                self.config.debug = True
                logging.basicConfig(format="%(levelname)-8s: %(funcName)-25s - %(message)s")
                logging.getLogger().setLevel(logging.DEBUG)

            # Checking if we should do a dry-run
            self.dryrun = self.dryrun or opts.dryrun

            # Getting the function to be invoked
            (runMethod, runArgs) = self.prepareRun(opts, args)
            self.runMethod = runMethod
            self.runArgs = runArgs

            if not self.dryrun:
                # Initializing the install area
                self.installArea = InstallArea(self.config)
                # Find the method instance in the InstallArea
                allmethods = inspect.getmembers(self.installArea, inspect.ismethod)
                found = [ m[1] for m in allmethods if m[0] == runMethod ]
                if len(found) != 1:
                    raise LbInstallException("Could not find method %s in InstallArea" % runMethod)
                # And invoke it...
                found[0](*runArgs) #IGNORE:W0142
            else:
                self.log.info("DRYRUN: %s %s" % (runMethod, " ".join(args)))
            self.postRun()

        except LbInstallException, lie:
            print >> sys.stderr, "ERROR: " + str(lie)
            self.parser.print_help()
            rc = 1
        except:
            print >> sys.stderr, "Exception in lb-install:"
            print >> sys.stderr, '-'*60
            traceback.print_exc(file=sys.stderr)
            print >> sys.stderr, '-'*60
            rc = 1
        return rc

    def prepareRun(self, opts, args):
        """ Hook called before invocation """
        raise NotImplementedError()

    def postRun(self):
        """ Hook called after invocation """
        raise NotImplementedError()

class LbInstallClient(MainClient):
    """ Client following new syntax """

    MODE_INSTALLRPM = "install"
    MODE_RPM     = "rpm"
    MODE_LIST    = "list"
    MODE_UPDATE  = "update"
    MODE_CHECK   = "check"
    MODES = [    MODE_INSTALLRPM, MODE_RPM, MODE_LIST, MODE_UPDATE, MODE_CHECK ]

    def __init__(self, config, arguments=None, dryrun=False, prog="LbInstall"):
        super(LbInstallClient, self).__init__(config, arguments, dryrun, prog)
        self.parser.disable_interspersed_args()
        self.log = logging.getLogger(__name__)

    def prepareRun(self, opts, args):
        """ Main method for the command """

        runMethod = None
        runArgs = []

        # Parsing first argument to check the mode
        if len(args) > 0:
            cmd = args[0].lower()
            if cmd in LbInstallClient.MODES:
                mode = cmd
            else:
                raise LbInstallException("Unrecognized command: %s" % args)
        else:
            raise LbInstallException("Argument list too short")

        # Now executing the command
        if mode == LbInstallClient.MODE_RPM:
            # Mode that passes the arguments to the local RPM
            runMethod = "rpm"
            runArgs = [ args[1:] ]
        elif mode == LbInstallClient.MODE_LIST:
            # Mode that list packages according to a regexp
            runMethod = "listpackages"
            runArgs = [ args[1:] ]
        elif mode == LbInstallClient.MODE_UPDATE or mode == LbInstallClient.MODE_CHECK:
            # Mode that list packages according to a regexp
            runMethod = "update"
            runArgs = [ mode  == LbInstallClient.MODE_CHECK ]
        elif mode == LbInstallClient.MODE_INSTALLRPM:
            # Mode where the RPMs are installed by name
            # Fills in with None if the arguments are not there,
            # Only the name is mandatory
            (rpmname, version, release) = args[1:4] + [ None ] * (4 - len(args))
            if rpmname == None:
                raise LbInstallException("Please specify at least the name of the RPM to install")
            runMethod = "installRpm"

            #If the version is in the name of the RPM then use that...
            m = re.match("(.*)-([\d\.]+)-(\d+)$", rpmname)
            if m != None:
                rpmname = m.group(1)
                version = m.group(2)
                release = m.group(3)
            self.log.info("Installing RPM: %s %s %s" % (rpmname, version, release))
            runArgs =  [rpmname, version, release ]
        return (runMethod, runArgs)

    def postRun(self):
        # No callback needed in this case
        pass

class InstallProjectClient(MainClient):
    """ Backwards compatibility for install_project """

    def __init__(self, configType, arguments=None, dryrun=False):
        super(InstallProjectClient, self).__init__(configType, arguments, dryrun)
        self.log = logging.getLogger(__name__)
        # Adding the binary option for install project compatibility
        self.parser.add_option('--binary',
                        dest="binary",
                        default=None,
                        action="store",
                        help="Download binary package as well as source (install_project mdoe only)")
        self.parser.add_option('-b',
                        dest="useCMTCONFIG",
                        default=False,
                        action="store_true",
                        help="Download binary package matching environment CMTCONFIG  (install_project mdoe only)")

    def prepareRun(self, opts, args):
        """ Main method for the command """

        (pname, pversion, pconfig) = (None, None, None)
        # Check options to see whether we need the binary packages
        if opts.binary != None:
            pconfig = opts.binary
        elif opts.useCMTCONFIG:
            cmtconfig = os.environ.get('CMTCONFIG', None)
            if cmtconfig != None:
                pconfig = cmtconfig
            else:
                raise LbInstallException("CMTCONFIG environment variable must be defined if -b is used")

        # Check the arguments to see if we have project/package and version
        if len(args) == 0:
            raise LbInstallException("Please specify project [version]")
        else:
            pname = args[0]
        if len(args) > 1:
            pversion = args[1]

        # Starting normal install project output
        printHeader(self.config)
        log = self.log
        log.info("Proceeding to the installation of %s/%s/%s" % (pname, pversion, pconfig))
        # Creating the install area object
        runMethod = "install"
        runArgs = [ pname, pversion, pconfig ]
        return (runMethod, runArgs)

    def postRun(self):
        """ Callback after actual execution """
        printTrailer(self.config)

def selectClient(args):
    """ Chooses which client to select depending on command name and command line"""
    retclient = LbInstallClient

    #if len(args) > 0 and args[0] == "install_project.py":
    #    retclient = InstallProjectClient
    #else:
    #    cmdlist = [cmd for cmd in args if cmd in LbInstallClient.MODES]
    #    if len(cmdlist) == 0:
    #        retclient = InstallProjectClient
    return retclient

# Usage for the script
###############################################################################
def usage(cmd) :
    """ Prints out how to use the script... """
    cmd = os.path.basename(cmd)
    return """\n%(cmd)s -  installs software in MYSITEROOT directory'

The environment variable MYSITEROOT MUST be set for this script to work.

It can be used in the following ways:

%(cmd)s install <rpmname> [<version> [<release>]]
Installs a RPM from the yum repository
--rpmupdate can be chose to perform an update instead of a straight install.

%(cmd)s list [<rpmname regexp>]
List packages available in the repositories configured with a name
the regular expression passed.

%(cmd)s rpm <rpm options>...
Pass through mode where the command is delegated to RPM (with the correct DB).

%(cmd)s check 
Checks whether there are any RPMs to update from repo and list them

%(cmd)s update
Checks whether there are any RPMs to update from repo and updates them

""" % { "cmd" : cmd }


def LbInstall(configType = "LCGConfig", prog="LbInstall"):
    logging.basicConfig(format="%(levelname)-8s: %(message)s")
    logging.getLogger().setLevel(logging.INFO)
    client = selectClient(sys.argv)
    sys.exit(client(configType, prog=prog).main())
    

# Main just chooses the client and starts it
if __name__ == "__main__":
    LbInstall()
