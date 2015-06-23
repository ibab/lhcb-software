#!/usr/bin/env python
"""
  Script to install a project where you are
  Please have a look at the documentation in the release.notes
"""
#------------------------------------------------------------------------------
#============================================================================
# bootstrapping the location of the file
try:
    _this_file = __file__
except NameError :
    _this_file = None

import sys, os, getopt, time, shutil
import atexit
import stat
import commands
import logging
import random
import socket
from urllib import urlretrieve, urlopen, urlcleanup
from tempfile import mkdtemp

script_version = '150622'
python_version = sys.version_info[:3]
txt_python_version = ".".join([str(k) for k in python_version])
lbscripts_version = "v8r4p1"
compat_version = None
line_size = 120
#-----------------------------------------------------------------------------------

# url from which to get files
url_dist = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'

# list of subdirectories created in runInstall
lcg_tar      = [ "LCGCMT", "LCGGrid", "LCGGanga" ]

subdir_dict = {}
subdir_dict["lcg"]             = None
subdir_dict["lhcb"]            = None
subdir_dict["contrib"]         = None
subdir_dict["html"]            = None
subdir_dict["targz"]           = None
subdir_dict["tmp"]             = None
subdir_dict["etc"]             = None
subdir_dict["conf"]            = None
subdir_dict["log"]             = None
subdir_dict["bootscripts"]     = None

# dynamic modules

LbLegacy = None
LbConfiguration = None
LbRelease = None


# base directories
cur_tmp_dir = None # current tmp directory. Unique. Not chained
boot_script_loc = None

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
make_flag = None
md5_check = True
nb_retries = 3
check_only = False
overwrite_mode = False
fix_perm = True
show_compatible_configs = False
install_binary = False
latest_data_link = False
dev_install = False
extra_binary = None

# Extra flag to enable/diable the LbScript version check
check_self = True
get_boot_scripts_flag = True
get_compat_flag = True

#----------------------------------------------------------------------------------

def usage() :
    print ' install_project.py - version %s:  install a project in the current directory ' % script_version
    print """
    Usage:
      python install_project.py [-b| --binary=<bin> ] [-d or --debug] [-m <global|select>] <project> <version>

          $CMTCONFIG  is the binary directory name
      creates log/         to receive log files
              lcg/         to receive lcg software
              lhcb/        to receive lhcb software
              contrib/     to receive CMT and OpenScientist
              targz/       to receive tar files from the web
      get the list of projects to download
      download project sources
      if binaries are required: download project binaries
      -p                   : name of the project to install (obsolete option)
      -v                   : version of the project to install (obsolete option)
      --version            : the version of this very script
      -d or --debug        : to print more info
      -l or --list         : to list the <project>_<version>_*.tar.gz files available on the web
      -r or --remove       : remove the <project>/<version>
      -c or --cmtversion   : download this CMT version
      -h or --help         : to print this help
      -b                   : to import $CMTCONFIG binaries
      --binary=<bin>       : to import another binary (i.e. $CMTCONFIG_dbg on Linux)
                             this triggers a 'cmt broadcast cmt config' of all projects but LCGCMT
      -f                   : to import source, $CMTCONFIG binaries, $CMTCONFIG_dbg binaries
      -n or -nocheck       : no md5 check of the tar balls
      --retry=<nb>         : nb of retries for the download (default=1)
      -C                   : show compatible CMTCONFIGs for that platform
      --dev-install        : use the devel location for the self-update
      -u or --url          : use of different distribution location
      -B or --boot-scripts : prepend boot scripts location (LbScripts base dir). For debug purposes.
      -t or --tmp-dir      : give the base tmp directory instead of the std $MYSITEROOT/tmp/$USER. mkdtemp
                             is used to make the directory unique.
      --no-self-check      : Disable check for download of latest version
      --no-boot-scripts    : Disable LbScripts installation
      --no-compat          : Disable COmpat installation
      
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

def systemCall(command, workdir=os.getcwd(), env=None):
    """
    simple wrapper for shell system execution with or without the
    subprocess module.
    @param command: command to be executed
    @type command: string
    @param workdir: directory where the command has to be executed
    @type workdir: string
    """
    try :
        from subprocess import Popen
        p = Popen(command, shell=True, cwd=workdir, env=env)
        rc = os.waitpid(p.pid, 0)[1]
    except ImportError:
        # fallback if subprocess doesn't exist
        if workdir :
            this_env = dict(os.environ)
            here = os.getcwd()
            os.chdir(workdir)
        os.environ.update(env)
        rc = os.system(command)
        if workdir:
            os.chdir(here)
            os.environ = this_env
    return rc


def initRandSeed():
    random.seed("%d-%s" % (os.getpid(), socket.getfqdn()))

_block_count = 0
_block_size = 0
_file_size = 0

def reportHook(bcount, bsize, fsize):
    global _block_count, _block_size, _file_size
    _block_count = bcount
    _block_size = bsize
    _file_size = fsize

_retry_time = 120.0

def retrieve(url, dest):
    global _block_count, _block_size, _file_size
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
            sleep_time = _retry_time + random.uniform(-_retry_time / 2, _retry_time / 2)
            log.warning("Sleeping for %f second before retrying" % sleep_time)
            time.sleep(sleep_time)
    debugmsglist = []
    debugmsglist.append("Block count %d" % _block_count)
    debugmsglist.append("Block size %d" % _block_size)
    debugmsglist.append("File size %d" % _file_size)
    log.debug("\t".join(debugmsglist))
    _block_size = 0
    _block_count = 0
    _file_size = 0

    return fname, finfo

def getLocalDirs(type_list):
    """
    get local verson of the subdirs
    @param type_list: list of subdirs
    @type type_list: list
    """
    local_dirs = {}
    for d in subdir_dict.keys() :
        local_dirs[d] = subdir_dict[d].split(os.pathsep)[0]

    return local_dirs

project_conf_cache = []

def getCachedProjectConf(name):
    """
    cache of the project configurations already found.
    @param name: name of the project
    @type name: string
    """
    import LbConfiguration.Project #@UnusedImport
    import LbConfiguration.Package
    import LbConfiguration.External
    global project_conf_cache
    p = None
    exclude_list  = [x.name() for x in project_conf_cache]
    exclude_list += [x.name() for x in package_conf_cache]
    exclude_list += [x.upper() for x in LbConfiguration.Package.package_names]
    exclude_list += [x.upper() for x in LbConfiguration.Package.project_names]
    exclude_list += [x.upper() for x in LbConfiguration.External.external_projects + lcg_tar]
    if name.upper() not in exclude_list :
        p = None
        try :
            p = LbConfiguration.Project.getProject(name, svn_fallback=True, raise_exception=False)
        except :
            try :
                p = LbConfiguration.Project.getProject(name)
            except :
                pass
        if p :
            project_conf_cache.append(p)
    else :
        for x in project_conf_cache :
            if name.upper() == x.NAME() :
                p = x
                break
    return p


package_conf_cache = []

def getCachedPackageConf(name):
    """
    cache of the package configurations already found.
    @param name: name of the package
    @type name: string
    """
    import LbConfiguration.Package
    import LbConfiguration.Project
    import LbConfiguration.External
    global package_conf_cache
    p = None
    exclude_list  = [x.name() for x in package_conf_cache]
    exclude_list += [x.name() for x in project_conf_cache]
    exclude_list += [x.upper() for x in LbConfiguration.Project.project_names]
    exclude_list += [x.upper() for x in LbConfiguration.Package.project_names]
    exclude_list += [x.upper() for x in LbConfiguration.External.external_projects + lcg_tar]
    if name.upper() not in exclude_list :
        p = None
        try :
            p = LbConfiguration.Package.getPackage(name, svn_fallback=True, raise_exception=False)
        except :
            try :
                p = LbConfiguration.Package.getPackage(name)
            except :
                pass
        if p :
            package_conf_cache.append(p)
    else :
        for x in package_conf_cache :
            if name.lower() == x.name() :
                p = x
                break
    return p



#----------------------------------------------------------------------------------

_postinstall_commands = {}
_post_install_env = {}


def registerPostInstallCommand(project, version, command, dirname=None):
    global _postinstall_commands
    log = logging.getLogger()
    if (project, version) in _postinstall_commands.keys() :
        cmdlist = _postinstall_commands[(project,version)]
    else :
        cmdlist = []
    cmdlist.append((command, dirname))
    _postinstall_commands[(project,version)] = cmdlist
    if dirname :
        log.debug("Registered PostInstall for %s %s: \"%s\" in %s" % (project, version, command, dirname))
    else :
        log.debug("Registered PostInstall for %s %s: \"%s\"" % (project, version, command))



def callPostInstallCommand(project, version):

    # get base environment to run the post install script.
    # has to be done only once per session
    global _post_install_env
    log = logging.getLogger()

    if not _post_install_env :
        log_level = log.getEffectiveLevel()
        _post_install_env = dict(os.environ)
        import LbConfiguration.LbLogin
        llsargs = []
        if debug_flag :
            llsargs.append("--debug")
        else :
            llsargs.append("--silent")
        llsargs.append("--shell=sh")
        llsargs.append("--mysiteroot=%s" % os.environ["MYSITEROOT"])
        llsargs.append("--scripts-version=%s" % lbscripts_version)
        llsargs.append("--cmtconfig=%s" % cmtconfig)
        log.debug("Running LbLogin %s" % " ".join(llsargs))
        tmp_env = LbConfiguration.LbLogin.getLbLoginEnv(llsargs)
        for var in tmp_env.keys() :
            _post_install_env[var] = tmp_env[var]
        log.setLevel(log_level)
    projcmds = _postinstall_commands.get((project,version), None)
    if projcmds :
        for c in projcmds :
            log.debug("Executing PostInstall for %s %s: \"%s\" in %s" % (project, version, c[0], c[1]))
            rc = systemCall(c[0], workdir=c[1], env=_post_install_env)
            if rc != 0 :
                log.error("PostInstall command for %s %s returned %d" % (project, version, rc))
    else :
        log.debug("Project %s %s has no postinstall command" % (project, version))


def isPostInstallRegistered(project, version):
    registered = False
    if (project, version) in _postinstall_commands.keys() :
        registered = True
    return registered

#----------------------------------------------------------------------------------

_update_commands = {}


def registerUpdateCommand(project, version, command, dirname=None):
    global _update_commands
    log = logging.getLogger()
    if (project, version) in _update_commands.keys() :
        cmdlist = _update_commands[(project,version)]
    else :
        cmdlist = []
    cmdlist.append((command, dirname))
    _update_commands[(project,version)] = cmdlist
    if dirname :
        log.debug("Registered Update for %s %s: \"%s\" in %s" % (project, version, command, dirname))
    else :
        log.debug("Registered Update for %s %s: \"%s\"" % (project, version, command))



def callUpdateCommand(project, version):

    # get base environment to run the post install script.
    # has to be done only once per session
    global _post_install_env

    log = logging.getLogger()
    if not _post_install_env :
        log_level = log.getEffectiveLevel()
        _post_install_env = dict(os.environ)
        import LbConfiguration.LbLogin
        llsargs = []
        if debug_flag :
            llsargs.append("--debug")
        else :
            llsargs.append("--silent")
        llsargs.append("--shell=sh")
        llsargs.append("--mysiteroot=%s" % os.environ["MYSITEROOT"])
        llsargs.append("--scripts-version=%s" % lbscripts_version)
        llsargs.append("--cmtconfig=%s" % cmtconfig)
        log.debug("Running LbLogin %s" % " ".join(llsargs))
        tmp_env = LbConfiguration.LbLogin.getLbLoginEnv(llsargs)
        for var in tmp_env.keys() :
            _post_install_env[var] = tmp_env[var]
        log.setLevel(log_level)
    projcmds = _update_commands.get((project,version), None)
    if projcmds :
        cmdargs = []
        if debug_flag:
            cmdargs.append("--debug")
        if overwrite_mode:
            cmdargs.append("--overwrite")
        for cmd, wd in projcmds:
            cmd = " ".join([cmd] + cmdargs)
            log.info("Executing Update for %s %s: \"%s\" in %s" % (project, version, cmd, wd))
            rc = systemCall(cmd, workdir=wd, env=_post_install_env)
            if rc != 0 :
                log.error("Update command for %s %s returned %d" % (project, version, rc))
    else :
        log.debug("Project %s %s has no postinstall command" % (project, version))
    return rc


def isUpdateRegistered(project, version):
    registered = False
    if (project, version) in _update_commands.keys() :
        registered = True
    return registered

def getCommandFile(postscr, flavor="PostInstall"):
    """
    get the file to be executed in either PostInstall or Update action.
    returns None if no valid file has been found.
    """
    cmd_file = None
    this_dir = getLocalDirs(["lhcb"])
    lhcb_rooted_dir = postscr.replace(this_dir["lhcb"],"").lstrip(os.sep)
    if flavor == "Update" : # look at the first valid entry in the chain
        cmd_file = _multiPathGetFirst(subdir_dict["lhcb"], lhcb_rooted_dir, None)
    elif os.path.exists(postscr) :
        cmd_file = postscr

    return cmd_file

def getProjectCommandFile(pack_ver, flavor="PostInstall"):
    postscr_name = "%s.py" % flavor
    postscr = None
    try :
        if "/" in pack_ver[0] :
            p_name = pack_ver[0].split("/")[-1]
        else :
            p_name = pack_ver[0]
        prj = getCachedProjectConf(p_name)
        if prj :
            cmtcontainer = os.path.join(pack_ver[3], prj.SteeringPackage(), "cmt")
            postscr = os.path.join(cmtcontainer, postscr_name)
        else :
            pkg = getCachedPackageConf(p_name)
            if pkg :
                cmtcontainer = os.path.join(pack_ver[3], "cmt")
                postscr = os.path.join(cmtcontainer, postscr_name)
        if postscr :
            postscr = getCommandFile(postscr, flavor)
    except ImportError:
        pass
    return postscr


def registerProjectCommand(pack_ver, flavor="PostInstall"):
    try :
        if "/" in pack_ver[0] :
            p_name = pack_ver[0].split("/")[-1]
        else :
            p_name = pack_ver[0]

        postscr = getProjectCommandFile(pack_ver, flavor)

        if postscr :
            if flavor == "PostInstall" :
                registerPostInstallCommand(p_name, pack_ver[1], "python %s" % postscr, os.path.dirname(postscr))
            if flavor == "Update" :
                registerUpdateCommand(p_name, pack_ver[1], "python %s" % postscr, os.path.dirname(postscr))
    except ImportError:
        pass

_updated_items = {}

def isUpdated(pname, pversion):
    updated = False
    if (pname, pversion) in _updated_items :
        if _updated_items[(pname, pversion)] :
            updated = True

    return updated

def setUpdated(pname, pversion):
    _updated_items[(pname, pversion)] = True


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
        shutil.rmtree(path, ignore_errors=True)
    except :
        if os.path.exists(path) :
            if os.path.isdir(path) and not os.path.islink(path):
                lst = os.listdir(path)
                for p in lst:
                    removeAll(os.path.join(path, p))
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
        authbits2 = stat.S_IMODE(os.stat(basedir)[stat.ST_MODE])
        authbits = authbits1 | authbits2
        globalmask = 0777 - authbits
        os.umask(globalmask)
        log.debug("Calculating global permissions:")
        log.debug("Read/write permissions according to mask %04o are %04o" % (themask, 0777 - themask))
        log.debug("Read/write permissions according to MYSITEROOT are %04o" % authbits2)
    return globalmask

def fixWinAttrib(dirpath):
    os.chmod(dirpath, stat.S_IWRITE)
    if os.path.isdir(dirpath):
        if os.listdir(dirpath) :
            systemCall("attrib -R -A -H /S /D", workdir=dirpath)
    else :
        systemCall("attrib -R -A -H %s" % dirpath)


def changePermissions(directory, recursive=True):
    """ change permissions according to the umask. and the
    MYSITEROOT directory permissions """
    log = logging.getLogger()
    if sys.platform != "win32" :
        # add write permissions to the user if needed
        authbits = 0777 - getGlobalMask()
        if recursive and os.path.isdir(directory):
            log.debug("Adding read/write permissions %04o recursively to %s" % (authbits, directory))
        else :
            log.debug("Adding read/write permissions %04o to %s" % (authbits, directory))
        addPermissions(authbits, directory, recursive)
    else :
        log.debug("Adding read/write permissions recursively to %s" % directory)
        fixWinAttrib(directory)
        if recursive :
            for root, dirs, files in os.walk(directory, topdown=True) :
                for d in dirs :
                    fixWinAttrib(os.path.join(root, d))
                for f in files :
                    os.chmod(os.path.join(root, f), stat.S_IWRITE)

def checkWriteAccess(directory):
    dirok = True
    log = logging.getLogger()

    canwrite = os.access(directory, os.W_OK)
    if not canwrite :
        log.warning("No POSIX write permission in %s" % directory)
    dirok = dirok & canwrite

    canread = os.access(directory, os.R_OK)
    if not canread :
        log.warning("No POSIX read permission in %s - cannot list directory" % directory)
    dirok = dirok & canread

    canexe = os.access(directory, os.X_OK)
    if not canexe :
        log.warning("No POSIX execute permission in %s - cannot cd in there" % directory)
    dirok = dirok & canexe


    return dirok

#----------------------------------------------------------------------------

def getTmpDirectory():
    return cur_tmp_dir

def createTmpDirectory():
    log = logging.getLogger()
    if os.path.isdir(getTmpDirectory()) :
        log.debug('     %s exists' % getTmpDirectory())
        destroyTmpDirectory()
    os.mkdir(getTmpDirectory())
    if fix_perm :
        changePermissions(getTmpDirectory(), recursive=True)
    log.info('     %s created' % getTmpDirectory())

def destroyTmpDirectory():
    log = logging.getLogger()
    log.debug('Removing %s' % getTmpDirectory())
    removeAll(getTmpDirectory())

def cleanTmpDirectory():
    log = logging.getLogger()
    log.debug('     Cleaning up %s' % getTmpDirectory())
    tmpdir = getTmpDirectory()
    for f in os.listdir(tmpdir) :
        removeAll(os.path.join(tmpdir, f))

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
def createDir(here, logname):
    log = logging.getLogger()

    log.info('create necessary sub-directories')

    this_dir = getLocalDirs(["log"])

    good = False
    if checkWriteAccess(here) :
        good = True
        if not os.path.isdir(this_dir["log"]):
            safeMakeDirs(this_dir["log"])
        elif logname:
            if os.path.exists(os.path.join(this_dir["log"], logname + '_old')):
                os.remove(logname + '_old')
            if os.path.exists(logname):
                os.rename(logname, logname + '_old')

        for dirnm in subdir_dict.keys():
            if os.path.isdir(os.path.join(here, dirnm)):
                log.debug('%s exists in %s ' % (dirnm, here))
            else:
                safeMakeDirs(dirnm)
                if dirnm == 'lcg':
                    safeMakeDirs(os.path.join(dirnm, 'external'))
                if dirnm == 'lhcb':
                    if multiple_mysiteroot :
                        found_dbase = False
                        found_param = False
                        found_tools = False
                        for b in subdir_dict["lhcb"].split(os.pathsep)[1:] :
                            if os.path.isdir(os.path.join(b, 'DBASE')) :
                                found_dbase = True
                            if os.path.isdir(os.path.join(b, 'PARAM')) :
                                found_param = True
                            if os.path.isdir(os.path.join(b, 'TOOLS')) :
                                found_tools = True
                        if not found_dbase :
                            safeMakeDirs(os.path.join(dirnm, 'DBASE'))
                        if not found_param :
                            safeMakeDirs(os.path.join(dirnm, 'PARAM'))
                        if not found_tools :
                            safeMakeDirs(os.path.join(dirnm, 'TOOLS'))
                        if found_dbase or found_param or found_tools:
                            safeMakeDirs(os.path.join(dirnm, 'EXTRAPACKAGES'))
                            safeMakeDirs(os.path.join(dirnm, 'EXTRAPACKAGES', 'cmt'))
                            f = open(os.path.join(dirnm, 'EXTRAPACKAGES', 'cmt', 'project.cmt'), "w")
                            f.write("project EXTRAPACKAGES \n\n")
                            f.close()
                    else :
                        safeMakeDirs(os.path.join(dirnm, 'DBASE'))
                        safeMakeDirs(os.path.join(dirnm, 'PARAM'))
                        safeMakeDirs(os.path.join(dirnm, 'TOOLS'))
                if fix_perm :
                    changePermissions(dirnm, recursive=True)
            if dirnm == "tmp" :
                createTmpDirectory()
    else :
        log.warning("Cann't write in the %s folder" % here)
        good = False

    return good

#
#  install CMT if not there ===============================================
#
def fixCMTSetup(cmt_mgr_dir):
    """ fix the location of CMTROOT to make it re-locatable """
    log = logging.getLogger()
    shellext = [ "sh", "csh" ]
    gard_line = "# FIXED by install_project.py"
    for s in shellext :
        sfile = os.path.join(cmt_mgr_dir, "setup.%s" % s)
        if os.path.isfile(sfile) :
            sfile_lines = open(sfile).readlines()
            if gard_line in sfile_lines[0] :
                log.debug("%s already fixed" % sfile)
                continue
            else :
                log.info("Fixing %s" % sfile)
                sfile_lines.insert(0, "%s\n" % gard_line)
                if s == "sh" :
                    for i, l in zip(range(len(sfile_lines)), sfile_lines) :
                        if l.startswith("CMTROOT=") :
                            new_l = 'if [[ ! -n "$CMTROOT" ]]; then\n  %sfi\n' % l
                            sfile_lines[i] = new_l
                            break
                elif s == "csh" :
                    for i, l in zip(range(len(sfile_lines)), sfile_lines) :
                        if l.startswith("setenv CMTROOT ") :
                            new_l = "if ( ! $?CMTROOT ) then\n  %sendif\n" % l
                            sfile_lines[i] = new_l
                            break
                sfile_bak = os.path.join(cmt_mgr_dir, "setup.%s.sav" % s)
                if os.path.exists(sfile_bak) :
                    log.debug("Removing %s" % sfile_bak)
                    os.remove(sfile_bak)
                safeMove(sfile, sfile_bak)
                log.debug("Writing %s" % sfile)
                nsf = open(sfile, "w")
                nsf.write("".join(sfile_lines))
                nsf.close()

def getCMT(version=0):
    log = logging.getLogger()
    log.debug('install CMT if not there')
    here = os.getcwd()

    this_dir = getLocalDirs(["contrib", "targz"])
    this_contrib_dir = this_dir["contrib"]
    this_targz_dir = this_dir["targz"]

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
        log.debug('get cmtversion from argument %s ' % cmtvers)

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
        cmtbin = 'Linux-' + hw

    os.environ['CMTBIN'] = cmtbin

    fname = 'CMT_' + cmtvers + '_' + platform + '.tar.gz'
    # if the version does not exist get it from the web site
    if overwrite_mode or not isInstalled(fname):
        os.chdir(this_contrib_dir)

        # get the tar fname
        log.info('get %s' % os.path.join(this_contrib_dir, 'CMT', cmtvers, platform))
        getFile(url_dist + 'source/', 'CMT_' + cmtvers + '_' + platform + '.tar.gz')
        # untar it
        checkWriteAccess(this_contrib_dir)
        os.chdir(this_contrib_dir)
        rc = unTarFileInTmp(os.path.join(this_targz_dir, fname), os.getcwd(), overwrite=overwrite_mode)
        if rc != 0 :
            removeAll(os.path.join(this_contrib_dir, 'CMT'))
            log.info('CMT directory removed')
            sys.exit("getCMT: Exiting ...\n")

        # install CMT
        os.chdir(os.path.join(this_contrib_dir, 'CMT', cmtvers, 'mgr'))
        if sys.platform == 'win32':
            rc = systemCall('call INSTALL.bat')
        else:
            output = commands.getstatusoutput('./INSTALL')[1]
            for l in output.split("\n") :
                log.debug(l)
        log.info('install CMT %s' % cmtvers)
        setInstalled(fname)
        os.environ['CMTROOT'] = os.path.join(this_contrib_dir, 'CMT', cmtvers)
    else:
        location = getInstallLocation(fname)
        log.info('CMT %s is already installed in %s' % (cmtvers, location))
        for cd in subdir_dict["contrib"].split(os.pathsep) :
            if cd.startswith(location) :
                that_contrib_dir = cd
                break
        os.environ['CMTROOT'] = os.path.join(that_contrib_dir, 'CMT', cmtvers)

    fixCMTSetup(os.path.join(this_contrib_dir, 'CMT', cmtvers, 'mgr'))

    newpath = os.path.join(os.getenv('CMTROOT'), cmtbin) + os.pathsep + os.getenv('PATH')
    os.environ['PATH'] = newpath

    if debug_flag :
        log.debug('CMTROOT %s' % os.getenv('CMTROOT'))
        log.debug('PATH %s ' % os.getenv('PATH'))
        log.debug('CMTBIN %s ' % os.getenv('CMTBIN'))

    os.chdir(here)


#
#  download a file from the web site in targz_dir =================================
#
def getFile(url, fname):
    log = logging.getLogger()
    isConf = False
    if not url.endswith("/") :
        url = url + "/"

    if fname.find('.tar.gz') != -1:
        filetype = 'x-gzip'
        this_targz_dir = subdir_dict["targz"].split(os.pathsep)[0]
        dest = os.path.join(this_targz_dir, fname)
        if url.find('scripts') != -1 or url.find('system') != -1:
            if os.path.exists(dest) :
                try:
                    os.remove(dest)
                except:
                    log.info("can't remove file name %s" % dest)
    elif fname.find('.html') != -1 or fname.find('.htm') != -1 :
        this_html_dir = subdir_dict["html"].split(os.pathsep)[0]
        dest = os.path.join(this_html_dir, fname)
        filetype = 'html'

    else:
        filetype = 'text'
        if fname.find('.py') != -1:
            dest = fname
            if fname == 'install_project.py' :
                dest = 'latest_install_project.py'
            if os.path.exists(dest):
                try:
                    os.remove(dest)
                except:
                    log.warning("Can't remove file name %s" % dest)
        elif url.endswith("conf") or url.endswith("conf/") :
            # to download configuration files
            this_conf_dir = subdir_dict["conf"].split(os.pathsep)[0]
            dest = os.path.join(this_conf_dir, fname)
            isConf = True


    exist_flag = False
    log.debug('%s on %s' % (fname, dest))

    if not os.path.exists(dest):
        local_retries = nb_retries + 1
        hasbeendownloaded = False
        while (not hasbeendownloaded and (local_retries > 0)) :
            log.info("Retrieving %s" % url + fname)
            h = retrieve(url + fname, dest)[1]
            if h.type.find(filetype) == -1:
                log.warning("Can't download %s - retry" % fname)
                os.remove(dest)
            else:
                if md5_check and ( isTarBall(dest) or isConf ) :
                    if not checkMD5(url, fname, os.path.dirname(dest)):
                        removeReferenceMD5(fname, os.path.dirname(dest))
                        os.remove(dest)
                        log.error('md5 check failed on %s' % fname)
                    else:
                        hasbeendownloaded = True
                else:
                    hasbeendownloaded = True
            local_retries = local_retries - 1

        if not hasbeendownloaded:
            sys.exit("Unable to download %s after %s attempts - exit " % (fname, nb_retries) + '\n')

    return exist_flag

def getFileContent(url):
    fd = urlopen(url)
    content = fd.readlines()
    fd.close()
    return content

def isTarBall(filename):
    fc = filename.split(".")
    if fc[-1] == "gz" and fc[-2] == "tar" :
        return True
    else :
        return False

# --------------------------------------------------------------------------------------

def calculateMD5(filename):
    if python_version >= (2, 6, 0) :
        from hashlib import md5 #@UnusedImport
    else :
        from md5 import md5 #@Reimport
    m = md5()
    f = open(filename, "rb")
    buf = f.read(2 ** 13)
    while(buf):
        m.update(buf)
        buf = f.read(2 ** 13)
    return m.hexdigest()

def removeReferenceMD5(fnm, dest):
    log = logging.getLogger()
    md5name = getMD5FileName(fnm)
    filename = os.path.join(dest, md5name)
    if os.path.exists(filename):
        try:
            os.remove(filename)
        except:
            log.warning("Can't remove file %s" % filename)

def extractMD5SumFromFile(filename):
    ftext = open(filename).read()
    return ftext.split()[0]


def getReferenceMD5(url, filen, dest):
    log = logging.getLogger()
    md5name = getMD5FileName(filen)
    filename = os.path.join(dest, md5name)
    if md5name :
        if os.path.exists(filename) :
            os.remove(filename)
        filename = retrieve(url + '/' + md5name, filename)[0]
    else:
        log.warning("Can't retrieve %s" % md5name)

    md5sum = extractMD5SumFromFile(filename)
    return md5sum

def checkMD5(url, filenm, dest):
    log = logging.getLogger()
    isok = False
    log.info("Checking %s file consistency ..." % filenm)
    refmd5 = getReferenceMD5(url, filenm, dest)
    log.debug("   reference md5 sum is: %s" % refmd5)
    compmd5 = calculateMD5(os.path.join(dest, filenm))
    log.debug("       local md5 sum is: %s" % compmd5)
    if refmd5 == compmd5 :
        isok = True
    return isok

def isMD5Valid(url, filename):
    """ This function checks the validity of the md5 sum file that was
        installed from a previous installation """
    pass


def getMD5FileName(filename):
    """
    extract the name of the corresponding MD5 sum check file
    it strips all the file extensions.
    @param filename: initial file name
    """

    if filename.endswith(".tar.gz") :
        fc = filename.split(".")
        name = ".".join(fc[:-2]) + ".md5"
    elif filename.endswith(".html") :
        fc = filename.split(".")
        name = ".".join(fc[:-1]) + ".md5"
    else :
        while True :
            froot = os.path.splitext(filename)[0]
            if froot == filename :
                break
            else :
                filename = froot
        name = ".".join([filename, "md5"])

    return name

# --------------------------------------------------------------------------------------

def getHTMLFileName(filename):
    fc = filename.split(".")
    name = ".".join(fc[:-2]) + ".html"
    return name
#
#  get package name, version and binary =======================================
#
def getPackVer(fname):

    this_lhcb_dir = subdir_dict["lhcb"].split(os.pathsep)[0]
    this_lcg_dir = subdir_dict["lcg"].split(os.pathsep)[0]
    this_contrib_dir = subdir_dict["contrib"].split(os.pathsep)[0]


    # get the binary if any
    if fname.find("LBSCRIPTS") == -1 :
        import LbConfiguration.Platform
        if LbConfiguration in sys.modules.values() :
            reload(LbConfiguration)
        if LbConfiguration.Platform in sys.modules.values() :
            reload(LbConfiguration.Platform)

        binary_list = LbConfiguration.Platform.binary_dbg_list + LbConfiguration.Platform.binary_opt_list
        if extra_binary :
            binary_list.append(extra_binary)
        for b in binary_list:
            if fname.find(b) != -1:
                binary = b
                if fname.find('_' + b) != -1 :
                    ffile = fname[:fname.find('_' + b)]
                else :
                    ffile = fname[:fname.find('.tar.gz')]
                break
            else:
                binary = None
                ffile = fname[:fname.find('.tar.gz')]
    else :
        binary = None
        ffile = fname[:fname.find('.tar.gz')]
    packver = ffile.split('_')
    vers = packver[-1]
    name = packver[0]
    file_path = os.path.join(this_lhcb_dir, name, name + '_' + vers)
    base_dir = subdir_dict["lhcb"].split(os.pathsep)
    file_base = []
    for bd in base_dir :
        file_base.append(os.path.join(bd, name, name + '_' + vers))
    if name == "LBSCRIPTS" :
        file_base = []
        file_base.append(os.path.join(base_dir[0], name, name + '_' + vers))
    if  name == "GENSER" or name in lcg_tar:
        if len(packver) >= 2:
            vers = '_'.join(packver[1:])
            file_path = os.path.join(this_lcg_dir, name, name + '_' + vers)
            base_dir = subdir_dict["lcg"].split(os.pathsep)
            file_base = []
            for bd in base_dir :
                file_base.append(os.path.join(bd, name, name + '_' + vers))
    if name == "OpenScientist" or name == "osc_vis" or name == "DIM" :
        file_path = os.path.join(this_contrib_dir, name, vers)
        base_dir = subdir_dict["contrib"].split(os.pathsep)
        file_base = []
        for bd in base_dir :
            file_base.append(os.path.join(bd, name, vers))
    if packver[0] == "DBASE" or packver[0] == "PARAM" or packver[0] == "TOOLS":
        if len(packver) == 3:
            name = packver[1]
        else:
            name = os.path.join(packver[1], packver[2])
        file_path = os.path.join(this_lhcb_dir, packver[0], name, vers)
        base_dir = subdir_dict["lhcb"].split(os.pathsep)
        file_base = []
        if len(base_dir) > 1 :
            file_base.append(os.path.join(this_lhcb_dir, 'EXTRAPACKAGES', name, vers))
        for bd in base_dir :
            file_base.append(os.path.join(bd, packver[0], name, vers))
    return (name, vers, binary, file_path, file_base)

#
#  get the project_list =====================================================
#

def updateProjectList(src_list, target_list):
    log = logging.getLogger()
    for s in src_list.keys() :
        if s not in target_list.keys() :
            log.debug("Adding %s to the project list" % s)
            target_list[s] = src_list[s]

def updateHTMLList(src_list, target_list):
    log = logging.getLogger()
    for s in src_list :
        if s not in target_list :
            log.debug("Adding %s to the html list" % s)
            target_list.append(s)

def getTarBallNameItems(tar_name):
    name = None
    version = None
    binary = None
    core_name = tar_name.replace(".tar.gz", "")
    from LbConfiguration.Platform import binary_list
    for b in binary_list :
        if core_name.endswith(b) :
            binary = b
            core_name = core_name.replace("_%s" % b, "")
            break

    cptes = core_name.split("_")
    nm = cptes[0]
    from LbConfiguration import Project, Package
    if nm.upper() in [ x.upper() for x in Project.project_names ] :
        version = cptes[-1]
        for x in Project.project_names :
            if nm.upper() == x.upper() :
                name = x
                break
    elif nm.upper() in [ x.upper() for x in Package.project_names ] :
        version = cptes[-1]
        for c in cptes :
            if c in Package.package_names :
                name = c
                break
    else :
        if nm in lcg_tar :
            name = nm
            version = cptes[1]
    return name, version, binary


def getUnknowTarBallNameItems(tb_name, binary):
    corename = tb_name.replace(".tar.gz", "")
    s_binary = None
    if corename.endswith(binary) :
        s_binary = binary
        corename = corename.replace(binary, "")
    coreitems = corename.split("_")
    s_version = coreitems[-1]
    s_name = coreitems[1]
    return s_name, s_version, s_binary



def getProjectList(name, version, binary=None, recursive=True):
    log = logging.getLogger()
    log.debug('get list of projects to install %s %s %s' % (name, version, binary))
    here = os.getcwd()

    project_list = {}
    html_list = []

    import LbConfiguration.Platform #@UnusedImport
    import LbConfiguration.Package

    p = getCachedPackageConf(name)

    if p :
        tar_file = p.tarBallName(version)
    elif name in lcg_tar:
        tar_file = "_".join([name, version])
        if binary :
            tar_file += "_%s" % binary
    else:
        pj = getCachedProjectConf(name)
        if pj :
            tar_file = pj.tarBallName(version, binary)
        else :
            tar_file = name.upper() + "_" + name.upper()
            if version != 0 :
                tar_file += "_%s" % version
            if binary:
                tar_file += "_%s" % binary

    this_html_dir = subdir_dict["html"].split(os.pathsep)[0]

    os.chdir(this_html_dir)

    tar_file_html = tar_file + ".html"

    if not check_only :
        checkWriteAccess(this_html_dir)
        if overwrite_mode and os.path.exists(tar_file_html) :
            os.remove(tar_file_html)
        getFile(url_dist + "html/", tar_file_html)


    if os.path.exists(tar_file_html) :
        fd = open(tar_file_html)
        fdlines = fd.readlines()
        fd.close()
    else :
        try :
            fdlines = getFileContent(url_dist + "html/" + tar_file_html)
        except :
            log.fatal("Can't retrieve dependency information for %s %s %s" % (name, version, binary))
            sys.exit(1)
    for fdline in fdlines:
        if fdline.find('was not found on this server') != -1:
            if name in lcg_tar :
                log.debug("The required project %s %s %s is not available. Skipping ..." % (name, version, binary))
                if os.path.exists(tar_file_html) :
                    log.debug("Removing %s" % tar_file_html)
                    os.remove(tar_file_html)
                return project_list, html_list
            else :
                log.fatal("the required project %s %s %s is not available" % (name, version, binary))
                if os.path.exists(tar_file_html) :
                    log.info("remove %s and exit" % tar_file_html)
                    os.remove(tar_file_html)
                sys.exit(1)
        if fdline.find('HREF=') != -1:
            eq_sign = fdline.find('HREF=')
            gt_sign = fdline.find('>')
            slash_sign = eq_sign + fdline[eq_sign:].find('/')
            source = fdline[eq_sign + 5:slash_sign]
            fname = fdline[slash_sign + 1:gt_sign]
            project_list[fname] = source
            html_list.append(fname)

    # loop over projects to be downloaded
    for fname in project_list.keys():
        if project_list[fname] == "source":
            pack_ver = getPackVer(fname)
            if pack_ver[2] != cmtconfig:

                #
                # XXX Hack for the do0 config
                # The issue needs to be sorted carefully.
                # Not sure why we are not trsuting what mkLHCbtar produced so need to investigate the use case
                if cmtconfig.endswith("-do0"):
                    # Just do not apply this rules for this platform...
                    continue

                # Normal processing, to be reviewed urgently
                del project_list[fname]
                html_list.remove(fname)
                ## There was a condition for this update which was NOT necessary.
                #if cmtconfig in LbConfiguration.Platform.binary_list:
                ## At this point the platform has been checked and we have to consider it as valid,
                ## in order to be able top use platforms unknow by Platform.py
                newbin = cmtconfig
                if LbConfiguration.Platform.isBinaryDbg(newbin) :
                    newbin = LbConfiguration.Platform.getBinaryOpt(newbin)
                fname = fname.replace(pack_ver[2], newbin)
                project_list[fname] = "source"
                html_list.append(fname)

    os.chdir(here)

    if recursive :
        for tb_name in html_list :
            s_name, s_version, s_binary = getTarBallNameItems(tb_name)
            if not s_binary and binary and (tb_name.find(binary) != -1) :
                _, s_version, s_binary = getUnknowTarBallNameItems(tb_name, binary)
            if s_name and s_version :
                if not (s_name.upper() == name.upper() and s_version == version and s_binary == binary) :
                    if s_name.upper() not in [x.upper() for x in LbConfiguration.Package.project_names] :
                        sub_project_list, sub_html_list = getProjectList(s_name, s_version, s_binary)
                        updateProjectList(sub_project_list, project_list)
                        updateHTMLList(sub_html_list, html_list)

    return project_list, html_list

#----------------------------------------------------------------------------------
# check installation
def isInstalled(fname):
    installed = False
    if not overwrite_mode :
        # special case: the LbScripts project has to be installed locally anyway
        if fname.find("LbScripts") != -1 or fname.find("LBSCRIPTS") != -1 :
            installedfilename = os.path.join(subdir_dict["log"].split(os.pathsep)[0], fname.replace(".tar.gz", ".installed"))
            if os.path.exists(installedfilename) :
                installed = True
        else :
            # regular case: the project can be installed in different locations
            for ld in subdir_dict["log"].split(os.pathsep) :
                installedfilename = os.path.join(ld, fname.replace(".tar.gz", ".installed"))
                if os.path.exists(installedfilename) :
                    installed = True
                    break
    return installed

def getInstallLocation(fname) :
    location = None
    if isInstalled(fname) :
        for ld in subdir_dict["log"].split(os.pathsep) :
            installedfilename = os.path.join(ld, fname.replace(".tar.gz", ".installed"))
            if os.path.exists(installedfilename) :
                location = os.path.dirname(ld)
                break

    return location

def setInstalled(filenm):
    this_log_dir = subdir_dict["log"].split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir, filenm.replace(".tar.gz", ".installed"))

    if os.path.exists(installedfilename) :
        os.remove(installedfilename)

    f = open(installedfilename, "w")
    f.write("Done\n")
    f.close()

def delInstalled(fname):
    this_log_dir = subdir_dict["log"].split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir, fname.replace(".tar.gz", ".installed"))
    os.remove(installedfilename)

# check installed project
def checkInstalledProjects(project_list, main_file=None):
    log = logging.getLogger()
    log.info('check all project in the list %s' % project_list)
    if main_file :
        pack_ver = getPackVer(main_file)
        update_scr = getProjectCommandFile(pack_ver, "Update")
        if update_scr :
            log.error("%s is not installed" % main_file)
            sys.exit("some projects are not installed. Exiting ...")
            sys.exit()
    import LbConfiguration.Platform
    for f in project_list.keys() :
        if project_list[f] == "source":
            pack_ver = getPackVer(f)
            if pack_ver[2] != cmtconfig:
                if cmtconfig in LbConfiguration.Platform.binary_list:
                    newbin = cmtconfig
                    if LbConfiguration.Platform.isBinaryDbg(newbin) :
                        newbin = LbConfiguration.Platform.getBinaryOpt(newbin)
                    f = f.replace(pack_ver[2], newbin)
        location = getInstallLocation(f)
        if location :
            log.info("%s is installed in %s" % (f, location))
        else :
            log.error("%s is not installed" % f)
            sys.exit("some projects are not installed. Exiting ...")
    sys.exit()


#----------------------------------------------------------------------------------

def updateLHCbProjectPath(mysiteroot):
    cmtprojpathlist = _multiPathJoin(mysiteroot, "lhcb").split(os.pathsep)
    cmtprojpathlist += _multiPathJoin(mysiteroot, os.path.join("lcg", "external")).split(os.pathsep)
    if os.environ.has_key("LHCBPROJECTPATH") :
        cmtprojpathlist += os.environ["LHCBPROJECTPATH"].split(os.pathsep)
    finalcmtlist = []
    for d in cmtprojpathlist :
        if d not in finalcmtlist :
            finalcmtlist.append(d)
    os.environ["LHCBPROJECTPATH"] = os.pathsep.join(finalcmtlist)


def scriptsPostInstall(pack_ver):
    """
    post install script for LbScripts if it doesn't provide one.
    @param mysiteroot: full list of install dirs os.pathsep separated
    @type mysiteroot: string
    """
    log = logging.getLogger()
    this_lhcb_dir = subdir_dict["lhcb"].split(os.pathsep)[0]
    updateLHCbProjectPath(os.environ["MYSITEROOT"])
    log.debug("LHCBPROJECTPATH: %s" % os.environ.get("LHCBPROJECTPATH", None))
    if boot_script_loc :
        # for debugging
        genlogscript = os.path.join(boot_script_loc, "InstallArea", "scripts", "generateLogin")
    else :
        genlogscript = os.path.join(pack_ver[3], "InstallArea", "scripts", "generateLogin")

    gencmd = "%s" % genlogscript
    if debug_flag :
        gencmd += " --debug"
    gencmd += " --without-python --no-cache -m %s --login-version=%s %s" % (os.environ["MYSITEROOT"], pack_ver[1], os.path.join(pack_ver[3], "InstallArea"))
    log.info("Running: python %s" % gencmd)
    systemCall("python %s" % gencmd)
    prodlink = os.path.join(os.path.dirname(pack_ver[3]), "prod")
    if sys.platform != "win32" :
        if os.path.exists(prodlink) :
            if os.path.islink(prodlink) :
                os.remove(prodlink)
                os.symlink(pack_ver[0] + '_' + pack_ver[1], prodlink)
                log.debug("linking %s -> %s" % (prodlink, pack_ver[0] + '_' + pack_ver[1]))
            else :
                log.error("%s is not a link. Please remove this file/directory" % prodlink)
    else :
        if os.path.exists(prodlink) :
            if os.path.isdir(prodlink) :
                removeAll(prodlink)
                shutil.copytree(pack_ver[0] + '_' + pack_ver[1], prodlink)
                log.debug("Copying %s to %s" % (pack_ver[0] + '_' + pack_ver[1], prodlink))
            else :
                log.error("%s is not a directory. Please remove this file" % prodlink)
    my_dir = os.path.dirname(this_lhcb_dir)
    selected_script_dir = os.path.join(prodlink, "InstallArea", "scripts")
    if not os.path.exists(selected_script_dir) :
        selected_script_dir = os.path.join(pack_ver[3], "InstallArea", "scripts")
    for f in os.listdir(selected_script_dir) :
        if f.startswith("LbLogin.") and not (f.endswith(".zsh") or f.endswith(".py")):
            sourcef = os.path.join(selected_script_dir, f)
            targetf = os.path.join(my_dir, f)
            if os.path.islink(targetf) or os.path.isfile(targetf):
                os.remove(targetf)
            if sys.platform == "win32" :
                shutil.copy(sourcef, targetf)
                log.debug("copying %s into %s" % (sourcef, targetf))
            else :
                sourcef = sourcef.replace(my_dir, "", 1)
                while sourcef.startswith("/") or sourcef.startswith("\\") :
                    sourcef = sourcef[1:]
                os.symlink(sourcef, targetf)
                log.debug("linking %s -> %s" % (targetf, sourcef))
    etc_scripts = [ "LbLogin", "group_login", "group_shell", "LHCb"]
    my_etc_dir = os.path.join(my_dir, "etc")
    for s in etc_scripts :
        for f in os.listdir(selected_script_dir) :
            if f.startswith("%s." % s) and not (f.endswith(".zsh") or f.endswith(".py")):
                sourcef = os.path.join(selected_script_dir, f)
                targetf = os.path.join(my_etc_dir, f)
                if os.path.islink(targetf) or os.path.isfile(targetf):
                    os.remove(targetf)
                if sys.platform == "win32" :
                    shutil.copy(sourcef, targetf)
                    log.debug("copying %s into %s" % (sourcef, targetf))
                else :
                    sourcef = sourcef.replace(my_dir, "", 1)
                    while sourcef.startswith("/") or sourcef.startswith("\\") :
                        sourcef = sourcef[1:]
                    sourcef = os.path.join(os.pardir, sourcef)
                    os.symlink(sourcef, targetf)
                    log.debug("linking %s -> %s" % (targetf, sourcef))


def fixDataPackages(pack_ver):
    log = logging.getLogger()

    if multiple_mysiteroot :
        if os.path.isdir('EXTRAPACKAGES'):
            extradir = None
            if pack_ver[3].find('DBASE') != -1 :
                extradir = 'DBASE'
            elif pack_ver[3].find('PARAM') != -1 :
                extradir = 'PARAM'
            elif pack_ver[3].find('TOOLS') != -1 :
                extradir = 'TOOLS'
            if extradir is not None :
                f = os.path.join(pack_ver[0], pack_ver[1])
                tdir = os.path.dirname(os.path.join('EXTRAPACKAGES', f))
                safeMakeDirs(tdir)
                shutil.copytree(os.path.join(extradir, f), os.path.join('EXTRAPACKAGES', f))
                if fix_perm :
                    changePermissions(os.path.join('EXTRAPACKAGES', f), recursive=True)
                shutil.rmtree(extradir, ignore_errors=True)
    if latest_data_link and (pack_ver[3].find('DBASE') != -1 or pack_ver[3].find('PARAM') != -1 or pack_ver[3].find('TOOLS') != -1):
        # create link to the latest version "v99r9" if requested
        f = os.path.join(pack_ver[0], pack_ver[1])
        extradir = None
        if pack_ver[3].find('DBASE') != -1 :
            extradir = 'DBASE'
        elif pack_ver[3].find('PARAM') != -1 :
            extradir = 'PARAM'
        elif pack_ver[3].find('TOOLS') != -1 :
            extradir = 'TOOLS'
        exdir = os.path.join('EXTRAPACKAGES', f)
        regul_dir = os.path.join(extradir, f)
        if os.path.exists(regul_dir) :
            tg_dir = regul_dir
        else :
            tg_dir = exdir
        if os.path.isdir(tg_dir) :
            tdir = os.path.dirname(tg_dir)
            ltg_list = []
            if extradir == "TOOLS" :
                ltg = os.path.join(tdir, "pro")
                ltg_list.append(ltg)
            else :
                ltg = os.path.join(tdir, "v999r0")
                ltg_list.append(ltg)
                try:
                    import re
                    vs = re.compile(r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?')
                    m = vs.match(pack_ver[1])
                    if m :
                        majv = m.group(1)
                        ltg = os.path.join(tdir, "v%sr99" % majv)
                        ltg_list.append(ltg)
                except :
                    pass
            for lg in ltg_list :
                if os.path.islink(lg) or os.path.isfile(lg):
                    os.remove(lg)
                elif os.path.isdir(lg) :
                    shutil.rmtree(lg, ignore_errors=True)
                if sys.platform == "win32" :
                    log.debug("copying %s to %s" % (pack_ver[1], lg))
                    shutil.copytree(tg_dir, lg)
                else :
                    log.debug("linking %s -> %s" % (lg, pack_ver[1]))
                    os.symlink(pack_ver[1], lg)



#
#  download project tar files ================================================
#
def getProjectTar(tar_list, already_present_list=None):
    log = logging.getLogger()
    here = os.getcwd()

    this_lcg_dir = subdir_dict["lcg"].split(os.pathsep)[0]
    this_contrib_dir = subdir_dict["contrib"].split(os.pathsep)[0]
    this_lhcb_dir = subdir_dict["lhcb"].split(os.pathsep)[0]
    this_targz_dir = subdir_dict["targz"].split(os.pathsep)[0]

    soft_type = "LHCb"


    for fname in tar_list.keys():
        log.info('-' * line_size)
        pack_ver = getPackVer(fname)
        if "/" in pack_ver[0] :
            pname = pack_ver[0].split("/")[-1]
        else :
            pname = pack_ver[0]
        pversion = pack_ver[1]
        if not isInstalled(fname) or overwrite_mode :
            log.debug(fname)
            if tar_list[fname] == "source":
                getFile(url_dist + 'source/', fname)
                if fname.find('LCGCMT') != -1 or fname.find('LCGGrid') != -1 or fname.find('LCGGanga') != -1:
                    checkWriteAccess(os.path.join(this_lcg_dir, '..'))
                    os.chdir(os.path.join(this_lcg_dir, '..'))
                    soft_type = "LCG"
                elif fname.find('GENSER') != -1:
                    # Special case for GENSER. "Classic" tars are to be extracted in
                    # ${SITEROOT}/lcg/external, whereas new style installs contain
                    # the top directory which is "releases" anyway, as specified by LCG
                    # Unfortunately, we have no way to know, except by opening the tar
                    # and checking whether the top dir is "releases" or not.
                    # We need to more to the RPM install ASAP
                    # BC 2014/10/15
                    hasTopDir = tarContainsTopDir(os.path.join(this_targz_dir, fname))
                    if hasTopDir:
                        log.warning("GENSER tar has top dir")
                        topdir = os.path.join(this_lcg_dir, '..')
                        checkWriteAccess(topdir)
                        os.chdir(topdir)
                    else:
                        checkWriteAccess(this_lcg_dir)
                        os.chdir(this_lcg_dir)
                    soft_type = "LCG"
                else:
                    checkWriteAccess(this_contrib_dir)
                    os.chdir(this_contrib_dir)
                    soft_type = "Contrib"
                
            elif tar_list[fname] == "system":
                log.info("Obsolete package %s will not be installed. The Compat project is replacing it" % fname)
                continue
            else:
                checkWriteAccess(this_lhcb_dir)
                os.chdir(this_lhcb_dir)
                soft_type = "LHCb"
                getFile(url_dist + tar_list[fname] + '/', fname)


            # untar the file
            log.debug('untar file %s' % fname)
            rc = unTarFileInTmp(os.path.join(this_targz_dir, fname), os.getcwd(), overwrite=overwrite_mode)
            if rc != 0 and (pack_ver[0] != 'LCGGrid' or pack_ver[0] != 'LCGGanga') :
                removeAll(pack_ver[3])
                log.info('Cleaning up %s' % pack_ver[3])
                sys.exit("getProjectTar: Exiting ...")
            import LbConfiguration.External
            if pack_ver[0] in LbConfiguration.External.external_projects:
                try:
                    # if it is a ext_lhcb project
                    # create a ext_lhcb project/vers/binary directory
                    # to remember which binary tar file has been untar
                    if fname.find('GENSER_v') != -1:
                        # the GENSER project as such does not exist anylonger in LCG
                        os.chdir(this_lcg_dir)
                        safeMakeDirs('GENSER')
                        os.chdir('GENSER')
                        safeMakeDirs('GENSER_' + pack_ver[1])
                    os.chdir(pack_ver[3])
                    safeMakeDirs(pack_ver[2])
                except:
                    # ignore if we do not manage to create the special directory
                    # ... it does not seem to be needed
                    pass

            if soft_type == "LHCb" :
                # if binary is requested and InstallArea does not exist : set it
                if pack_ver[2] :
                    os.chdir(os.path.join(this_lhcb_dir, pack_ver[0], pack_ver[0] + '_' + pack_ver[1]))
                    if not os.path.exists(os.path.join('InstallArea', pack_ver[2])):
                        safeMakeDirs("InstallArea")
                        os.chdir('InstallArea')
                        safeMakeDirs(pack_ver[2])
                if sys.platform != 'win32' :
                    try :
                        from LbLegacy.ProjectLinks import fixLinks
                        log.debug("Fixing links in %s" % pack_ver[3])
                        fixLinks(pack_ver[3])
                    except :
                        log.warning("Cannot use fixLinks")

                fixDataPackages(pack_ver)

                registerProjectCommand(pack_ver, "PostInstall")

                # fall back procedure for LbScripts if there is no PostInstall.py script
                if pack_ver[0] == "LBSCRIPTS" and not isPostInstallRegistered(pack_ver[0], pack_ver[1]):
                    scriptsPostInstall(pack_ver)

            if isPostInstallRegistered(pname, pversion) :
                callPostInstallCommand(pname, pversion)

            setInstalled(fname)
        else :
            log.info('%s is already installed' % fname)
            if already_present_list != None:
                already_present_list.append(tar_list[fname])

        if not isUpdated(pname, pversion) and soft_type == "LHCb":
            registerProjectCommand(pack_ver, "Update")

        if isUpdateRegistered(pname, pversion) and not isUpdated(pname, pversion) :
            rc = callUpdateCommand(pname, pversion)
            if rc == 0 :
                setUpdated(pname, pversion)

    os.chdir(here)

# Autoupdate myself
def getMySelf():
    log = logging.getLogger()
    global check_self
    if not check_self:
        log.warning("Install_project automatic update disabled")
        return
    here = os.getcwd()
    mysiteroot = os.environ["MYSITEROOT"].split(os.pathsep)[0]
    os.chdir(mysiteroot)
    the_install = "install_project.py"
    new_install = "latest_%s" % the_install
    if  not os.path.exists(the_install) and _this_file :
        shutil.copy(_this_file, the_install)
        log.debug("Copied %s to %s" % (_this_file, the_install))
    if os.path.exists(new_install) :
        os.remove(new_install)
    inst_loc = url_dist
    if dev_install :
        inst_loc += "devel"
    getFile(inst_loc, the_install)
    if fix_perm :
        changePermissions(new_install, recursive=False)
    latest_version = os.popen("python %s --version" % new_install).read()[:-1]
    if script_version < latest_version :
        log.warning("You are running an old version of this script - latest version: %s" % latest_version)
        log.warning("Restarting with the latest one")
        old_install = "%s.old" % the_install
        if os.path.exists(old_install) :
            log.debug("Removing %s" % old_install)
            os.remove(old_install)
        if os.path.exists(the_install) : # needed if installing to a remote location
            shutil.copy(the_install, old_install)
        shutil.copy(new_install, the_install)
        newscript = os.path.join(mysiteroot, the_install)
        if sys.platform != "win32" :
            os.execv(sys.executable, [sys.executable, newscript] + sys.argv[1:])
        else :
            print "please restart the command. This cannot be done automatically on windows"
    os.chdir(here)

#
# download necessary scripts ==============================================
#
def getBootScripts():
    global LbLegacy
    global LbConfiguration
    global LbRelease
    log = logging.getLogger()
    here = os.getcwd()
    if not check_only :
        getMySelf()
        cleanBootScripts()
    scripttar = "LBSCRIPTS_LBSCRIPTS_%s.tar.gz" % lbscripts_version
    if isInstalled(scripttar) and not overwrite_mode :
        location = getInstallLocation(scripttar)
        log.debug("LbScripts %s is already installed in %s" % (lbscripts_version, location))
        for l in subdir_dict["lhcb"].split(os.pathsep) :
            if l.startswith(location) :
                that_lhcb_dir = l
                break
        sys.path.insert(0, os.path.join(that_lhcb_dir,
                                        "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version,
                                        "InstallArea", "python"))
    else :
        if not check_only :
            log.info("LbScripts %s is not installed. Dowloading it." % lbscripts_version)
            getFile(url_dist + 'LBSCRIPTS/', scripttar)
            this_bootscripts_dir = subdir_dict["bootscripts"].split(os.pathsep)[0]
            this_targz_dir = subdir_dict["targz"].split(os.pathsep)[0]
            safeMakeDirs(this_bootscripts_dir)
            checkWriteAccess(this_bootscripts_dir)
            os.chdir(this_bootscripts_dir)
            rc = unTarFileInTmp(os.path.join(this_targz_dir, scripttar), os.getcwd(), overwrite=True)
            os.chdir(here)
            if rc != 0 :
                removeAll(this_bootscripts_dir)
                sys.exit('getBootScripts: Exiting ...')
            else :
                if fix_perm :
                    changePermissions(this_bootscripts_dir, recursive=True)
                sys.path.insert(0, os.path.join(this_bootscripts_dir,
                                                "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version,
                                                "InstallArea", "python"))
            atexit.register(cleanBootScripts)
        else :
            log.critical("LbScripts %s is not installed and cannot be installed in check mode" % lbscripts_version)
            sys.exit(2)

    if boot_script_loc :
        sys.path.insert(0, os.path.join(boot_script_loc, "InstallArea", "python"))
        log.debug("Prepending %s to sys.path" % os.path.join(os.path.join(boot_script_loc, "InstallArea", "python")))

    log.debug("sys.path is %s" % os.pathsep.join(sys.path))

    LbLegacy        = __import__("LbLegacy")
    LbConfiguration = __import__("LbConfiguration")
    LbRelease       = __import__("LbRelease")

def cleanBootScripts():
    log = logging.getLogger()
    this_bootscripts_dir = subdir_dict["bootscripts"].split(os.pathsep)[0]
    if os.path.isdir(this_bootscripts_dir) :
        log.debug("Removing the %s directory" % this_bootscripts_dir)
        removeAll(this_bootscripts_dir)

def isConfUpToDate(name):
    """
    check if the md5 sums have changed or not. If the don't match
    the configuration is not up to date.
    @param name: name of the configuration file
    @type name: string
    """
    uptodate = False

    return uptodate

def getConfFile(name):
    this_dir = getLocalDirs(["conf"])
    fpath = os.path.join(this_dir["conf"], name)
    md5path = os.path.join(this_dir["conf"], "%s.md5" % name)
    # clean up first the remaining files if any
    if os.path.exists(fpath) :
        os.remove(fpath)
    if os.path.exists(md5path) :
        os.remove(md5path)
    getFile(url_dist + "conf/", name)

def updateConf():
    log = logging.getLogger()
    this_dir = getLocalDirs(["conf"])
    file_list = ["projects", "packages"]
    for f in file_list :
        fpath = os.path.join(this_dir["conf"], f)
        if os.path.exists(fpath) :
            if not isConfUpToDate(f) :
                log.debug("The %s configuration file is not up to date. Downloading it" % f )
                getConfFile(f)
        else:
            log.debug("The %s configuration file doesn't exist. Downloading it" % f )
            getConfFile(f)
    os.environ["LHCB_CONF_DIR"] = this_dir["conf"]

def showCompatibleConfigs():
    from LbConfiguration.Platform import NativeMachine
    m = NativeMachine()
    print " ".join(m.CMTSupportedConfig(debug=True))
#
#  list available versions ==============================================================================
#
def getVersionList(pname, ver=None):
    from LbConfiguration.Version import sortStrings

    datapackage = False

    log = logging.getLogger()
    log.debug('Browsing versions for %s ' % pname)

    p = getCachedPackageConf(pname)

    if p :
        PROJECT = p.project().upper()
        datapackage = True
    else :
        if pname in lcg_tar :
            PROJECT = pname
        else :
            PROJECT = pname.upper()

    if pname in lcg_tar :
        webpage = urlopen(url_dist + 'source')
    else :
        webpage = urlopen(url_dist + PROJECT)

    weblines = webpage.readlines()
    plist = []
    for webline in weblines:
        if webline.find('href="' + PROJECT) != -1:
            href = webline.index('href=')
            quote1 = webline[href:].index('"')
            quote2 = webline[href + quote1 + 1:].index('"')
            filename = webline[href + quote1 + 1:href + quote1 + 1 + quote2]
            if filename.find(".md5") == -1 :
                if datapackage and filename.find(pname) == -1:
                    continue
                elif ver :
                    if filename.find(ver + ".") != -1 :
                        plist.append(filename)
                else :
                    plist.append(filename)
    plist.sort()

    atexit.register(urlcleanup)

    if pname not in lcg_tar :
        plist = sortStrings(plist, safe=True)

    return plist

def listVersions(pname, ver=None):
    log = logging.getLogger()
    for l in getVersionList(pname, ver) :
        log.info(l)

def doesVersionExist(vlist, pname, version, cmt_config=None):

    exist = False

    isproj = False

    p = getCachedPackageConf(pname)

    if not p :
        p = getCachedProjectConf(pname)
        if p :
            isproj = True

    for l in vlist :
        if not cmt_config :
            if p.tarBallName(version, full=True) == l :
                exist = True
                break
        else :
            if isproj and p.tarBallName(version, cmt_config, full=True) == l :
                exist = True
                break
    return exist

def getProjectVersions(pname, cmt_config=None):
    from LbConfiguration.Package import package_names
    from LbConfiguration.Project import project_names
    from LbConfiguration.Version import extractVersion

    isproj = False
    ispack = False

    version_list = []

    if pname.upper() in [x.upper() for x in package_names] :
        ispack = True
    elif pname.upper() in [x.upper() for x in project_names] :
        isproj = True


    vlist = getVersionList(pname)
    for l in vlist :
        toappend = False
        v = extractVersion(l).name()
        if ispack and doesVersionExist(vlist, pname, v):
            toappend = True
        if isproj :
            if not cmt_config and doesVersionExist(vlist, pname, v):
                toappend = True
            elif cmt_config :
                if doesVersionExist(vlist, pname, v) and doesVersionExist(vlist, pname, v, cmt_config) :
                    toappend = True
        if toappend :
            if v not in version_list :
                version_list.append(v)

    return version_list

def getLatestVersion(pname, cmt_config=None, noerror=False):
    try:
        vlist = getProjectVersions(pname, cmt_config)
        return vlist[-1]
    except IndexError, e:
        # No version was found...
        if noerror:
            return None
        else:
            raise e
        

#
#  read a string from a file ==============================================
#
def readString(filename, string):
    fd = open(filename)
    fdlines = fd.readlines()
    for fdline in fdlines:
        if fdline.find(string) != -1:
            return fdline


#
#  remove a project,version from the install area ==========================
#
def removeProject(project, pvers):
    log = logging.getLogger()
    log.info('%s %s ' % (project, pvers))

    this_lhcb_dir = subdir_dict["lhcb"].split(os.pathsep)[0]
    this_html_dir = subdir_dict["html"].split(os.pathsep)[0]
    this_targz_dir = subdir_dict["targz"].split(os.pathsep)[0]
    this_lcg_dir = subdir_dict["lcg"].split(os.pathsep)[0]
    this_log_dir = subdir_dict["log"].split(os.pathsep)[0]
    this_contrib_dir = subdir_dict["contrib"].split(os.pathsep)[0]

    import LbConfiguration.Project

    PROJECT = project.upper()

    if PROJECT in [ p.NAME() for p in LbConfiguration.Project.project_list ] :
        head = this_lhcb_dir
        VERSION = PROJECT + '_' + pvers

        flist = os.listdir(this_html_dir)
        for filen in flist:
            if filen.find(VERSION + '.') != -1 or filen.find(VERSION + '_') != -1 :
                os.remove(os.path.join(this_html_dir, filen))
                log.info('remove %s' % os.path.join(this_html_dir, filen))

        flist = os.listdir(this_targz_dir)
        for filen in flist:
            if filen.find(VERSION + '.') != -1 or filen.find(VERSION + '_') != -1:
                os.remove(os.path.join(this_targz_dir, filen))
                log.info('remove %s' % os.path.join(this_targz_dir, filen))

        flist = os.listdir(this_log_dir)
        for filen in flist:
            if filen.find(VERSION + '.') != -1 or filen.find(VERSION + '_') != -1:
                os.remove(os.path.join(this_log_dir, filen))
                log.info('remove %s' % os.path.join(this_log_dir, filen))

        if os.path.isdir(os.path.join(head, PROJECT, VERSION)):
            shutil.rmtree(os.path.join(head, PROJECT, VERSION))
            log.info('remove %s' % os.path.join(head, PROJECT, VERSION))
        if os.path.isdir(os.path.join(head, project + 'Env', pvers)):
            shutil.rmtree(os.path.join(head, project + 'Env', pvers))
            log.info('remove %s' % os.path.join(head, project + 'Env', pvers))
    else:
        proj_vers = project + '_' + pvers
        flist = os.listdir(this_html_dir)
        for filen in flist:
            if filen.find(proj_vers) != -1 :
                os.remove(os.path.join(this_html_dir, filen))
                log.info('remove %s' % os.path.join(this_html_dir, filen))

        flist = os.listdir(this_targz_dir)
        for filen in flist:
            if filen.find(proj_vers + '.') != -1 or filen.find(proj_vers + '_') != -1:
                os.remove(os.path.join(this_targz_dir, filen))
                log.info('remove %s \n' % os.path.join(this_targz_dir, filen))

        flist = os.listdir(this_log_dir)
        for filen in flist:
            if filen.find(proj_vers + '.') != -1 or filen.find(proj_vers + '_') != -1:
                os.remove(os.path.join(this_log_dir, filen))
                log.info('remove %s' % os.path.join(this_log_dir, filen))

        head = this_contrib_dir
        if os.path.isdir(os.path.join(head, project, pvers)):
            shutil.rmtree(os.path.join(head, project, pvers))
            log.info('remove %s' % os.path.join(head, project, pvers))
        else:
            head = this_lcg_dir
            vers = pvers
            if project == 'LCGCMT' or project == 'GENSER': vers = proj_vers
            if os.path.isdir(os.path.join(head, project, vers)):
                shutil.rmtree(os.path.join(head, project, vers))
                log.info('remove %s' % os.path.join(head, project, vers))

    if multiple_mysiteroot and os.path.isdir(os.path.join(this_lhcb_dir, 'EXTRAPACKAGES')):
        head = os.path.join(this_lhcb_dir, 'EXTRAPACKAGES')
        if project.find('Field') != -1:
            head = os.path.join(this_lhcb_dir, 'EXTRAPACKAGES')
        if project.find('DDDB') != -1:
            head = os.path.join(this_lhcb_dir, 'EXTRAPACKAGES', 'Det')
        if project.find('Dec') != -1:
            head = os.path.join(this_lhcb_dir, 'EXTRAPACKAGES', 'Gen')
        if os.path.isdir(os.path.join(head, project, pvers)):
            shutil.rmtree(os.path.join(head, project, pvers))
            log.info('remove %s' % os.path.join(head, project, pvers))

    head = os.path.join(this_lhcb_dir, 'PARAM')
    if project.find('Field') != -1 or project.find("AppConfig") != -1:
        head = os.path.join(this_lhcb_dir, 'DBASE')
    if project.find('DDDB') != -1:
        head = os.path.join(this_lhcb_dir, 'DBASE', 'Det')
    if project.find('Dec') != -1:
        head = os.path.join(this_lhcb_dir, 'DBASE', 'Gen')
    if project.find('L0TCK') != -1 :
        head = os.path.join(this_lhcb_dir, 'DBASE', "TCK")
    if project.find('Emacs') != -1 :
        head = os.path.join(this_lhcb_dir, 'TOOLS', "Tools")
    if os.path.isdir(os.path.join(head, project, pvers)):
        shutil.rmtree(os.path.join(head, project, pvers))
        log.info('remove %s' % os.path.join(head, project, pvers))

# -------------------------------------------------------------------------------------------
# Generate SetupScript
def genSetupScript(pname, pversion, cmtconfig, scriptfile):
    log = logging.getLogger()
    this_lhcb_dir = subdir_dict["lhcb"].split(os.pathsep)[0]
    targetshells = ["sh", "csh", "bat"]
    usedshell = "csh"
    for t in targetshells :
        if scriptfile.endswith(".%s" % t) :
            usedshell = t
            break
    os.environ["CMTCONFIG"] = cmtconfig
    # setup the scripts python path
    lbscriptspydir = os.path.join(this_lhcb_dir, "LBSCRIPTS", "LBSCRIPTS_" + lbscripts_version, "InstallArea", "python")
    if os.path.exists(lbscriptspydir) :
        sys.path.append(lbscriptspydir)
    else :
        log.error("%s doesn't exist" % lbscriptspydir)
    # setup the base LHCb environment
    lbloginscript = os.path.join(lbscriptspydir, "LbConfiguration", "LbLogin.py")
    log.debug("Using LbLogin from %s" % lbloginscript)
    if os.path.exists(lbloginscript) :
        log_level = log.getEffectiveLevel()
        from LbConfiguration.LbLogin import getLbLoginEnv
        llsargs = []
        if debug_flag :
            llsargs.append("--debug")
        llsargs.append("--shell=%s" % usedshell)
        llsargs.append("--mysiteroot=%s" % os.environ["MYSITEROOT"])
        llsargs.append("--scripts-version=%s" % lbscripts_version)
        llsargs.append("--cmtconfig=%s" % cmtconfig)
        log.debug("Running LbLogin %s" % " ".join(llsargs))
        env = getLbLoginEnv(llsargs)
        for var in env.keys() :
            os.environ[var] = env[var]
        log.setLevel(log_level)
    else :
        log.error("%s doesn't exist" % lbloginscript)

    # run genProjectSetup and create the setup script
    genprojsetup = os.path.join(lbscriptspydir, "LbConfiguration", "Tools.py")
    log.debug("Using genProjectSetup from %s" % genprojsetup)
    if os.path.exists(genprojsetup) :
        from LbConfiguration.Tools import genProjectSetup
        extra_args = ["--no-user-area"]
        if debug_flag :
            extra_args.append("--debug")
        genProjectSetup(scriptfile, pname, pversion, cmtconfig,
                        usedshell, extra_args)
    else :
        log.error("%s doesn't exist" % genprojsetup)

# -------------------------------------------------------------------------------------------
#
#  create base directories #################################################
#

def _multiPathJoin(path, subdir):
    pathlist = []
    for d in path.split(os.pathsep) :
        pathlist.append(os.path.join(d, subdir))
    return os.pathsep.join(pathlist)

def _multiPathGetFirst(path, subdir, default=None):
    """ Quick way to find a file or dir in a series of paths """
    result = default
    for d in path.split(os.pathsep) :
        sd = os.path.join(d, subdir)
        if os.path.exists(sd) :
            result = sd
            break
    return result

def _cleanPath(path_value, normalize=False):
    """fully clean up a path: remove empty entries and
    also remove trailing os.sep for each entry
    @param path_value: initial pathname. Can be a single path or a collection separated with os.pathsep
    @param normalized: if True remove .., removes extra '/' and convert '/' to '\\' on windows
    """
    lst = []
    for p in path_value.split(os.pathsep) :
        if p :
            p = p.rstrip(os.sep)
            if normalize :
                p = os.path.normpath(p)
            lst.append(p)
    return os.pathsep.join(lst)


def createBaseDirs(pname, pversion):
    global cmtconfig
    global subdir_dict
    global cur_tmp_dir


    log = logging.getLogger()

    mysiteroot = os.environ["MYSITEROOT"]


    mypath = os.path.realpath(mysiteroot.split(os.pathsep)[0])
    thispwd = os.path.realpath(os.getcwd())
    if sys.platform == 'win32' :
        if mypath.upper() != thispwd.upper() :
            log.warning("Using the directory %s for installation" % mypath)
            os.chdir(mypath)
    else:
        if mypath != thispwd :
            log.warning("Using the directory %s for installation" % mypath)
            os.chdir(mypath)

    for d in ["log", "contrib", "lhcb", "html", "etc", "bootscripts", "targz", "conf"] :
        subdir_dict[d] = _multiPathJoin(mysiteroot, d)

    subdir_dict["lcg"] = _multiPathJoin(mysiteroot, os.path.join("lcg", "external"))

    if sys.platform != "win32" and "USER" in os.environ :
        subdir_dict["tmp"] = _multiPathJoin(mysiteroot, os.path.join("tmp", os.environ["USER"]))
    else :
        subdir_dict["tmp"] = _multiPathJoin(mysiteroot, "tmp")

    if not cur_tmp_dir :
        cur_tmp_dir = subdir_dict["tmp"].split(os.pathsep)[0]

    logname = None

    if pversion :
        if logname :
            logname = os.path.join(subdir_dict["log"].split(os.pathsep)[0], pname + '_' + pversion + '.log')
        else :
            logname = os.path.join(subdir_dict["log"].split(os.pathsep)[0], pname + '.log')
    if not check_only :
        done = createDir(mypath.split(os.pathsep)[0], logname)
    else :
        done = False
    if not done :
        logname = None

    return logname

#
#  install one project #################################################
#
def runInstall(pname, pversion, binary=None):
    global cmtconfig
    log = logging.getLogger()

# print action list
    log.info("%s %s %s" % (pname, pversion, binary))
    log.info('cmt version = %s, make_flag= %s, full_flag= %s, list_flag= %s, remove_flag= %s ' % (cmtversion, make_flag, full_flag, list_flag, remove_flag))


# cleanup the cache at exit
    urlcleanup()
    atexit.register(urlcleanup)


# start the project installation
    global get_boot_scripts_flag
    if get_boot_scripts_flag:
        getBootScripts()
    else:
        log.warning("Boot scripts installation disabled")
        # Need to add LbScripts to the PYTHONPATH...
        scripttar = "LBSCRIPTS_LBSCRIPTS_%s.tar.gz" % lbscripts_version
        location = getInstallLocation(scripttar)
        log.debug("LbScripts %s is already installed in %s" % (lbscripts_version, location))
        for l in subdir_dict["lhcb"].split(os.pathsep) :
            if l.startswith(location) :
                that_lhcb_dir = l
                break
            sys.path.insert(0, os.path.join(that_lhcb_dir,
                                            "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version,
                                            "InstallArea", "python"))

# update main lhcb project configuration
# This has to be removed as many grid jobs do not have write permissions
    #updateConf()

# if list_flag is set: give the list of available versions for this project
    if list_flag :
        listVersions(pname)
        sys.exit()

    if show_compatible_configs :
        showCompatibleConfigs()
        sys.exit()


# if remove flag is set then correspondind tar files and directories will
# be removed
    if remove_flag :
        if pversion :
            removeProject(pname, pversion)
        else :
            log.error("A version of %s must be specified for the removal" % pname)
        sys.exit()

# check binary name
    cmtconfig = checkBinaryName(binary)
    if install_binary :
        binary = cmtconfig

    if get_boot_scripts_flag:
        getCMT(cmtversion)
    else:
        log.warning("CMT install disabled as boot scripts installation disabled")

    if not pversion:
        if binary:
            pversion = getLatestVersion(pname, binary)
        else :
            pversion = getLatestVersion(pname)

    from LbConfiguration.Platform import isBinaryDbg, getBinaryOpt, getBinaryDbg


    if not check_only :
        if not get_boot_scripts_flag:
            log.warning("LbScripts install disabled as boot scripts installation disabled")        
        else:
            if pname != 'LbScripts' :
                script_project_list = getProjectList('LbScripts', lbscripts_version)[0]
                getProjectTar(script_project_list)

    project_list, html_list = getProjectList(pname, pversion)


    cmtconfig_opt = cmtconfig
    if isBinaryDbg(cmtconfig) :
        cmtconfig_opt = getBinaryOpt(cmtconfig)

    if not check_only :
        if pname != 'LHCbGrid' and sys.platform != "win32" and cmtconfig.find("slc3") != -1 and cmtconfig.find("sl3") != -1 :
            grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version)
            project_list.update(grid_project_list)
            html_list += grid_html_list
            grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version, cmtconfig_opt)
            project_list.update(grid_project_list)
            html_list += grid_html_list
        global get_compat_flag
        if not get_compat_flag:
            log.warning("Compat installation disabled")
        if pname != 'Compat' and get_compat_flag:
            if not compat_version:
                new_compat_version = getLatestVersion("Compat", cmtconfig_opt, True)
            else :
                new_compat_version = compat_version
            compat_project_list, compat_html_list = getProjectList('Compat', new_compat_version)
            project_list.update(compat_project_list)
            html_list += compat_html_list
            compat_project_list, compat_html_list = getProjectList('Compat', new_compat_version, cmtconfig_opt)
            project_list.update(compat_project_list)
            html_list += compat_html_list


    if binary :
        if pname in lcg_tar :
            binary = cmtconfig_opt
        binary_project_list, binary_html_list = getProjectList(pname, pversion, binary)
        project_list.update(binary_project_list)
        html_list += binary_html_list


    if full_flag :
        log.info('download debug version and reconfigure it')
        binary_dbg = getBinaryDbg(binary)
        if isBinaryDbg(binary) :
            binary_dbg = binary
        full_project_list, full_html_list = getProjectList(pname, pversion, binary_dbg)
        project_list.update(full_project_list)
        html_list += full_html_list

    obsolete_pak = cmtconfig_opt + ".tar.gz"
    if obsolete_pak in project_list :
        del project_list[obsolete_pak]
    if obsolete_pak in html_list :
        html_list.remove(obsolete_pak)

    log.debug('global project_list %s' % project_list)
    log.debug('global html_list %s' % html_list)

    if check_only :
        checkInstalledProjects(project_list, html_list[0])



    getProjectTar(project_list)

    if setup_script :
        os.chdir(os.environ["MYSITEROOT"].split(os.pathsep)[0])
        genSetupScript(pname, pversion, cmtconfig, setup_script)



#
#  unpack tar.gz file =====================================================
#

def tarFileList(filename):
    log = logging.getLogger()
    if not useTarFileModule() :
        lststr = 'tar --list --ungzip --file %s' % filename
        for l in os.popen(lststr) :
            yield l[:-1]
    else :
        import tarfile
        if tarfile.is_tarfile(filename):
            tar = tarfile.open(filename, 'r:gz')
            for l in tar.getnames() :
                yield l
        else:
            log.warning('tar_file: %s is not a tar file' % filename)


def cleanTarFileTarget(filename, targetlocation):
    for f in tarFileList(filename) :
        fp = os.path.join(targetlocation, f)
        if os.path.isfile(fp) :
            os.remove(fp)

def _basename(path):
    # A basename() variant which first strips the trailing slash, if present.
    # Thus we always get the last component of the path, even for directories.
    return os.path.basename(path.rstrip(os.path.sep))

def _destinsrc(src, dst):
    src = os.path.abspath(src)
    dst = os.path.abspath(dst)
    if not src.endswith(os.path.sep):
        src += os.path.sep
    if not dst.endswith(os.path.sep):
        dst += os.path.sep
    return dst.startswith(src)

def safeMove(src, dst):
    """
    copy of the shutil.move function that moves properly links across filesystems. It
    also fix windows attributes from tarball expansion.
    @param src: source
    @param dst: destination
    """

    real_dst = dst
    if os.path.isdir(dst):
        real_dst = os.path.join(dst, _basename(src))
        if os.path.exists(real_dst):
            raise shutil.Error, "Destination path '%s' already exists" % real_dst
    try:
        os.rename(src, real_dst)
    except OSError:
        if os.path.isdir(src):
            if _destinsrc(src, dst):
                raise shutil.Error, "Cannot move a directory '%s' into itself '%s'." % (src, dst)
            shutil.copytree(src, real_dst, symlinks=True)
            shutil.rmtree(src)
        elif os.path.islink(src) :
            # treats correctly symlinks across filesystems
            linkto = os.readlink(src)
            os.symlink(linkto, real_dst)
            os.unlink(src)
        else:
            shutil.copy2(src, real_dst)
            os.unlink(src)

    # fix windows attributes
    if os.path.isdir(dst) and sys.platform.startswith("win32") :
        fixWinAttrib(dst)

def safeMakeDirs(path):
    log = logging.getLogger()
    if not os.path.exists(path) :
        if os.path.islink(path) :
            os.unlink(path)
            log.debug("Removed broken link %s" % path)
        os.makedirs(path)
        log.debug("Created directory %s" % path)

def addSoft(srcdir, dstdir, overwrite=False):
    log = logging.getLogger()
    if overwrite :
        log.debug("Overwriting %s with %s" % (dstdir, srcdir))
    else :
        log.debug("Add %s to %s" % (dstdir, srcdir))
    safeMakeDirs(dstdir)
    for root, dirs, files in os.walk(srcdir, topdown=True) :
        dirstoremove = []
        for d in dirs :
            src = os.path.join(root, d)
            dst = src.replace(srcdir + os.sep, "")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                safeMakeDirs(pdst)
                safeMove(src, dst)
                dirstoremove.append(d)
        for f in files :
            src = os.path.join(root, f)
            dst = src.replace(srcdir + os.sep, "")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                safeMakeDirs(pdst)
                safeMove(src, dst)
            elif overwrite :
                if os.path.exists(dst) :
                    os.remove(dst)
                pdst = os.path.dirname(dst)
                safeMakeDirs(pdst)
                safeMove(src, dst)
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

def untarFile(fname, output_path=""):
    log = logging.getLogger()

    retcode = 0

    log.info("untarring %s in %s" % (fname, os.getcwd()))

    this_targz_dir = subdir_dict["targz"].split(os.pathsep)[0]

    filename = os.path.join(this_targz_dir, fname)
    md5filename = getMD5FileName(filename)
    htmlfilename = os.path.join(subdir_dict["html"], getHTMLFileName(fname))
    if not os.path.isfile(filename):
        log.warning('%s does not exist' % filename)
        retcode = 1
        return retcode

    if useTarFileModule() :

        import tarfile
        if tarfile.is_tarfile(filename):
            tar = tarfile.open(filename, 'r:gz')
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
        i = 0
        j = 0
        try:
            for tarinfo in tar:
                i = i + 1
                try:
                    if not os.path.exists(tarinfo.name):
                        j = j + 1
                        if sys.platform == 'win32'and tarinfo.issym():
                            log.info('%s %s %s' % (j, tarinfo.name, tarinfo.issym()))
                        else:
                            tar.extract(tarinfo, path=output_path)
                    else:
                        if fname.find('script') != -1:
                            j = j + 1
                            tar.extract(tarinfo, path=output_path)
                except tarfile.ExtractError:
                    os.remove(filename)
                    log.warning('extract error %s %s ' % (filename, tarinfo.name))
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
                log.info('%s removed' % htmlfilename)
            retcode = 1
            return retcode

        tar.close()
        log.info('%s - %s entries, untar %s entries ' % (fname, i, j))

    else:
        if sys.platform == 'win32':
            os.remove(filename)
            log.warning('your python version %s is not able to untar a file - try to get a version >= 2.3.4 ' % python_version)
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
            if output_path :
                strcmd += " --directory=%s " % output_path
            tar_output = None
            try:
                for l in tarFileList(filename) :
                    if os.path.isfile(l) :
                        os.remove(l)
                status, tar_output = commands.getstatusoutput(strcmd)
            except:
                os.remove(filename)
                log.warning('exception in: %s' % strcmd)
                log.warning('%s removed' % filename)
                log.warning('tar command output:')
                if tar_output :
                    for l in tar_output.split("\n") :
                        log.warning(l)
                if os.path.exists(md5filename):
                    os.remove(md5filename)
                    log.info('%s removed' % md5filename)
                if os.path.exists(htmlfilename):
                    os.remove(htmlfilename)
                    log.info('%s removed' % htmlfilename)
                retcode = 1
                return retcode
            else:
                if status != 0:
                    log.warning('error in: %s' % strcmd)
                    log.warning('return code %s' % status)
                    log.warning('tar command output:')
                    for l in tar_output.split("\n") :
                        log.warning(l)
                    if os.path.exists(filename):
                        os.remove(filename)
                        log.info('%s removed' % (filename))
                    if os.path.exists(md5filename):
                        os.remove(md5filename)
                        log.info('%s removed' % (md5filename))
                    if os.path.exists(htmlfilename):
                        os.remove(htmlfilename)
                        log.info('%s removed' % (htmlfilename))
                    retcode = 1
                    return retcode


    # replace the tar.gz file with an empty one to save space

    os.remove(filename)

    return retcode


def checkBinaryName(binary):
    global make_flag
    global extra_binary

    from LbConfiguration.Platform import NativeMachine, isBinaryOpt, getBinaryDbg, binary_list

    log = logging.getLogger()

    if not binary :
        log.warning("No CMTCONFIG has been provided on the command line")
        if os.environ.has_key("CMTCONFIG") :
            binary = os.environ["CMTCONFIG"]
            log.warning("Extracting CMTCONFIG from the environment: %s" % binary)
        else :
            m = NativeMachine()
            plist = m.CMTSupportedConfig(debug=True)
            if plist :
                binary = plist[0]
            else :
                binary = m.CMTNativeConfig(debug=False)
            log.warning("No CMTCONFIG in the environment")
            log.warning("Guessed CMTCONFIG is %s" % binary)


    os.environ['CMTCONFIG'] = binary
    os.environ['CMTDEB'] = binary
    if isBinaryOpt(binary) :
        os.environ['CMTDEB'] = getBinaryDbg(binary)
    # if a win32 binary is installed from a non win32 platform then do not cmt config
    if sys.platform != 'win32' and binary.find('win32') != -1 :
        make_flag = None

    # Disabling this check as it is more harmful than useful...
    if binary not in binary_list:
    #    print 'BE CAREFUL - your CMTCONFIG %s is not part of the lhcb_binary %s' % (os.getenv('CMTCONFIG'), LbConfiguration.Platform.binary_list)
    #    print 'do you want to continue? [yes|no]'
    #    nextin = sys.stdin.readline()
    #    if nextin.lower()[0] != 'y':
    #        sys.exit()
        extra_binary = binary

    return binary

def parseArgs():
    global debug_flag, full_flag, list_flag, remove_flag
    global cmtversion, md5_check, grid_version, nb_retries
    global setup_script, check_only, overwrite_mode
    global _retry_time, fix_perm
    global show_compatible_configs
    global install_binary
    global compat_version
    global latest_data_link
    global dev_install
    global url_dist
    global boot_script_loc
    global cur_tmp_dir
    global check_self
    global get_boot_scripts_flag
    global get_compat_flag

    pname = None
    pversion = None
    binary = None

    arguments = sys.argv[1:]

    if not arguments :
        print "No enough argument passed"
        usage()
        sys.exit(2)
    try:
        opts, args = getopt.getopt(arguments, "hdflrbp:v:c:ng:s:Cu:B:t:",
            ['help', 'debug', 'full', 'list', 'remove', 'binary=',
             'project=', 'cmtversion=', 'nocheck',
             'retry=', 'grid=', 'setup-script=', 'check', 'overwrite',
             'compatversion=', "retrytime=", "nofixperm", "version",
             "compatible-configs", "latest-data-link", "url",
             "boot-scripts=","tmp-dir=", "dev-install", "no-self-check",
             "no-boot-scripts", "no-compat"])

    except getopt.GetoptError, err:
        print str(err)
        usage()
        sys.exit(2)

    for key, value in opts:
        if key == '--version':
            print script_version
            sys.exit()
        if key in ('-d', '--debug'):
            debug_flag = True
        if key in ('-C', '--compatible-configs'):
            show_compatible_configs = True
        if key in ('-f', '--full'):
            full_flag = True
        if key in ('-h', '--help'):
            usage()
        if key in ('-l', '--list'):
            list_flag = True
        if key in ('-r', '--remove'):
            remove_flag = True
        if key in ('-c', '--cmtversion'):
            cmtversion = value
        if key == '--compatversion':
            compat_version = value
        if key == '--latest-data-link' :
            latest_data_link = True
        if key in ('-v'):
            pversion = value
            print "Obsolete option. Please use the new syntax."
        if key in ('-p', '--project'):
            pname = value
            print "Obsolete option. Please use the new syntax."
        if key == '-b':
            binary = os.environ.get('CMTCONFIG', None)
            install_binary = True
        if key == '--binary':
            binary = value
            os.environ["CMTCONFIG"] = binary
            install_binary = True
        if key in ('-n', '--nocheck'):
            md5_check = False
        if key in ('-g', '--grid'):
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
        if key == "--dev-install" :
            dev_install = True
        if key in ('-u', '--url'):
            url_dist = value
        if key in ('-B', '--boot-scripts'):
            boot_script_loc = value
        if key == '--no-self-check':
            check_self = False
        if key == '--no-boot-scripts':
            get_boot_scripts_flag = False
        if key == '--no-compat':
            get_compat_flag = False
        if key in ('-t', '--tmp-dir'):
            try :
                cur_tmp_dir = mkdtemp(dir=value)
                atexit.register(destroyTmpDirectory)
            except:
                cur_tmp_dir = value

    if not url_dist.endswith("/") :
        url_dist += "/"

    if not pname and len(args) > 0 :
        pname = args[0]
    if pname and pname.find("/") != -1 :
        plist = pname.split("/")
        pname = "/".join(plist[1:])

    if not pversion and len(args) > 1 :
        pversion = args[1]

    return pname, pversion, binary


def checkMySiteRoot():
    global multiple_mysiteroot
    log = logging.getLogger()

    if not os.environ.has_key("MYSITEROOT") :
        log.warning("The env variable MYSITEROOT is not set")
        if os.environ.has_key("VO_LHCB_SW_DIR") :
            os.environ["VO_LHCB_SW_DIR"] = _cleanPath(os.environ["VO_LHCB_SW_DIR"])
            fallback_mysiteroot = os.path.join(os.environ["VO_LHCB_SW_DIR"], "lib")
            if os.path.exists(fallback_mysiteroot) :
                log.warning("Using $VO_LHCB_SW_DIR/lib for MYSTITEROOT")
                os.environ["MYSITEROOT"] = fallback_mysiteroot
    else :
        # cleanup the MYSITEROOT variable. It can be a path
        os.environ["MYSITEROOT"] = _cleanPath(os.environ["MYSITEROOT"])

    if not os.environ.has_key("MYSITEROOT") :
        log.fatal('please set MYSITEROOT to $INSTALLDIR:$MYSITEROOT before running the python script \n')
        sys.exit(1)
    else :
        log.debug("MYSITEROOT is set to %s" % os.environ["MYSITEROOT"])

    if len(os.environ["MYSITEROOT"].split(os.pathsep)) > 1 :
        multiple_mysiteroot = True
    else :
        multiple_mysiteroot = False


    if os.environ.has_key("VO_LHCB_SW_DIR") :
        os.environ["VO_LHCB_SW_DIR"] = _cleanPath(os.environ["VO_LHCB_SW_DIR"])
        log.debug("The VO_LHCB_SW_DIR variable is set to %s" % os.environ["VO_LHCB_SW_DIR"])
        test_vo_dir = os.environ["VO_LHCB_SW_DIR"]
        test_mysiteroot = os.environ["MYSITEROOT"]
        if not multiple_mysiteroot :
            if os.path.join(test_vo_dir, "lib") == test_mysiteroot :
                log.debug("VO_LHCB_SW_DIR is already correctly defined to %s (${MYSITEROOT}==${VO_LHCB_SW_DIR}/lib) " % test_vo_dir)
            else :
                log.warning("VO_LHCB_SW_DIR isn't correctly defined (${MYSITEROOT}!=${VO_LHCB_SW_DIR}/lib) ")
                del os.environ["VO_LHCB_SW_DIR"]
                log.warning("The VO_LHCB_SW_DIR variable has been removed from the environment")

    if not os.environ.has_key("VO_LHCB_SW_DIR") and not multiple_mysiteroot:
        # only tries to set VO_LHCB_SW_DIR if MYSITEROOT has one entry
        test_mysiteroot = os.environ["MYSITEROOT"]
        mysite_parent = os.path.dirname(test_mysiteroot)
        if os.path.basename(test_mysiteroot) == "lib" :
            log.warning("Setting VO_LHCB_SW_DIR to %s" % mysite_parent)
            os.environ["VO_LHCB_SW_DIR"] = mysite_parent
        else :
            log.debug("Impossible to set VO_LHCB_SW_DIR")

def tarContainsTopDir(tarName):
    """ Util function to see  if a tar file contains the release directory """
    log = logging.getLogger()
    import tarfile
    retval = False
    try:
        tar = tarfile.open(tarName, "r")
        m = tar.next()
        if m != None:
            topDirs = [ "external", "app", "releases" ]
            for d in topDirs:
                if m.name.startswith(d):
                    log.debug("Tarfile name %s starts with %s" % (m.name, d))
                    retval = True
                    break
        tar.close()
    except:
        pass
    log.debug("Checkin if tar %s has topdir: %s" % (tarName, retval))
    return retval
    

#---------------------------------------------------------------------
def main():

# get arguments
    pname, pversion, binary = parseArgs()

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

    start_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())

    thelog.info('=' * line_size)
    thelog.info(('<<< %s - Start of install_project.py %s with python %s >>>' % (start_time, script_version, txt_python_version)).center(line_size))
    thelog.info('=' * line_size)
    thelog.debug(("Using LbScripts %s" % lbscripts_version).center(line_size))
    thelog.debug('-' * line_size)
    thelog.debug("Command line arguments: %s" % " ".join(sys.argv))


    checkMySiteRoot()


    if not check_only and fix_perm:
        changePermissions('install_project.py', recursive=False)

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

    end_time = time.strftime("%a, %d %b %Y %H:%M:%S", time.localtime())
    thelog.info('=' * line_size)
    thelog.info(('<<< %s - End of install_project.py %s >>>' % (end_time, script_version)).center(line_size))
    thelog.info('=' * line_size)


if __name__ == "__main__":
    main()
