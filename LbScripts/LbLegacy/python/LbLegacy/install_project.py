#!/usr/bin/env python
"""
  Script to install a project where you are
  050426 - remove broadcast cmt config , add build vsnet on Win32
  050428 - extract tarinfo if it does not exist
  050502 - introduce system/
  050509 - replace system/ with $CMTCONFIG/
  050509 - use os.system('tar blabla') instead of tarfile if python version < 234
  050511 - replace Visualc with VisualC in cmtbin in Win32
  050511 - if not given take the CMT version from ExtCMT  in the line 'CMT/v'
  050520 - add Online/GaudiOnline in the application list
  050525 - download script tar file instead of individual scripts
  050526 - binary, when required, is $CMTCONFIG
  050527 - print the script version with the help
  050615 - download DIM on contrib as OpenScientist
  050616 - add MOORE and EULER applications, remove Online application
  050708 - check CMTCONFIG, ask for confirmation if CMTCONFIG not part of lhcb_binary
            - add -r or --remove argument to remove a project
                  python install_project.py -p DaVinci -v12r4 -r
              will remove all files and directories referencing DaVinci v12r4
            - remove unreadable tar file and exit
            - add -l or --list argument to list all project versions available on the repository
                  python install_project.py -p DaVinci -l
              will list all DaVinci/version tar files available on the web
            - write a log file in $MYSITEROOT/log/install.log

 050718 - fix remove for LCGCMT
 050826 - run LbInstallArea on Linux to update SoftLinks
 050829 - untar GENSER below lcg/external
 050912 - print the script version number
 050919 - use commands.getstatusoutput instead of os.system to print the output in sequence
 050920 - set Lbcom, Rec, Phys, Geant4_release_area in setLHCbEnv
 050921 - add Examples in application_list for Gaudi, LHCb and Bender
             get the list of examples with versions to configure and build library links
 050922 - if untar is successful remove targz file, replace it with a dummy file
             cmt config packages under DBASE and PARAM
 050927 - replace string.replace with file.replace in getFile
 050930 - add installPackage function ( stuart Patersson) to be called by DIRAC
 050930 - restore os.system on win32 because the commands module is Unix only
 051004 - fix 2 'if <string>.find' which were not tested
 051012 - avoid to extract symlinks on win32
 051108 - replace targz files with a dummy one, correction introduced in 050922 and then lost
           - set CMTBIN in getCMT
           - set CMTSITE, SITEROOT, LD_LIBRARY_PATH  in setLHCbEnv to allow compilation to work

 060124 - add a retry if a download does not work
           - set sys.exit('with a message') instead of sys.exit(integer)
 060301 - execute a SealPluginRefresh
 060306 - remove install_package function which is no longer necessary
 060313 - in pool-refresh return if gaudipath is not set
 060315 - OpenScientist has nolonger InstallArea
 060327 - remove packages in DBASE or PARAM
 060502 - remove GENSER as LCGCMT
 060517 - add some printout in exec_cmt_library_links
 060523 - on Linux platforms download CMT_<version>_Linux.tar.gz
 060601 - on Linux use 'uname -p' to set cmtbin in getCMT
 060614 - add XmlConditions in data_files
 060619 - -b assumes $CMTCONFIG
             --binary=<something> set os.environ['CMTCONFIG']=<something>
             introduce slc3_ia32_gcc323_dbg as possible binary
             run pool_refresh only on Linux for non debug binary
 060621 - if -m do_config is given execute at end of installation on all LHCb projects
             > cmt br cmt config
             > cmt br cmt build library_links
 060622 - introduce full_flag to download source, binary, binary_dbg in one go
 060626 - Bender application is Phys/Bender
 060628 - replace uname -p with uname -m in getCMT to set CMTBIN
 060629 - add VETRA project
 060704 - VETRA has no application yet
 060712 - global variable make was not set : replace it with string 'make'
 060814 = fix exec_cmt_library_links : there was an extra space in checking the bradcast variable
 060829 - do not check existence of InstallArea for DIM as for OpenScientist, LCGCMT and GENSER
 060830 - set CMTPATHPROJECT when cmtvers >= 18
             do not configure packEnv if it does not exist
             do not build library_links if no binary directory in packSys
 060911 - do not use tarfile module from python 2.4.3 on linux
 060925 - print Python version in the logfile
 061013 - add slc4_ia32_gcc34, slc4_ia32_gcc34_dbg
 061106 - download install_project.py as latest_install_project.py.
             check the version, exit if old version
 061115 - protect all print statments with if debug_flag == 1, set default debug_flag = 0
 061116 - print the python version and the time
 061127 - fix lhcb_binary (put slc4_ia32_gcc34_dbg before slc4_ia32_gcc34)
 061128 - import LHCb_config - using getFile
             check definition of logfile before using it in getFile because
             getFile(url_dist,'LHCb_config.py') is called in the main program before setting
             logfile in runInstall
 061204 - use cmtvers instead of cmtversion in cmt_config
 061212 - set here when entering a function which makes a chdir and chdir(here) before return
             set make = 'nmake /f nmake' on win32
             if python_version < 25 : python_version = python_version * 10
 061222 - remove the pool_refresh function to solve the problem of local installation of a given package with the SHARED installtion of POOL
 070126 - several fixes for introduction of CMTPROJECTPATH usage
             use applications_other_executables in exec_cmt_library_links
 070205 - remove os.system(LbInstallArea, keep getstatusoutput to avoid /bin/ls  printout
 070307 - prepare for MCGenerators tar file , for SQLDDDB tar file
 070313 - add a binary directory if not yet there in ext_lhcb project to distinguish different platforms
 070320 - fix getFile in case of ext_lhcb project
 070321 - create DBASE and PARAM when lhcb is created
 070328 - on Linux call LbInstallArea in compile_project if do_config and InstallArea/$CMTCONFIG exist
 070329 - fix mistype in handling of MCGenerators
 070403 - delete CMTPROJECTPATH if it exists when PackEnv is used
 070418 - add some sys.exit('blabla') in untarFile
           - sys.exit if MYSITEROOT is not identical to os.getcwd()
 070420 - use the binary flag in getScripts to choose the right script tar file
           - do not set make_flag to do_config by default if binary contains 'win32'
 070511 - raise exception if we can not delete file
 070709 - Ugly hack to avoid the building of library links for already present projects
 070809 - added the md5 checks of the tar balls and the customization of the retries
 070810 - added message in the logfile in case of md5 check failure
 071004 - fixed a couple of bugs occuring when the download (or md5sum check) fails
 071204 - caught exception and remove tar file if the tar command fails
 071211 - added the total removal of the project if its tar ball fails to untar
 080108 - fixed the import of the ExtractError class from the tarfile module
 080115 - always pass the binary value to the LbInstallArea script
 080211 - moved the exception catching for the tar file expansion
 080407 - added the automatic download of the Grid software
 080424 - excluded the 2.4.4 version from the usable python tarfile module
 080429 - fixed stupid typo for the previous change
 080429 - added case insensitive checks for MYSITEROOT on windows
 080429 - removed the download of the grid tar balls on windows and slc3
 080429 - fixed configure step for LCGGrid
 080430 - fixed logic for the slc3 exclusion of LHCbGrid
 080709 - fixed unix permissions after the untar of the file
 080813 - implemented the support for multiple mysiteroot
 080820 - added the permissions for writing to the group and other according to the umask
 080820 - moved to the use of the 'tar' command only. Except for windows
 080822 - changed the error message reported by chmod. Now using the full path
 080825 - added check to the removall of the path. It should exist.
 080825 - added the read permissions if the mask allows them.
 080826 - refine fixing of the permissions: only for the exact installed tar balls.
 080827 - added the '--touch' option to the tar command.
 080901 - added the global check for the installation of a project.
 080902 - Fixed permissions on the DBASE/cmt and PARAM/cmt directories.
 080903 - Fixed permissions after cmt configure.
 080903 - removed the '-no-same-permissions' option of the tar command. Should fix problem with untarring
          in the same directory already existing files installed by a different account.
 080903 - Fixed the global umask at the start of the script.
 080907 - implemented untarring in a temporary local directory.
 080908 - added '-s', '--setup-script' options to generate the setup of the top installed project.
 080908 - fixed windows temporary directory name.
 080909 - added the '--silent' option to the call to SetupProject.
 080916 - refactoring of the output messages using the python logging facility.
 080916 - use the function name in the logger only if we have python 2.5.
 080916 - fix the destination directory creation after untarring in the tmp directory.
 080917 - change the permissions of the newly created base directories.
 080922 - Fixed problem in the recusive function to move the file from the directory where
          the untarring has happened.
        - Fix link problem in the removeAll function (Vladimir Romanovskiy)
        - added the read/write permissions after untarring on windows too.
        - fixed mistake in the previous change. Have to use the absolute path in the os.walk function
 080924 - fixed typo in compile_project
 081008 - enforces the removal of the reference md5sum file whenever a new one is downloaded
 081009 - fixed issues with project without versioned-directory for the packages
 081009 - fixed careless removal of the CVS directory from the list
 081010 - faked version modification to force the pickup of the latest changes from 081009
 081013 - Fixed configuration of standalone data packages.
 081015 - Fixed the removal of data packages. Corrected the setup of Boole v12r10
 081016 - Fixed the removal of packages if it is a patched version (vXrYpZ).
 081017 - Fixed the global configure step. A separate installation is now done for LbScripts
 081022 - added full support for multiple mysiteroot. added the --check option.
 081029 - moved to LbScripts v1r8
 081031 - moved to LbScripts v1r9
 081104 - Creates relative links to LbLogin instead of absolute ones. It fixes problem with the
          synchronisation of the CERNVM server.
 081202 - added the fixing of the windows attributes in the changePermissions function
 081203 - fixed the attributes tuning for the files
 081205 - moved to LbScripts v1r11
 081209 - Changed to default behavior. Now only appends soft. There is a new option --overwrite to
          force the overwriting
 081216 - computes the CMTDEB variable on the fly
 081217 - Fixed trailing '/' at the end of the MYSITEROOT components
 090107 - fixed the installation of the LbLogin.bat script on windows
 090119 - fixed the removal of the .html file if the project is not available
 090120 - moved to the version v1r13 for LbScripts
 090120 - always install the LbScripts project locally if it is not present
 090210 - moved to LbScripts v2r4
 090216 - suppressed the call to LbInstallArea. added the delayed call to generateLogin
 090218 - moved to LbScripts v2r5
 090302 - fixed creation of intermediate in shutil.copytree which was not working in python 2.4
        - fixed getCMTPath which was not returning a tuple some cases
 090303 - removed obsolete putenv calls
        - put some checks on the parent directory whenever using shutil.move
 090311 - moved to LbScripts v2r6. Prevent the generation and usage of the cache
 090317 - moved to LbScripts v2r7
 090324 - removed every obsolete and uneeded features (config, compile, library links etc)
        - fixed the python version calculation with sys.version_info
        - refactored the name of the functions to comply with the convention
        - implemented the path stripping for the generation of the project setup script
 090325 - factored out the lbscripts version.
        - call LbLogin before generating the setup script
 090401 - fixed small hickup in the installation of a local data package.
 090406 - removed the obsolete string module
        - moved to LbScripts v3r1
 090414 - moved to use the LbScript tarball for bootstrapping.
        - Use the LbLegacy.LHCb_config for the old settings.
        - moved version of LbScripts to be used to be the current one (v3r2).
 090421 - comment the full_flag setting in win32 section.
        - put the import statement for LHCb_config.
 090513 - Added the support for slc5
        - removed the usage of the old scripts.tar.gz for the bootstrapping.
 090515 - Moving to LbScripts v4r0
 090526 - Reenabled the fixProjectLinks procedure to cure the link target starting with "/afs"
 090602 - Added AppConfig and L0TCK to the removal process.
 090602 - Moved to LbScripts v4r1
 090618 - fixed bootstrap order for the removal mode.
 090709 - Don't try to create a log file in check-mode.
 090804 - Moved the Compat project verson to v1r2
 090819 - use absolute patch for the local distribution.htm file
        - put security around the usage of fixLinks. 
 090821 - implemented retry loop for the software retrieval.
 090916 - Added the --nofixperm option to prevent the fixing of the permissions. This
          could improve the installation time on windows 
 091013 - Added the support for the LCGGanga tarball.
 091015 - fixed the --check option. It must not create any file.
 091026 - fixed the exception handling in the retrieve function
 091105 - stripped off the hat for the data package if there is one.
"""
#------------------------------------------------------------------------------
import sys, os, getopt, time, shutil
import atexit
import stat
import commands
import logging
import random
import socket
from urllib import urlretrieve, urlopen, urlcleanup
from shutil import rmtree

script_version = '091105'
python_version = sys.version_info[:3]
txt_python_version = ".".join([str(k) for k in python_version])
lbscripts_version = "v4r3"
compat_version = "v1r2"
#-----------------------------------------------------------------------------------

# url from which to get files
url_dist = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'

# list of subdirectories created in runInstall
subdir = ['lcg','lhcb','contrib','html','targz', 'tmp']

# dynamic modules

LbLegacy = None
LbConfiguration = None


# base directories
log_dir     = None
contrib_dir = None
lcg_dir     = None
lhcb_dir    = None
html_dir    = None
bootscripts_dir = None
targz_dir   = None
system_dir  = None
tmp_dir     = None

# global creation mask
globalmask = None

# multiple mysiteroot
multiple_mysiteroot = False

# build flags
full_flag = False
make_flag = None

# cmt stuff
cmtconfig = None
setup_script = None
cmtversion = 0
grid_version = 0

# various flags
debug_flag = False
list_flag = False
remove_flag = False
make_flag = ' '
md5_check = True
nb_retries = 3
check_only = False
overwrite_mode = False
fix_perm = True

#----------------------------------------------------------------------------------
def usage() :
    print 'Usage:'
    print '  python install_project.py -p <project> -v <version> [-b] '
    print 'version %s - Try "python install_project.py -h" for more information.'% script_version
    sys.exit()

def help() :
    print ' install_project.py - version %s:  install a project in the current directory '% script_version
    print """
    Usage:
      python install_project.py -p <project> -v <version> [-b| --binary=<bin> ] [-d or --debug] [-m <global|select>]

          $CMTCONFIG  is the binary directory name
      creates log/         to receive log files
              lcg/         to receive lcg software
              lhcb/        to receive lhcb software
              contrib/     to receive CMT and OpenScientist
              targz/       to receive tar files from the web
              $CMTCONFIG/  to receive runtime libraries
      get the list of projects to download
      download project sources
      if binaries are required: download project binaries
      otherwise compile project sources
      -d or --debug      : to print more info
      -l or --list       : to list the <project>_<version>_*.tar.gz files available on the web
      -r or --remove     : remove the <project>/<version>
      -c or --cmtversion : download this CMT version
      -h or --help       : to print this help
      -b                 : to import $CMTCONFIG binaries
      --binary=<bin>     : to import another binary (i.e. $CMTCONFIG_dbg on Linux)
                           this triggers a 'cmt broadcast cmt config' of all projects but LCGCMT
      -f                 : to import source, $CMTCONFIG binaries, $CMTCONFIG_dbg binaries and
                           to make a 'cmt broadcast cmt config' of all projects but LCGCMT
      -n or -nocheck     : no md5 check of the tar balls
      --retry=<nb>       : nb of retries for the download (default=1)

    Perequisite:
      requires python version >= 2.3.4 on Win32 and python >=2.3 on Linux
      the following environment variables should have been set before invoking the script:
            $MYSITEROOT is the full path of the current directory
            $MYSITEROOT is not a link.
            > cd $MYSITEROOT
            the answer given by the pwd command should be strickly identical to $MYSITEROOT
            $CMTCONFIG  is the binary directory name
            if you want to download binaries $CMTCONFIG should be identical to
            one the LHCb CERN platforms
    Remarks:
      scripts and system tar.gz files are always download
      project tar.gz files are not download if they already exist.
      tar.gz file which cannot be untared is removed and a message is printed
      """
    sys.exit()
    
#----------------------------------------------------------------------------------

def initRandSeed():
    random.seed("%d-%s" % (os.getpid(), socket.getfqdn()))

_block_count = 0
_block_size  = 0
_file_size   = 0
_cur_size    = 0

def reportHook(bcount, bsize, fsize):
    global _block_count, _block_size, _file_size, _cur_size
    _block_count = bcount
    _block_size  = bsize
    _file_size   = fsize
    _cur_size   += bcount * bsize 

_retry_time = 120.0

def retrieve(url, dest):
    global _block_count, _block_size, _file_size, _cur_size
    log = logging.getLogger()
    retrieved = False
    local_retries = nb_retries + 1
    while not retrieved and local_retries :
        try :
            retrieved = True
            fname, finfo = urlretrieve(url, dest, reportHook)
        except :
            retrieved = False
            log.error("Failed to retrieve %s" % url)
            if os.path.exists(dest) :
                os.remove(dest)
        local_retries -= 1
        if local_retries and not retrieved:
            sleep_time = _retry_time + random.uniform(-_retry_time/2, _retry_time/2)
            log.warning("Sleeping for %f second before retrying" % sleep_time)
            time.sleep(sleep_time)
    log.debug("Block count: %d\tBlock size: %d\tFile size:%d\tRetrieved: %d" % (_block_count, _block_size, _file_size, _cur_size) )
    _block_size  = 0
    _block_count = 0
    _file_size   = 0
    _cur_size    = 0

    return fname, finfo


#----------------------------------------------------------------------------------

_postinstall_commands = {}


def registerPostInstallCommand(project, command, dirname=None):
    global _postinstall_commands
    log = logging.getLogger()
    if _postinstall_commands.has_key(project) :
        cmdlist = _postinstall_commands[project]
    else :
        cmdlist = []
    cmdlist.append((command,dirname))
    _postinstall_commands[project] = cmdlist
    if dirname :
        log.debug("Registered PostInstall for %s: \"%s\" in %s" % (project, command, dirname) )
    else :
        log.debug("Registered PostInstall for %s: \"%s\"" % (project, command) )


def callPostInstallCommand(project):
    log = logging.getLogger()
    projcmds = _postinstall_commands.get(project, None)
    if projcmds :
        for c in projcmds :
            if c[1] :
                os.chdir(c[1])
            os.system("%s" % c[0])
            log.info("Executing PostInstall for %s: \"%s\" in %s" % (project, c[0], c[1]) )
    else :
        log.debug("Project %s has no postinstall command" % project)


def isProjectRegistered(project):
    registered = False
    if project in _postinstall_commands.keys() :
        registered = True
    return registered

#----------------------------------------------------------------------------
#
#  helper function to remove a directory recursively
#  This functionality is not implemented in some python version
#
def removeAll(path):
    """
    Recursively remove directories and files.
    """
    try :
        rmtree(path, ignore_errors=True)
    except :
        if os.path.exists(path) :
            if os.path.isdir(path) and not os.path.islink(path):
                lst = os.listdir(path)
                for p in lst:
                    removeAll(os.path.join(path,p))
                os.rmdir(path)
            else:
                os.remove(path)

def addPermissions(authbits, directory, recursive=True):
    wholist = ""
    authlist = []
    if authbits & stat.S_IRUSR != 0 :
        wholist += "u"
    if authbits & stat.S_IRGRP != 0 :
        wholist += "g"
    if authbits & stat.S_IROTH != 0 :
        wholist += "o"
    if wholist != "" :
        authlist.append("%s+rX" % wholist)
    wholist = ""
    if authbits & stat.S_IWUSR != 0 :
        wholist += "u"
    if authbits & stat.S_IWGRP != 0 :
        wholist += "g"
    if authbits & stat.S_IWOTH != 0 :
        wholist += "o"
    if wholist != "" :
        authlist.append("%s+w" % wholist)
    if authlist :
        if recursive :
            chmodcmd = "chmod -R %s %s" % (",".join(authlist), directory)
        else :
            chmodcmd = "chmod -R %s %s" % (",".join(authlist), directory)
        commands.getstatusoutput(chmodcmd)




def getGlobalMask():
    global  globalmask
    log = logging.getLogger()
    if globalmask is None:
        themask = os.umask(0)
        os.umask(themask)
        authbits1 = 0777 - themask
        basedir = os.environ["MYSITEROOT"].split(os.pathsep)[0]
        authbits2=stat.S_IMODE(os.stat(basedir)[stat.ST_MODE])
        authbits = authbits1 | authbits2
        globalmask = 0777 - authbits
        os.umask(globalmask)
        log.debug("Calculating global permissions:")
        log.debug("Read/write permissions according to mask %04o are %04o" % (themask, 0777-themask))
        log.debug("Read/write permissions according to MYSITEROOT are %04o" % authbits2 )
    return globalmask

def changePermissions(directory, recursive=True):
    """ change permissions according to the umask. and the
    MYSITEROOT directory permissions """
    log = logging.getLogger()
    here = os.getcwd()
    if sys.platform != "win32" :
        # add write permissions to the user if needed
        authbits = 0777 - getGlobalMask()
        if recursive and os.path.isdir(directory):
            log.debug( "Adding read/write permissions %04o recursively to %s" % (authbits, directory) )
        else :
            log.debug( "Adding read/write permissions %04o to %s" % (authbits, directory))
        addPermissions(authbits, directory, recursive)
    else :
        if recursive :
            log.debug( "Adding read/write permissions recursively to %s" % directory )
            os.chmod(directory, stat.S_IWRITE)
            for root, dirs, files in os.walk(directory, topdown=True) :
                for d in dirs :
                    os.chmod(os.path.join(root, d), stat.S_IWRITE)
                for f in files :
                    os.chmod(os.path.join(root, f), stat.S_IWRITE)
        else :
            log.debug( "Adding read/write permissions to %s" % directory)
            os.chmod(directory, stat.S_IWRITE)
        if os.path.isdir(directory):
            if os.listdir(directory) :
                os.chdir(directory)
                os.system("attrib -R -A /S /D")
                os.chdir(here)
        else :
            os.system("attrib -R -A %s" % directory)

def checkWriteAccess(directory):
    dirok = True
    log = logging.getLogger()

    canwrite = os.access(directory, os.W_OK)
    if not canwrite :
        log.warning( "No POSIX write permission in %s" % directory )
    dirok = dirok & canwrite

    canread = os.access(directory, os.R_OK)
    if not canread :
        log.warning("No POSIX read permission in %s - cannot list directory" % directory )
    dirok = dirok & canread

    canexe = os.access(directory, os.X_OK)
    if not canexe :
        log.warning( "No POSIX execute permission in %s - cannot cd in there" % directory )
    dirok = dirok & canexe


    return dirok

#----------------------------------------------------------------------------

def getTmpDirectory():
    return tmp_dir.split(os.pathsep)[0]

def createTmpDirectory():
    log = logging.getLogger()
    if os.path.isdir(getTmpDirectory()) :
        log.info( '     %s exists' % getTmpDirectory() )
        destroyTmpDirectory()
    os.mkdir(getTmpDirectory())
    if fix_perm :
        changePermissions(getTmpDirectory(), recursive=True)
    log.info( '     %s created' % getTmpDirectory() )

def destroyTmpDirectory():
    log = logging.getLogger()
    log.info( '     Removing %s' % getTmpDirectory())
    removeAll(getTmpDirectory())

def cleanTmpDirectory():
    log = logging.getLogger()
    log.info('     Cleaning up %s' % getTmpDirectory())
    tmpdir = getTmpDirectory()
    for f in os.listdir(tmpdir) :
        removeAll(os.path.join(tmpdir,f))

#----------------------------------------------------------------------------

def getScriptExt():
    ext = 'sh'
    if sys.platform == 'win32':
        ext = 'bat'
    return ext

def getSourceCmd():
    command = '.'
    if sys.platform == 'win32':
        command = 'call'
    return command
#----------------------------------------------------------------------------
#
# create subdirectories if they don't exist ===============================
#
def createDir(here , logname):
    log = logging.getLogger()

    log.info('create necessary sub-directories' )

    this_log_dir = log_dir.split(os.pathsep)[0]


    if not os.path.isdir(this_log_dir):
        os.mkdir(this_log_dir)
    else:
        if os.path.exists(os.path.join(this_log_dir,logname+'_old')):
            os.remove(logname+'_old')
        if os.path.exists(logname):
            os.rename(logname,logname+'_old')

    this_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    log.info(" =========== Python %s %s " % (txt_python_version, this_time) )
    for dirnm in subdir:
        if os.path.isdir(os.path.join(here, dirnm)):
            log.debug('%s exists in %s '%(dirnm, here))
        else:
            os.mkdir(dirnm)
            log.info('%s is created in %s '%(dirnm, here))
            if dirnm == 'lcg':
                os.mkdir(os.path.join(dirnm, 'external'))
                log.info('%s is created in %s ' % (os.path.join(dirnm, 'external'), here))
            if dirnm == 'lhcb':
                if multiple_mysiteroot :
                    found_dbase = False
                    found_param = False
                    for b in lhcb_dir.split(os.pathsep)[1:] :
                        if os.path.isdir(os.path.join(b, 'DBASE')) :
                            found_dbase = True
                        if os.path.isdir(os.path.join(b, 'PARAM')) :
                            found_param = True
                    if not found_dbase :
                        os.mkdir(os.path.join(dirnm, 'DBASE'))
                    if not found_param :
                        os.mkdir(os.path.join(dirnm,'PARAM'))
                    if found_dbase or found_param :
                        os.mkdir(os.path.join(dirnm, 'EXTRAPACKAGES'))
                        os.mkdir(os.path.join(dirnm, 'EXTRAPACKAGES', 'cmt'))
                        f = open(os.path.join(dirnm, 'EXTRAPACKAGES', 'cmt', 'project.cmt'), "w")
                        f.write("project EXTRAPACKAGES \n\n")
                        f.close()
                else :
                    os.mkdir(os.path.join(dirnm, 'DBASE'))
                    os.mkdir(os.path.join(dirnm, 'PARAM'))
            if fix_perm :
                changePermissions(dirnm, recursive=True)
        if dirnm == "tmp" :
            createTmpDirectory()
    if os.path.isdir(os.path.join(here, cmtconfig)):
        log.info('%s exists in %s '%(cmtconfig, here))
    else:
        os.mkdir(cmtconfig)
        log.info('%s is created in %s '%(cmtconfig,here))

#
#  install CMT if not there ===============================================
#
def getCMT(version=0):
    log = logging.getLogger()
    log.debug('install CMT if not there')
    here = os.getcwd()

    this_contrib_dir = contrib_dir.split(os.pathsep)[0]
    this_targz_dir = targz_dir.split(os.pathsep)[0]

    # get the CMT version number from ExtCMT
    if version == 0:
        try :
            import LbConfiguration.External
            cmtvers = LbConfiguration.External.CMT_version
            log.debug("Extracting CMT version from LbConfiguration.External: %s" % cmtvers)
        except ImportError :
            cmtvers = "v1r20p20090520"
            log.debug("Using CMT version %s" % cmtvers)
    # get the cmt version number from the argument
    else:
        cmtvers = version
        log.debug('get cmtversion from argument %s '% cmtvers)

    log.info('CMT version %s ' % cmtvers)

    # choose the binary name
    platform = sys.platform
    if sys.platform == 'win32':
        cmtbin = 'VisualC'
    elif sys.platform == 'mac':
        cmtbin = 'Darwin-PowerMacintosh'
    elif sys.platform.find('linux') != -1:
        platform = 'linux'
        hw = commands.getstatusoutput('uname -m')[1]
        cmtbin = 'Linux-'+hw

    os.environ['CMTBIN'] = cmtbin

    file ='CMT_'+cmtvers+'_'+platform+'.tar.gz'
    # if the version does not exist get it from the web site
    if overwrite_mode or not isInstalled(file):
        os.chdir(this_contrib_dir)

        # get the tar file
        log.info('get %s' % os.path.join(this_contrib_dir, 'CMT', cmtvers, platform))
        getFile(url_dist+'source/','CMT_'+cmtvers+'_'+platform+'.tar.gz')
        # untar it
        checkWriteAccess(this_contrib_dir)
        os.chdir(this_contrib_dir)
        rc = unTarFileInTmp(os.path.join(this_targz_dir,file), os.getcwd(), overwrite=overwrite_mode)
        if rc != 0 :
            removeAll(os.path.join(this_contrib_dir, 'CMT'))
            log.info('CMT directory removed')
            sys.exit("getCMT: Exiting ...\n")

        # install CMT
        os.chdir(os.path.join(this_contrib_dir,'CMT',cmtvers,'mgr'))
        if sys.platform == 'win32':
            rc = os.system('call INSTALL.bat')
        else:
            output = commands.getstatusoutput('./INSTALL')[1]
            for l in output.split("\n") :
                log.debug(l)
        log.info('install CMT %s' % cmtvers)
        setInstalled(file)
    else:
        log.info( 'CMT %s is already installed' % cmtvers)

    os.environ['CMTROOT'] = os.path.join(this_contrib_dir,'CMT',cmtvers)
    newpath = os.path.join(os.getenv('CMTROOT'),cmtbin)+os.pathsep+os.getenv('PATH')
    os.environ['PATH'] = newpath

    if debug_flag :
        log.debug( 'CMTROOT %s' % os.getenv('CMTROOT'))
        log.debug( 'PATH %s ' % os.getenv('PATH'))
        log.debug( 'CMTBIN %s '% os.getenv('CMTBIN'))

    os.chdir(here)


#
#  download a file from the web site in targz_dir =================================
#
def getFile(url,file):
    log = logging.getLogger()
    log.debug('%s/%s ' % (url, file))

    if file.find('.tar.gz') != -1:
        filetype = 'x-gzip'
        this_targz_dir = targz_dir.split(os.pathsep)[0]
        dest = os.path.join(this_targz_dir,file)
        if url.find('scripts') != -1 or url.find('system') != -1:
            if os.path.exists(dest) :
                try:
                    os.remove(dest)
                except:
                    log.info('can not remove file %s' % dest)
        else:

    # check existence of the project
    # if it exists then return
            pack_ver = getPackVer(file)
            bin = pack_ver[2]
            file_base = pack_ver[4]
            for f in file_base :
                if bin != '':
        # download binary tar file if InstallArea is not there
                    if pack_ver[0] in LbLegacy.LHCb_config.ext_lhcb:
                        f = os.path.join(f,bin)
                    else:
                        f = os.path.join(f,'InstallArea',bin)
                    if os.path.exists(f):
                        if overwrite_mode :
                            exist_flag = False
                            log.info('%s already exist - Overwriting' % f)
                        else :
                            exist_flag = True
                            log.info('%s already exist - do not overwrite' % f)
                        return exist_flag
                else:
        # check existence of the file_path
        # if it exists then return
                    exist_flag = os.path.exists(f)
                    if exist_flag == True:
                        if overwrite_mode :
                            exist_flag = False
                            log.info('%s on %s exists - Overwriting' % (file, f))
                        else :
                            log.info('%s on %s exists - do not overwrite' % (file, f))
                        return exist_flag
                    else:
                        log.info('%s on %s' % (file, os.getcwd()))

    elif file.find('.html') != -1 or file.find('.htm') != -1 :
        this_html_dir = html_dir.split(os.pathsep)[0]
        dest = os.path.join(this_html_dir,file)
        filetype = 'html'

    else:
        filetype = 'text'
        if file.find('.py') != -1:
            dest = file
            if file == 'install_project.py' : dest = 'latest_install_project.py'
            if os.path.exists(dest):
                try:
                    os.remove(dest)
                except:
                    log.warning('can not remove file %s' % dest)


    exist_flag = False
    log.debug('%s on %s' % (file, dest))

    if not os.path.exists(dest):
        local_retries = nb_retries + 1
        hasbeendownloaded = False
        while ( not hasbeendownloaded and (local_retries>0) ) :
            h =  retrieve(url+'/'+file,dest)[1]
            if h.type.find(filetype) == -1:
                log.warning('cannot download %s - retry' % file)
                os.remove(dest)
            else:
                if md5_check and isTarBall(dest):
                    if not checkMD5(url,file,os.path.dirname(dest)):
                        removeReferenceMD5(file, os.path.dirname(dest) )
                        os.remove(dest)
                        log.error('md5 check failed on %s' % file)
                    else:
                        hasbeendownloaded = True
                else:
                    hasbeendownloaded = True
            local_retries = local_retries - 1

        if not hasbeendownloaded:
            sys.exit('Cannot download %s after %s attempts - exit ' %(file,nb_retries)+'\n')

    return exist_flag

def isTarBall(filename):
    fc = filename.split(".")
    if fc[-1] == "gz" and fc[-2] == "tar" :
        return True
    else :
        return False

# --------------------------------------------------------------------------------------

def calculateMD5(filename):
    import md5
    m = md5.new()
    f = open(filename,"rb")
    buf = f.read(2**13)
    while(buf):
        m.update(buf)
        buf = f.read(2**13)
    return m.hexdigest()

def removeReferenceMD5(fnm, dest):
    log = logging.getLogger()
    md5name = getMD5FileName(fnm)
    filename = os.path.join(dest,md5name)
    if os.path.exists(filename):
        try:
            os.remove(filename)
        except:
            log.warning('cannot remove file %s' % filename)

def getReferenceMD5(url, filen, dest):
    log = logging.getLogger()
    md5name = getMD5FileName(filen)
    filename = os.path.join(dest,md5name)
    if md5name :
        if os.path.exists(filename) :
            os.remove(filename)
        filename = retrieve(url+'/'+ md5name, filename )[0]
    else:
        log.warning('cannot retrieve %s' % md5name)
    for line in open(filename,"r").readlines():
        md5sum = line.split(" ")[0]
    return md5sum

def checkMD5(url, filenm, dest):
    log = logging.getLogger()
    isok = False
    log.info("Checking %s tar ball consistency ..." % filenm)
    refmd5 = getReferenceMD5(url, filenm, dest)
    if debug_flag :
        log.info("   reference md5 sum is: %s" % refmd5)
    compmd5 = calculateMD5(os.path.join(dest,filenm))
    log.info("       local md5 sum is: %s" % compmd5)
    if refmd5 == compmd5 :
        isok = True
    return isok

def getMD5FileName(filename):
    fc = filename.split(".")
    name = ".".join(fc[:-2]) + ".md5"
    return name

# --------------------------------------------------------------------------------------

def getHTMLFileName(filename):
    fc = filename.split(".")
    name = ".".join(fc[:-2]) + ".html"
    return name
#
#  get package name, version and binary =======================================
#
def getPackVer(file):
    log = logging.getLogger()
    log.debug('extract package name and version from %s' % file)

    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]


    # get the binary if any
    if file.find("LBSCRIPTS") == -1 :
        import LbConfiguration.Platform
        binary_list = LbConfiguration.Platform.binary_dbg_list + LbConfiguration.Platform.binary_opt_list
        for b in binary_list:
            if file.find(b) != -1:
                bin = b
                if file.find('_'+b) != -1 :
                    ffile = file[:file.find('_'+b)]
                else :
                    ffile = file[:file.find('.tar.gz')]
                break
            else:
                bin = ''
                ffile = file[:file.find('.tar.gz')]
    else :
        bin = ''
        ffile = file[:file.find('.tar.gz')]
    packver = ffile.split('_')
    vers = packver[-1]
    name = packver[0]
    file_path = os.path.join(this_lhcb_dir,name,name+'_'+vers)
    base_dir = lhcb_dir.split(os.pathsep)
    file_base = []
    if bin + ".tar.gz" == file :
        name = ffile
        vers = ''
        file_path = os.path.join(os.environ["MYSITEROOT"].split(os.pathsep)[0], name)
    for bd in base_dir :
        if bin + ".tar.gz" == file :
            file_base.append(os.path.join(bd,"..",name))
        else :
            file_base.append(os.path.join(bd, name, name+'_'+vers))
    if name == "LBSCRIPTS" :
        file_base = []
        file_base.append(os.path.join(base_dir[0],name,name+'_'+vers))
    if name == "LCGCMT" or name == "GENSER" or name == "LCGGrid" or name == "LCGGanga":
        if len(packver) >= 2:
            vers = '_'.join(packver[1:])
            file_path = os.path.join(this_lcg_dir,name,name+'_'+vers)
            base_dir = lcg_dir.split(os.pathsep)
            file_base = []
            for bd in base_dir :
                file_base.append(os.path.join(bd, name, name+'_'+vers))
    if name == "OpenScientist" or name == "osc_vis" or name == "DIM" :
        file_path = os.path.join(this_contrib_dir,name,vers)
        base_dir = contrib_dir.split(os.pathsep)
        file_base = []
        for bd in base_dir :
            file_base.append(os.path.join(bd, name, vers))
    if packver[0] == "DBASE" or packver[0] == "PARAM":
        if len(packver) == 3:
            name = packver[1]
        else:
            name = os.path.join(packver[1],packver[2])
        file_path = os.path.join(this_lhcb_dir,packver[0],name,vers)
        base_dir = lhcb_dir.split(os.pathsep)
        file_base = []
        if len(base_dir) > 1 :
            file_base.append(os.path.join(this_lhcb_dir, 'EXTRAPACKAGES', name, vers))
        for bd in base_dir :
            file_base.append(os.path.join(bd, packver[0], name, vers))
    return (name, vers, bin, file_path, file_base)

#
#  get the project_list =====================================================
#
def getProjectList(name,version,binary=' '):
    log = logging.getLogger()
    log.debug('get list of projects to install %s %s %s' % (name, version, binary))
    here = os.getcwd()

    import LbLegacy.LHCb_config
    import LbConfiguration.Platform

    if LbLegacy.LHCb_config.data_files.has_key(name) == 1:
        tar_file = LbLegacy.LHCb_config.data_files[name]+'_'+version
    else:
        tar_file = name.upper()+'_'+name.upper()
        if version != 0 :
            tar_file = tar_file + '_' + version
        if binary != ' ': tar_file = tar_file+'_'+binary

    this_html_dir = html_dir.split(os.pathsep)[0]

    if not check_only :
        checkWriteAccess(this_html_dir)
    os.chdir(this_html_dir)
    if not check_only :
        getFile(url_dist+'html/',tar_file+'.html')
    
    disthtm = os.path.join(this_html_dir,"distribution.htm")
    
    if not check_only :
        if os.path.exists(disthtm) :
            os.remove(disthtm)
        log.debug("Downloading %s/%s" % (url_dist + "html", disthtm) )
        getFile(url_dist + "html", disthtm)

    # loop over projects to be downloaded
    project_list = {}
    html_list = []
    tar_file_html = tar_file + '.html'
    if os.path.exists(tar_file_html) :
        fd = open(tar_file_html)
    else :
        log.error("File %s doesn't exist" % os.path.join(this_html_dir,tar_file_html))
        log.warning("%s %s %s is not installed" % (name, version, binary))
        sys.exit("some projects are not installed. Exiting ...")

    fdlines = fd.readlines()
    for fdline in fdlines:
        if fdline.find('was not found on this server') != -1:
            log.info('the required project %s %s %s is not available' % (name, version, binary))
            log.info('remove %s.html and exit ' % tar_file)
            fd.close()
            os.remove(tar_file+'.html')
            sys.exit(' %s %s %s is not available' %(name,version,binary)+'\n')
        if fdline.find('HREF=') != -1:
            eq_sign = fdline.find('HREF=')
            gt_sign = fdline.find('>')
            slash_sign = eq_sign + fdline[eq_sign:].find('/')
            source = fdline[eq_sign+5:slash_sign]
            file   = fdline[slash_sign+1:gt_sign]
            project_list[file] = source
            html_list.append(file)


    for file in project_list.keys():
        if project_list[file] == "source":
            pack_ver = getPackVer(file)
            if pack_ver[2] != cmtconfig:
                del project_list[file]
                html_list.remove(file)
                if cmtconfig in LbConfiguration.Platform.binary_list:
                    newbin = cmtconfig
                    if LbConfiguration.Platform.isBinaryDbg(newbin) :
                        newbin = LbConfiguration.Platform.getBinaryOpt(newbin)
                    file = file.replace(pack_ver[2],newbin)
                project_list[file] = "source"
                html_list.append(file)
    log.debug('project_list %s' % project_list)
    log.debug('CMTPATH order - html_list %s' % html_list)

    os.chdir(here)

    return project_list, html_list

#----------------------------------------------------------------------------------
# check installation
def isInstalled(file):
    installed = False
    if not overwrite_mode :
        if file.find("LbScripts") != -1 or file.find("LBSCRIPTS") != -1 :
            installedfilename = os.path.join(log_dir.split(os.pathsep)[0],file.replace(".tar.gz", ".installed"))
            if os.path.exists(installedfilename) :
                installed = True
        else :
            for ld in log_dir.split(os.pathsep) :
                installedfilename = os.path.join(ld,file.replace(".tar.gz", ".installed"))
                if os.path.exists(installedfilename) :
                    installed = True
                    break
    return installed

def setInstalled(file):
    this_log_dir = log_dir.split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir,file.replace(".tar.gz", ".installed"))

    if os.path.exists(installedfilename) :
        os.remove(installedfilename)

    f = open(installedfilename, "w")
    f.write("Done\n")
    f.close()

def delInstalled(file):
    this_log_dir = log_dir.split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir,file.replace(".tar.gz", ".installed"))
    os.remove(installedfilename)

# check installed project
def checkInstalledProjects(project_list):
    log = logging.getLogger()
    log.info('check all project in the list %s' % project_list)
    import LbConfiguration.Platform
    for file in project_list.keys() :
        if project_list[file] == "source":
            pack_ver = getPackVer(file)
            if pack_ver[2] != cmtconfig:
                if cmtconfig in LbConfiguration.Platform.binary_list:
                    newbin = cmtconfig
                    if LbConfiguration.Platform.isBinaryDbg(newbin) :
                        newbin = LbConfiguration.Platform.getBinaryOpt(newbin)
                    file = file.replace(pack_ver[2],newbin)
        if isInstalled(file) :
            log.debug("%s is installed" % file)
        else :
            log.error("%s is not installed" % file)
            sys.exit("some projects are not installed. Exiting ...")
    sys.exit()
#----------------------------------------------------------------------------------

#
#  download project tar files ================================================
#
def getProjectTar(tar_list, already_present_list=None):
    log = logging.getLogger()
    log.info('download and untar all tar files in the list')
    here = os.getcwd()

    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]
    this_system_dir = system_dir.split(os.pathsep)[0]
    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_targz_dir = targz_dir.split(os.pathsep)[0]


    for file in tar_list.keys():
        if not isInstalled(file) :
            log.debug('file= %s' % file)
            if tar_list[file] == "source":
                if file.find('LCGCMT') != -1 or file.find('LCGGrid') != -1 or file.find('LCGGanga') != -1:
                    checkWriteAccess(os.path.join(this_lcg_dir,'..'))
                    os.chdir(os.path.join(this_lcg_dir,'..'))
                elif file.find('GENSER') != -1:
                    checkWriteAccess(this_lcg_dir)
                    os.chdir(this_lcg_dir)
                else:
                    checkWriteAccess(this_contrib_dir)
                    os.chdir(this_contrib_dir)
                exist_flag = getFile(url_dist+'source/',file)
            elif tar_list[file] == "system":
                checkWriteAccess(this_system_dir)
                os.chdir(this_system_dir)
                exist_flag = getFile(url_dist+'system/',file)
            else:
                checkWriteAccess(this_lhcb_dir)
                os.chdir(this_lhcb_dir)
                exist_flag = getFile(url_dist+tar_list[file]+'/',file)
                if exist_flag and already_present_list != None:
                    already_present_list.append(tar_list[file])


            if not exist_flag :
                # untar the file
                log.debug('untar file %s' % file)
                rc = unTarFileInTmp(os.path.join(this_targz_dir,file), os.getcwd(), overwrite=overwrite_mode)
                pack_ver = getPackVer(file)
                if rc != 0 and ( pack_ver[0] != 'LCGGrid' or pack_ver[0] != 'LCGGanga') :
                    removeAll(pack_ver[3])
                    log.info('Cleaning up %s' % pack_ver[3])
                    sys.exit("getProjectTar: Exiting ...")
                if pack_ver[0] in LbLegacy.LHCb_config.ext_lhcb:
                    # if it is a ext_lhcb project
                    # create a ext_lhcb project/vers/binary directory
                    # to remember which binary tar file has been untar
                    if file.find('GENSER_v') != -1:
                    # the GENSER project as such does not exist anylonger in LCG
                        os.chdir(this_lcg_dir)
                        if not os.path.exists('GENSER'): os.mkdir('GENSER')
                        os.chdir('GENSER')
                        if not os.path.exists('GENSER_'+pack_ver[1]): os.mkdir('GENSER_'+pack_ver[1])
                    os.chdir(pack_ver[3])
                    if not os.path.exists(pack_ver[2]):
                        os.mkdir(pack_ver[2])
                        log.info('mkdir %s in %s ' % (pack_ver[2], pack_ver[3]))

                if os.getcwd() == this_lhcb_dir :
                    # if binary is requested and InstallArea does not exist : set it
                    if pack_ver[2] != '':
                        os.chdir(os.path.join(this_lhcb_dir,pack_ver[0],pack_ver[0]+'_'+pack_ver[1]))
                        if not os.path.exists(os.path.join('InstallArea',pack_ver[2])):
                            log.debug('mkdir InstallArea')
                            if not os.path.exists('InstallArea'):
                                os.mkdir ('InstallArea')
                            os.chdir('InstallArea')
                            os.mkdir(pack_ver[2])
                    if sys.platform != 'win32' :
                        try :
                            from LbLegacy.ProjectLinks import fixLinks
                            fixLinks(pack_ver[3])
                        except :
                            log.warning("Cannot use fixLinks")
                    if multiple_mysiteroot :
                        if os.path.isdir('EXTRAPACKAGES'):
                            extradir = None
                            if pack_ver[3].find('DBASE') != -1 :
                                extradir = 'DBASE'
                            elif pack_ver[3].find('PARAM') != -1 :
                                extradir = 'PARAM'
                            if extradir is not None :
                                f = os.path.join(pack_ver[0], pack_ver[1])
                                tdir = os.path.dirname(os.path.join('EXTRAPACKAGES', f))
                                if not os.path.exists(tdir) :
                                    os.makedirs(tdir)
                                shutil.copytree(os.path.join(extradir, f), os.path.join('EXTRAPACKAGES', f))
                                if fix_perm :
                                    changePermissions(os.path.join('EXTRAPACKAGES',f),recursive=True)
                                shutil.rmtree(extradir, ignore_errors=True)
                    if pack_ver[0] == "LBSCRIPTS" :
                        genlogscript = os.path.join(pack_ver[3],"InstallArea", "scripts", "generateLogin")
                        log.debug("Running: %s --without-python --no-cache -m %s %s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                        os.environ["LHCBPROJECTPATH"] = os.pathsep.join([os.path.join(os.environ["MYSITEROOT"], "lhcb"), os.path.join(os.environ["MYSITEROOT"], "lcg", "external")])
                        log.debug("LHCBPROJECTPATH: %s" % os.environ.get("LHCBPROJECTPATH", None))
                        os.system("python %s --without-python --no-cache -m %s %s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                        registerPostInstallCommand("LCGCMT", "python %s --no-cache -m %s %s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                        prodlink = os.path.join(os.path.dirname(pack_ver[3]),"prod")
                        if sys.platform != "win32" :
                            if os.path.exists(prodlink) :
                                if os.path.islink(prodlink) :
                                    os.remove(prodlink)
                                    os.symlink(pack_ver[0]+'_'+pack_ver[1], prodlink)
                                    log.debug("linking %s to %s" % (pack_ver[0]+'_'+pack_ver[1], prodlink) )
                                else :
                                    log.error("%s is not a link. Please remove this file/directory" % prodlink)
                        my_dir = os.path.dirname(this_lhcb_dir)
                        for f in os.listdir(os.path.join(pack_ver[3], "InstallArea", "scripts")) :
                            if f.startswith("LbLogin.") and not (f.endswith(".zsh") or f.endswith(".py")):
                                sourcef = os.path.join(pack_ver[3],"InstallArea", "scripts", f)
                                targetf = os.path.join(my_dir,f)
                                if os.path.islink(targetf) or os.path.isfile(targetf):
                                    os.remove(targetf)
                                if sys.platform == "win32" :
                                    shutil.copy(sourcef, targetf)
                                    log.debug("copying %s into %s" % (sourcef, targetf) )
                                else :
                                    sourcef = sourcef.replace(my_dir,"",1)
                                    while sourcef.startswith("/") or sourcef.startswith("\\") :
                                        sourcef = sourcef[1:]
                                    os.symlink(sourcef, targetf)
                                    log.debug("linking %s to %s" % (sourcef, targetf) )
            else:
                log.debug('do not untar %s ' % file)

            log.debug(' --------- next file -----------')
            setInstalled(file)
        else :
            log.debug('%s already installed' % file)
    for file in tar_list.keys():
        pack_ver = getPackVer(file)
        prj = pack_ver[0]
        if isProjectRegistered(prj) :
            callPostInstallCommand(prj)

    os.chdir(here)
    
# Autoupdate myself
def getMySelf():
    log = logging.getLogger()
    log.info("script version : %s" % script_version)
    new_install = 'latest_install_project.py'
    if os.path.exists("latest_install_project.py") :
        os.remove("latest_install_project.py")
    getFile(url_dist,'install_project.py')
    if fix_perm :
        changePermissions('latest_install_project.py', recursive=False)
    latest_line = readString(new_install,'script_version')
    latest_version = latest_line.split("'")[1]
    if script_version < latest_version :
        log.warning("You are running an old version of this script - latest version: %s" % latest_version)
        log.warning("Restarting with the latest one")
        if os.path.exists("install_project.py.old") :
            log.debug("Removing install_project.py.old")
            os.remove("install_project.py.old")
        shutil.copy("install_project.py", "install_project.py.old")
        shutil.copy("latest_install_project.py", "install_project.py")
        os.execv(sys.executable, [sys.executable] + sys.argv)    

#
# download necessary scripts ==============================================
#
def getBootScripts():
    global LbLegacy
    global LbConfiguration
    log = logging.getLogger()
    here = os.getcwd()
    if not check_only :
        getMySelf()
        cleanBootScripts()
    scripttar = "LBSCRIPTS_LBSCRIPTS_%s.tar.gz" % lbscripts_version
    if isInstalled(scripttar) :
        log.debug("LbScripts %s is already installed" % lbscripts_version)
        this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
        sys.path.insert(0, os.path.join(this_lhcb_dir, "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version, "InstallArea", "python"))
        log.debug("sys.path is %s" % os.pathsep.join(sys.path))
    else :
        log.info("LbScripts %s is not installed. Dowloading it." % lbscripts_version)
        getFile(url_dist+'LBSCRIPTS/',scripttar)
        this_bootscripts_dir = bootscripts_dir.split(os.pathsep)[0]
        this_targz_dir = targz_dir.split(os.pathsep)[0]
        if not os.path.isdir(this_bootscripts_dir) :
            os.mkdir(this_bootscripts_dir)
        checkWriteAccess(this_bootscripts_dir)
        os.chdir(this_bootscripts_dir)
        rc = unTarFileInTmp(os.path.join(this_targz_dir,scripttar), os.getcwd(), overwrite=True)
        os.chdir(here)
        if rc != 0 :
            removeAll(this_bootscripts_dir)
            sys.exit('getBootScripts: Exiting ...')
        else :
            if fix_perm :
                changePermissions(this_bootscripts_dir, recursive=True)
            sys.path.insert(0, os.path.join(this_bootscripts_dir, "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version, "InstallArea", "python"))
            log.debug("sys.path is %s" % os.pathsep.join(sys.path))
    import LbLegacy as lbl
    LbLegacy = lbl
    import LbConfiguration as lbconf
    LbConfiguration = lbconf
    atexit.register(cleanBootScripts)

def cleanBootScripts():
    log = logging.getLogger()
    this_bootscripts_dir = bootscripts_dir.split(os.pathsep)[0]
    if os.path.isdir(this_bootscripts_dir) :
        log.debug("Removing the %s directory" % this_bootscripts_dir)
        removeAll(this_bootscripts_dir)

#
#  list available versions ==============================================================================
#
def listVersions(pname):
    log = logging.getLogger()
    log.debug('listVersions for %s ' % pname)

    PROJECT = pname.upper()
    webpage = urlopen(url_dist+'/'+PROJECT)
    weblines = webpage.readlines()
    for webline in weblines:
        if webline.find('href="'+PROJECT) != -1:
            href = webline.index('href=')
            quote1 = webline[href:].index('"')
            quote2 = webline[href+quote1+1:].index('"')
            log.info(webline[href+quote1+1:href+quote1+1+quote2])
    atexit.register(urlcleanup)

#
#  read a string from a file ==============================================
#
def readString(filename,string):
    fd = open(filename)
    fdlines = fd.readlines()
    for fdline in fdlines:
        if fdline.find(string) != -1:
            return fdline


#
#  remove a project,version from the install area ==========================
#
def removeProject(project,pvers):
    log = logging.getLogger()
    log.info('%s %s '% (project, pvers))

    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_html_dir = html_dir.split(os.pathsep)[0]
    this_targz_dir = targz_dir.split(os.pathsep)[0]
    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_log_dir = log_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]

    import LbLegacy.LHCb_config

    PROJECT = project.upper()
    if LbLegacy.LHCb_config.lhcb_projects.has_key(PROJECT):
        head = this_lhcb_dir
        VERSION = PROJECT+'_'+pvers

        flist = os.listdir(this_html_dir)
        for file in flist:
            if file.find(VERSION+'.') != -1 or file.find(VERSION+'_') != -1 :
                os.remove(os.path.join(this_html_dir,file))
                log.info('remove %s' % os.path.join(this_html_dir, file))

        flist = os.listdir(this_targz_dir)
        for file in flist:
            if file.find(VERSION+'.') != -1 or file.find(VERSION+'_') != -1:
                os.remove(os.path.join(this_targz_dir,file))
                log.info('remove %s' % os.path.join(this_targz_dir, file))

        flist = os.listdir(this_log_dir)
        for file in flist:
            if file.find(VERSION+'.') != -1 or file.find(VERSION+'_') != -1:
                os.remove(os.path.join(this_log_dir,file))
                log.info('remove %s' % os.path.join(this_log_dir, file))

        if os.path.isdir(os.path.join(head, PROJECT, VERSION)):
            shutil.rmtree(os.path.join(head, PROJECT, VERSION))
            log.info('remove %s'% os.path.join(head, PROJECT, VERSION))
        if os.path.isdir(os.path.join(head, project+'Env', pvers)):
            shutil.rmtree(os.path.join(head, project+'Env', pvers))
            log.info('remove %s' % os.path.join(head, project+'Env', pvers))
    else:
        proj_vers = project+'_'+pvers
        flist = os.listdir(this_html_dir)
        for file in flist:
            if file.find(proj_vers) != -1 :
                os.remove(os.path.join(this_html_dir,file))
                log.info('remove %s' % os.path.join(this_html_dir, file))

        flist = os.listdir(this_targz_dir)
        for file in flist:
            if file.find(proj_vers+'.') != -1 or file.find(proj_vers+'_') != -1:
                os.remove(os.path.join(this_targz_dir,file))
                log.info('remove %s \n' % os.path.join(this_targz_dir, file))

        flist = os.listdir(this_log_dir)
        for file in flist:
            if file.find(proj_vers+'.') != -1 or file.find(proj_vers+'_') != -1:
                os.remove(os.path.join(this_log_dir,file))
                log.info('remove %s' % os.path.join(this_log_dir, file))

        head = this_contrib_dir
        if os.path.isdir(os.path.join(head,project,pvers)):
            shutil.rmtree(os.path.join(head,project,pvers))
            log.info('remove %s' % os.path.join(head, project, pvers))
        else:
            head = this_lcg_dir
            vers = pvers
            if project == 'LCGCMT' or project == 'GENSER': vers = proj_vers
            if os.path.isdir(os.path.join(head,project,vers)):
                shutil.rmtree(os.path.join(head,project,vers))
                log.info('remove %s' % os.path.join(head, project, vers))

    if multiple_mysiteroot and os.path.isdir(os.path.join(this_lhcb_dir, 'EXTRAPACKAGES')):
        head = os.path.join(this_lhcb_dir,'EXTRAPACKAGES')
        if project.find('Field') != -1:
            head = os.path.join(this_lhcb_dir,'EXTRAPACKAGES')
        if project.find('DDDB') != -1:
            head = os.path.join(this_lhcb_dir,'EXTRAPACKAGES','Det')
        if project.find('Dec') != -1:
            head = os.path.join(this_lhcb_dir,'EXTRAPACKAGES','Gen')
        if os.path.isdir(os.path.join(head,project,pvers)):
            shutil.rmtree(os.path.join(head,project,pvers))
            log.info('remove %s' % os.path.join(head, project, pvers))

    head = os.path.join(this_lhcb_dir,'PARAM')
    if project.find('Field') != -1 or project.find("AppConfig") != -1:
        head = os.path.join(this_lhcb_dir,'DBASE')
    if project.find('DDDB') != -1:
        head = os.path.join(this_lhcb_dir,'DBASE','Det')
    if project.find('Dec') != -1:
        head = os.path.join(this_lhcb_dir,'DBASE','Gen')
    if project.find('L0TCK') != -1 :
        head = os.path.join(this_lhcb_dir, 'DBASE', "TCK")
    if os.path.isdir(os.path.join(head,project,pvers)):
        shutil.rmtree(os.path.join(head,project,pvers))
        log.info('remove %s' % os.path.join(head, project, pvers))

# -------------------------------------------------------------------------------------------
# Generate SetupScript
def genSetupScript(pname, pversion, cmtconfig, scriptfile):
    log = logging.getLogger()
    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    targetshells = ["sh", "csh", "bat"]
    usedshell = "csh"
    for t in targetshells :
        if scriptfile.endswith(".%s" % t) :
            usedshell = t
            break
    os.environ["CMTCONFIG"] = cmtconfig
    # setup the scripts python path
    lbscriptspydir = os.path.join(this_lhcb_dir,"LBSCRIPTS", "LBSCRIPTS_"+lbscripts_version, "InstallArea", "python")
    if os.path.exists(lbscriptspydir) :
        sys.path.append(lbscriptspydir)
    else :
        log.error("%s doesn't exist" % lbscriptspydir )
    # setup the base LHCb environment
    lbloginscript = os.path.join(lbscriptspydir, "LbConfiguration", "LbLogin.py")
    log.debug("Using LbLogin from %s" % lbloginscript)
    if os.path.exists(lbloginscript) :
        from LbConfiguration.LbLogin import getLbLoginEnv
        llsargs = []
        llsargs.append("--shell=%s" % usedshell)
        llsargs.append("--mysiteroot=%s" % os.environ["MYSITEROOT"])
        llsargs.append("--scripts-version=%s" % lbscripts_version)
        llsargs.append("--cmtconfig=%s" % cmtconfig)
        log.debug("Running LbLogin %s" % " ".join(llsargs))
        env = getLbLoginEnv(llsargs)
        for var in env.keys() :
            os.environ[var] = env[var]
    else :
        log.error("%s doesn't exist" % lbloginscript)

    # run SetupProject and create the setup script
    setprojscript = os.path.join(lbscriptspydir, "LbConfiguration", "SetupProject.py")
    log.debug("Using SetuProject from %s" % setprojscript)
    if os.path.exists(setprojscript) :
        from LbConfiguration.SetupProject import SetupProject
        setuprojargs = []
        setuprojargs.append("--silent")
        setuprojargs.append("--shell=%s" % usedshell)
        setuprojargs.append("--output=%s" % scriptfile)
        setuprojargs.append("--no-user-area")
        setuprojargs.append(pname)
        setuprojargs.append(pversion)
        SetupProject().main(setuprojargs)
        cmd = "python %s --silent --shell=%s --output=%s %s %s" % (setprojscript, usedshell,scriptfile,
                                                                    pname, pversion)
        log.debug("Running %s" % cmd)
#        os.system(cmd)
    else :
        log.error("%s doesn't exist" % setprojscript)
    # Setup script postprocessing
    if os.path.exists(scriptfile) :
        postProcessSetupScript(scriptfile, usedshell)
    else :
        log.error("%s doesn't exist" % scriptfile)
def postProcessSetupScript(scriptfile, shell):
    log = logging.getLogger()
    log.debug("Postprocessing of %s" % scriptfile)
    cleanStripPath(scriptfile, shell)

def cleanStripPath(scriptfile, shell):
    # remove the call to StripPath
    inf = open(scriptfile, "r")
    data = inf.readlines()
    inf.close()
    outf = open(scriptfile, "w")
    found = 0
    for l in data :
        words = l.split()
        if len(words)>0 and words[0] == "if" and l.find("StripPath") != -1 :
            found = 1
        else :
            if found :
                if found < 4 :
                    found += 1
                else :
                    found = 0
        if found :
            if shell == "bat" :
                outf.write("REM " + l )
            else :
                outf.write("# " + l)
        else :
            outf.write(l)
    outf.close()
    # clean up the variables
    varlist = ["PATH", "LD_LIBRARY_PATH", "PYTHONPATH",
               "JOBOPTSEARCHPATH", "HPATH", "MANPATH"]
    for v in varlist :
        varvalue = StripPath(getVariable(scriptfile, shell, v))
        appendVariable(scriptfile, shell, v, varvalue)


def getVariable(scriptfile, shell, varname):
    log = logging.getLogger()
    log.debug("Extracting %s from %s" % (varname, scriptfile))
    inf = open(scriptfile, "r")
    data = inf.readlines()
    inf.close()
    value = ""
    for l in data :
        words = l.split()
        if len(words) > 1 :
            if shell == "sh" :
                if words[0] == "export" and words[1].startswith("%s=" % varname) :
                    value = words[1].split("=")[1]
                    continue
            elif shell == "csh" and len(words)>2 :
                if words[0] == "setenv" and words[1] == varname :
                    value = words[2]
                    continue
            elif shell == "bat" :
                if words[0] == "set" and words[1].startswith("%s=" % varname) :
                    value = words[1].split("=")[1]
                    continue
    return value

def appendVariable(scriptfile, shell, varname, varcont):
    log = logging.getLogger()
    if varcont :
        log.debug("Appending %s to %s" % (varname, scriptfile))
        outf = open(scriptfile, "a")
        if shell == "sh" :
            outf.write("export %s=%s\n" % (varname, varcont) )
        elif shell == "csh" :
            outf.write("setenv %s %s\n" % (varname, varcont) )
        elif shell == "bat" :
            outf.write("set %s=%s\n" % (varname, varcont) )
        outf.close()
    else :
        log.warning("Content of %s is empty" % varname)


def StripPath(path):
    collected = []
    for p in path.split(os.pathsep):
        rp = os.path.realpath(p)
        if os.path.exists(rp) and os.path.isdir(rp):
            if len(os.listdir(rp)) != 0:
                collected.append(p)
    return os.pathsep.join(collected)

# -------------------------------------------------------------------------------------------
#
#  create base directories #################################################
#

def createBaseDirs(pname, pversion):
    global multiple_mysiteroot
    global cmtconfig
    global log_dir, contrib_dir, lcg_dir, lhcb_dir, html_dir
    global bootscripts_dir, targz_dir, system_dir, tmp_dir


    # removes the trailing "/" at the end of the path
    if os.environ.has_key("MYSITEROOT") :
        path_list = []
        for p in  os.environ["MYSITEROOT"].split(os.pathsep) :
            while p.endswith(os.sep) :
                p = p[:-1]
            path_list.append(p)
        os.environ["MYSITEROOT"] = os.pathsep.join(path_list)

    if os.environ.has_key('MYSITEROOT') :
        if os.environ['MYSITEROOT'].find(os.pathsep) != -1 :
            multiple_mysiteroot = True

    if os.environ.has_key('MYSITEROOT') == 1:
        mypath = os.path.realpath(os.environ['MYSITEROOT'])
        thispwd = os.path.realpath(os.getcwd())
        if sys.platform == 'win32' :
            if mypath.split(os.pathsep)[0].upper() != thispwd.upper() :
                sys.exit('please set MYSITEROOT == $PWD:$MYSITEROOT before running the python script \n')
        else:
            if mypath.split(os.pathsep)[0] != thispwd :
                sys.exit('please set MYSITEROOT == $PWD:$MYSITEROOT before running the python script \n')

    else:
        #print 'please set $MYSITEROOT before running the python script'
        sys.exit('please set $MYSITEROOT before running the python script \n')

    if os.environ.has_key('CMTCONFIG') == 0:
        #print ' please set $CMTCONFIG before running the python script'
        sys.exit(' please set $CMTCONFIG before running the python script \n')

    cmtconfig = os.environ['CMTCONFIG']

    log_dir = []
    contrib_dir = []
    lcg_dir = []
    lhcb_dir = []
    html_dir = []
    bootscripts_dir = []
    targz_dir = []
    system_dir = []
    tmp_dir = []
    for p in mypath.split(os.pathsep) :
        log_dir.append(os.path.join(p,'log'))
        contrib_dir.append(os.path.join(p,'contrib'))
        lcg_dir.append(os.path.join(p, 'lcg', 'external'))
        lhcb_dir.append(os.path.join(p,'lhcb'))
        html_dir.append(os.path.join(p,'html'))
        bootscripts_dir.append(os.path.join(p,'bootscripts'))
        targz_dir.append(os.path.join(p,'targz'))
        system_dir.append(os.path.join(p,cmtconfig))
        if sys.platform != "win32" :
            tmp_dir.append(os.path.join(p,"tmp", os.environ["USER"]))
        else :
            tmp_dir.append(os.path.join(p,"tmp"))

    log_dir= os.pathsep.join(log_dir)
    contrib_dir = os.pathsep.join(contrib_dir)
    lcg_dir = os.pathsep.join(lcg_dir)
    lhcb_dir = os.pathsep.join(lhcb_dir)
    html_dir = os.pathsep.join(html_dir)
    bootscripts_dir = os.pathsep.join(bootscripts_dir)
    targz_dir = os.pathsep.join(targz_dir)
    system_dir = os.pathsep.join(system_dir)
    tmp_dir = os.pathsep.join(tmp_dir)


    logname = os.path.join(log_dir.split(os.pathsep)[0], pname+'_'+pversion+'.log')

    if not check_only :
        logname = os.path.join(log_dir.split(os.pathsep)[0], pname+'_'+pversion+'.log')
        createDir(mypath.split(os.pathsep)[0], logname)
    else :
        logname = None
    return logname

# install main logging script
def installLoginScripts():
    log = logging.getLogger()
    found = False
    this_maindir = os.path.dirname(log_dir.split(os.pathsep)[0])
    if not "LbLogin.csh" in os.listdir(this_maindir) :
        for l in log_dir.split(os.pathsep) :
            maindir = os.path.dirname(l)
            if "LbLogin.csh" in os.listdir(maindir) :
                found = True
                break
        if found :
            for f in os.listdir(maindir) :
                if f.startswith("LbLogin.") :
                    sourcef = os.path.join(maindir, f)
                    targetf = os.path.join(this_maindir, f)
                    if sys.platform != "win32" :
                        os.symlink(sourcef, targetf)
                        log.debug("linking %s to %s" % (sourcef, targetf))
                    else :
                        shutil.copy(sourcef, targetf)
                        log.debug("copying %s to %s" % (sourcef, targetf))
        else :
            log.error("cannot find LbLogin scripts")
#
#  install one project #################################################
#
def runInstall(pname,pversion,binary=''):
    log = logging.getLogger()

# print action list
    start_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    log.info('++++++++++++++++++++++%s  python %s starts install_project.py - version no %s' % (start_time, txt_python_version, script_version) )
    log.info("%s %s %s" % (pname, pversion, binary))
    log.info('cmt version = %s, make_flag= %s, full_flag= %s, list_flag= %s, remove_flag= %s ' % (cmtversion, make_flag, full_flag, list_flag, remove_flag))




    log.info(' +++++++ start install_project.py version= %s ' % script_version)
    log.info('cmt version =%s, make_flag= %s, debug_flag= %s, list_flag= %s, remove_flag= %s' % (cmtversion, make_flag, debug_flag, list_flag, remove_flag))

# cleanup the cache at exit
    urlcleanup()
    atexit.register(urlcleanup)

# if list_flag is set: give the list of available versions for this project
    if list_flag :
        listVersions(pname)
        sys.exit()

# start the project installation
    getBootScripts()


# if remove flag is set then correspondind tar files and directories will
# be removed
    if remove_flag :
        removeProject(pname,pversion)
        sys.exit()

# check binary name
    checkBinaryName(binary)

    getCMT(cmtversion)

    setLHCbEnv()

    if not check_only :
        if pname != 'LbScripts' :
            script_project_list = getProjectList('LbScripts', lbscripts_version)[0]
            getProjectTar(script_project_list)
    
    project_list,html_list = getProjectList(pname,pversion)
    if pname != 'LHCbGrid' and sys.platform != "win32" and cmtconfig.find("slc3")!=-1 and cmtconfig.find("sl3") != -1 :
        grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version)
        project_list.update(grid_project_list)
        html_list += grid_html_list

    if pname != 'Compat' and sys.platform != "win32" :
        compat_project_list, compat_html_list = getProjectList('Compat', compat_version)
        project_list.update(compat_project_list)
        html_list += compat_html_list

    if check_only :
        checkInstalledProjects(project_list)

    getProjectTar(project_list)

    if binary != ' ':
        from LbConfiguration.Platform import isBinaryDbg, getBinaryOpt, getBinaryDbg
        binary_dbg = getBinaryDbg(binary)
        binary_opt = binary
        if isBinaryDbg(binary) :
            binary_dbg = binary
            binary_opt = getBinaryOpt(binary)
        project_list, html_list = getProjectList(pname, pversion, binary)
        if pname != 'LHCbGrid' and sys.platform != "win32" and cmtconfig.find("slc3")!=-1 and cmtconfig.find("sl3") != -1 :
            grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version, binary_opt)
            project_list.update(grid_project_list)
            html_list += grid_html_list
        if pname != 'Compat' and sys.platform != "win32" :
            compat_project_list, compat_html_list = getProjectList('Compat', compat_version, binary_opt)
            project_list.update(compat_project_list)
            html_list += compat_html_list
        already_exist = []
        getProjectTar(project_list, already_exist)


# if full_flag is set : download binary_dbg tar files and configure the projects
    if full_flag :
        log.info('download debug version and reconfigure it')
        from LbConfiguration.Platform import isBinaryDbg, getBinaryOpt, getBinaryDbg
        binary_dbg = getBinaryDbg(binary)
        binary_opt = binary
        if isBinaryDbg(binary) :
            binary_dbg = binary
            binary_opt = getBinaryOpt(binary)
        os.environ['CMTCONFIG'] = binary_dbg
        project_list, html_list = getProjectList(pname, pversion, binary_dbg)
        getProjectTar(project_list)

    if setup_script :
        os.chdir(os.environ["MYSITEROOT"].split(os.pathsep)[0])
        genSetupScript(pname, pversion, cmtconfig, setup_script)

    end_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    log.info( '+++++++++++++++++++++++ %s end install_project.py -version no %s' % (end_time, script_version))


#
# set lhcb environment ====================================================
#
def setLHCbEnv():
    log = logging.getLogger()
    log.debug('set necessary environment variables')

    strcmd = getSourceCmd() + ' '+os.path.join(os.environ['CMTROOT'],'mgr','setup.' + getScriptExt() )
    log.debug('str= %s ' % strcmd)
    if sys.platform == 'win32':
        os.system(strcmd)
    else:
        output = commands.getstatusoutput(strcmd)[1]
        log.debug(output)

    if os.environ.has_key('LD_LIBRARY_PATH') == 1:
        ldlibrarypath = os.environ['LD_LIBRARY_PATH']
    else:
        ldlibrarypath = ''


    mysiteroot = os.environ['MYSITEROOT'].split(os.pathsep)[0]
    cmtsite = 'LOCAL'

    os.environ['CMTSITE']             = cmtsite
    os.environ['SITEROOT']            = mysiteroot
    os.environ['Gaudi_release_area']  = lhcb_dir
    os.environ['LHCb_release_area']   = lhcb_dir
    os.environ['Lbcom_release_area']  = lhcb_dir
    os.environ['LHCbGrid_release_area']  = lhcb_dir
    os.environ['Geant4_release_area'] = lhcb_dir
    os.environ['Rec_release_area']    = lhcb_dir
    os.environ['Phys_release_area']   = lhcb_dir
    os.environ['LCG_release_area']    = lcg_dir
    os.environ['OSC_release_area']    = contrib_dir
    os.environ['LD_LIBRARY_PATH']     = ldlibrarypath

    log.debug('   LD_LIBRARY_PATH=%s' % os.getenv('LD_LIBRARY_PATH'))
    log.debug('   CMTSITE=%s' % os.getenv('CMTSITE'))
    log.debug('   SITEROOT= %s' % os.getenv('SITEROOT'))
    log.debug('   Gaudi_release_area= %s' % os.getenv('Gaudi_release_area'))
    log.debug('   LHCb_release_area= %s ' % os.getenv('LHCb_release_area'))
    log.debug('   Geant4, Rec, Phys, Lbcom_release_area are set to LHCb_release_area')
    log.debug('   LCG_release_area= %s ' % os.getenv('LCG_release_area'))
    log.debug('   PATH= %s ' % os.getenv('PATH'))

#
#  unpack tar.gz file =====================================================
#

def tarFileList(filename):
    log = logging.getLogger()
    if not useTarFileModule() :
        lststr =  'tar --list --ungzip --file %s' % filename
        for l in os.popen(lststr) :
            yield l[:-1]
    else :
        import tarfile
        if tarfile.is_tarfile(filename):
            tar = tarfile.open(filename,'r:gz')
            for l in tar.getnames() :
                yield l
        else:
            log.warning('tar_file: %s is not a tar file' % filename)


def cleanTarFileTarget(filename, targetlocation):
    for f in tarFileList(filename) :
        fp = os.path.join(targetlocation, f)
        if os.path.isfile(fp) :
            os.remove(fp)


def addSoft(srcdir, dstdir, overwrite=False):
    log = logging.getLogger()
    if overwrite :
        log.debug("Overwriting %s with %s" % (dstdir, srcdir))
    else :
        log.debug("Add %s to %s" % (dstdir, srcdir))
    if not os.path.exists(dstdir):
        os.makedirs(dstdir)
        log.info("%s has been created" % dstdir)
    for root, dirs, files in os.walk(srcdir, topdown=True) :
        dirstoremove = []
        for d in dirs :
            src = os.path.join(root, d)
            dst = src.replace(srcdir+os.sep,"")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
                shutil.move(src, dst)
                dirstoremove.append(d)
        for f in files :
            src = os.path.join(root, f)
            dst = src.replace(srcdir+os.sep,"")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
                shutil.move(src, dst)
            elif overwrite :
                if os.path.exists(dst) :
                    os.remove(dst)
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
                shutil.move(src, dst)
        for d in dirstoremove :
            dirs.remove(d)


def unTarFileInTmp(filename, targetlocation, overwrite=False, offset=None):
    cleanTmpDirectory()
    here = os.getcwd()
    os.chdir(getTmpDirectory())
    rc = untarFile(filename)
    thisdir = os.getcwd()
    if fix_perm :
        changePermissions(thisdir, recursive=True)
    walkdir = thisdir
    if offset :
        walkdir = os.path.join(walkdir, offset)
    addSoft(walkdir, targetlocation, overwrite)
    os.chdir(here)
    cleanTmpDirectory()
    return rc

def useTarFileModule():
    usetfm = False
    # tarfile is available in Python version >= 234
    vmin = (2, 3, 4)
    if sys.platform == "win32" :
        if python_version >= vmin and python_version != (2, 4, 3)  and python_version != (2, 4, 4) :
            usetfm = True
    return usetfm

def untarFile(file):
    log = logging.getLogger()

    retcode = 0

    log.info('%s on %s ' % (file, os.getcwd()))

    this_targz_dir = targz_dir.split(os.pathsep)[0]

    filename = os.path.join(this_targz_dir,file)
    md5filename = getMD5FileName(filename)
    htmlfilename = os.path.join(html_dir,getHTMLFileName(file))
    if not os.path.isfile(filename):
        log.warning('%s does not exist' % filename)
        retcode = 1
        return retcode

    if useTarFileModule() :

        import tarfile
        if tarfile.is_tarfile(filename):
            tar = tarfile.open(filename,'r:gz')
        else:
            log.warning('%s is not a tar file' % filename)
            log.warning('check the existence of the file on the repository')
            log.warning('the local file will be removed')
            os.remove(filename)
            if os.path.exists(md5filename):
                os.remove(md5filename)
                log.info('%s removed' % md5filename)
            if os.path.exists(htmlfilename):
                os.remove(htmlfilename)
                log.info('%s removed' % htmlfilename)
            retcode = 1
            return retcode
        i=0
        j=0
        try:
            for tarinfo in tar:
                i=i+1
                try:
                    if not os.path.exists(tarinfo.name):
                        j=j+1
                        if sys.platform == 'win32'and tarinfo.issym():
                            log.info( '%s %s %s' % (j, tarinfo.name, tarinfo.issym()))
                        else:
                            tar.extract(tarinfo)
                    else:
                        if file.find('script') != -1:
                            j=j+1
                            tar.extract(tarinfo)
                except tarfile.ExtractError:
                    os.remove(filename)
                    log.warning('extract error %s %s %s ' % (filename, tarinfo.name))
                    if os.path.exists(md5filename):
                        os.remove(md5filename)
                        log.info('%s removed' % md5filename)
                    if os.path.exists(htmlfilename):
                        os.remove(htmlfilename)
                        log.info('%s removed' % htmlfilename)
                    retcode = 1
                    return retcode
        except:
            os.remove(filename)
            log.warning('tarfile error %s' % filename)
            if os.path.exists(md5filename):
                os.remove(md5filename)
                log.info('%s removed' % md5filename)
            if os.path.exists(htmlfilename):
                os.remove(htmlfilename)
                log.info('%s removed'% htmlfilename)
            retcode = 1
            return retcode

        tar.close()
        log.info('%s - %s entries, untar %s entries ' % (file, i, j))

    else:
        if sys.platform == 'win32':
            os.remove(filename)
            log.warning('your python version %s is not able to untar a file - try to get a version >= 2.3.4 '% python_version )
            if os.path.exists(md5filename):
                os.remove(md5filename)
                log.info('%s removed' % md5filename)
            if os.path.exists(htmlfilename):
                os.remove(htmlfilename)
                log.info('%s removed' % htmlfilename)
            retcode = 1
            return retcode
        else:
            strcmd = 'tar --extract --ungzip --touch --backup=simple --file %s' % filename
            if fix_perm :
                strcmd = 'tar --extract --ungzip --touch --no-same-permissions --backup=simple --file %s' % filename
            try:
                for l in tarFileList(filename) :
                    if os.path.isfile(l) :
                        os.remove(l)
                status, tar_output = commands.getstatusoutput(strcmd)
            except:
                os.remove(filename)
                log.warning( 'exception in: %s' % strcmd)
                log.warning( '%s removed' % filename)
                log.warning( 'tar command output:' )
                for l in tar_output.split("\n") :
                    log.warning(l)
                if os.path.exists(md5filename):
                    os.remove(md5filename)
                    log.info('%s removed'% md5filename)
                if os.path.exists(htmlfilename):
                    os.remove(htmlfilename)
                    log.info('%s removed' % htmlfilename)
                retcode = 1
                return retcode
            else:
                if status != 0:
                    log.warning( 'error in: %s' % strcmd)
                    log.warning( 'return code %s' % status )
                    log.warning( 'tar command output:' )
                    for l in tar_output.split("\n") :
                        log.warning(l)
                    if os.path.exists(filename):
                        os.remove(filename)
                        log.info('%s removed'%(filename))
                    if os.path.exists(md5filename):
                        os.remove(md5filename)
                        log.info('%s removed'%(md5filename))
                    if os.path.exists(htmlfilename):
                        os.remove(htmlfilename)
                        log.info('%s removed'%(htmlfilename))
                    retcode = 1
                    return retcode


    # replace the tar.gz file with an empty one to save space

    os.remove(filename)

    return retcode


def checkBinaryName(binary):
    global make_flag
    import LbConfiguration.Platform
    if binary != ' ':
        for b in LbConfiguration.Platform.binary_list:
            if b == binary:
                os.environ['CMTCONFIG'] = binary
                os.environ['CMTDEB'] = binary
                if LbConfiguration.Platform.isBinaryOpt(binary) :
                    os.environ['CMTDEB'] = LbConfiguration.Platform.getBinaryDbg(binary)
                # if a win32 binary is installed from a non win32 platform then do not cmt config
                if sys.platform != 'win32' and binary.find('win32') != 'win32':
                    make_flag = ' '
                break
        else:
            print ' this binary %s is not part of LHCb distribution '% binary
            print ' choose another one from the list %s '% LbConfiguration.Platform.binary_list
            print ' or do not require the binary  '
            sys.exit(' %s is not part of LHCb distribution '%(binary)+'\n' )
    else:
        if not os.getenv('CMTCONFIG') in LbConfiguration.Platform.binary_list:
            print 'BE CAREFUL - your CMTCONFIG %s is not part of the lhcb_binary %s'%(os.getenv('CMTCONFIG'),LbConfiguration.Platform.binary_list)
            print 'do you want to continue? [yes|no]'
            next = sys.stdin.readline()
            if next.lower()[0] != 'y':
                sys.exit()

#---------------------------------------------------------------------
def main():
    
    global debug_flag, full_flag, list_flag, remove_flag
    global cmtversion, md5_check, grid_version, nb_retries
    global setup_script, check_only, overwrite_mode
    global _retry_time, fix_perm
# get arguments
    pname =' '
    pversion = ' '
    binary = ' '

    arguments = sys.argv[1:]
    if len(sys.argv) == 1:
        help()
        sys.exit()
    try:
        keys, values = getopt.getopt(arguments,'hdflrbp:v:c:ng:s:',
            ['help','debug','full','list','remove','binary=',
             'project=','version=','cmtversion=','nocheck',
             'retry=','grid=','setup-script=','check', 'overwrite',
             'compatversion=', 'retrytime=', 'nofixperm'])

    except getopt.GetoptError:
        help()
        sys.exit()

    for key,value in keys:
        if key in ('-d', '--debug'):
            debug_flag = True
        if key in ('-f', '--full'):
            full_flag = True
        if key in ('-h', '--help'):
            help()
        if key in ('-l', '--list'):
            list_flag = True
        if key in ('-r', '--remove'):
            remove_flag = True
        if key in ( '-c', '--cmtversion'):
            cmtversion = value
        if key == '--compatversion':
            compat_version = value
        if key in ( '-v', '--version'):
            pversion = value
        if key in ('-p', '--project'):
            pname = value
            if pname.find("/") != -1 :
                plist = pname.split("/")
                pname = "/".join(plist[1:])
        if key == '-b':
            binary = os.environ['CMTCONFIG']
        if key == '--binary':
            binary = value
            os.environ["CMTCONFIG"] = binary
        if key in ('-n','--nocheck'):
            md5_check = False
        if key in ('-g','--grid'):
            grid_version = value
        if key == '--retry':
            nb_retries = int(value)
        if key == '--retrytime':
            _retry_time = float(value)
        if key in ('-s', '--setup-script') :
            setup_script = value
        if key == '--check':
            check_only = True
        if key == '--nofixperm':
            fix_perm = False
        if key == '--overwrite':
            overwrite_mode = True
            

    thelog = logging.getLogger()
    thelog.setLevel(logging.DEBUG)
    console = logging.StreamHandler()
    if python_version < (2, 5, 1) :
        console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    else :
        if debug_flag == 1 :
            console.setFormatter(logging.Formatter("%(levelname)-8s: %(funcName)-25s - %(message)s"))
        else :
            console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
    if debug_flag == 1 :
        console.setLevel(logging.DEBUG)
    else :
        console.setLevel(logging.INFO)
    thelog.addHandler(console)

    if not check_only and fix_perm:
        changePermissions('install_project.py', recursive=False)

# check pversion
    if pversion == ' ' and pname != 'LHCbGrid':
        list_flag = True

    initRandSeed()
    lognm = createBaseDirs(pname, pversion)

    if lognm :
        filehandler = logging.FileHandler(lognm, "w")
        if python_version < (2, 5, 1) :
            filehandler.setFormatter(logging.Formatter("%(asctime)s %(levelname)-8s: %(message)s"))
        else :
            filehandler.setFormatter(logging.Formatter("%(asctime)s %(levelname)-8s: %(funcName)-25s - %(message)s"))
        filehandler.setLevel(logging.DEBUG)
        thelog.addHandler(filehandler)

    runInstall(pname, pversion, binary)

if __name__ == "__main__":
    main()
