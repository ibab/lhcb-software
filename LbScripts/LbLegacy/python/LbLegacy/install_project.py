#!/usr/bin/env python
"""
  Script to install a project where you are
  Please have a look at the documentation in the release.notes
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

script_version = '100210'
python_version = sys.version_info[:3]
txt_python_version = ".".join([str(k) for k in python_version])
lbscripts_version = "v5r0"
compat_version = None
#-----------------------------------------------------------------------------------

# url from which to get files
url_dist = 'http://lhcbproject.web.cern.ch/lhcbproject/dist/'

# list of subdirectories created in runInstall
site_subdirs = ['lcg', 'lhcb', 'contrib', 'html', 'targz', 'tmp']

# dynamic modules

LbLegacy = None
LbConfiguration = None


# base directories
log_dir = None
contrib_dir = None
lcg_dir = None
lhcb_dir = None
html_dir = None
bootscripts_dir = None
targz_dir = None
tmp_dir = None

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
      -p                 : name of the project to install (obsolete option)
      -v                 : version of the project to install (obsolete option)
      --version          : the version of this very script
      -d or --debug      : to print more info
      -l or --list       : to list the <project>_<version>_*.tar.gz files available on the web
      -r or --remove     : remove the <project>/<version>
      -c or --cmtversion : download this CMT version
      -h or --help       : to print this help
      -b                 : to import $CMTCONFIG binaries
      --binary=<bin>     : to import another binary (i.e. $CMTCONFIG_dbg on Linux)
                           this triggers a 'cmt broadcast cmt config' of all projects but LCGCMT
      -f                 : to import source, $CMTCONFIG binaries, $CMTCONFIG_dbg binaries
      -n or -nocheck     : no md5 check of the tar balls
      --retry=<nb>       : nb of retries for the download (default=1)
      -C                 : show compatible CMTCONFIGs for that platform

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
    debugmsglist.append("File size %d" % _file_size )
    log.debug("\t".join(debugmsglist))
    _block_size = 0
    _block_count = 0
    _file_size = 0

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
    cmdlist.append((command, dirname))
    _postinstall_commands[project] = cmdlist
    if dirname :
        log.debug("Registered PostInstall for %s: \"%s\" in %s" % (project, command, dirname))
    else :
        log.debug("Registered PostInstall for %s: \"%s\"" % (project, command))


def callPostInstallCommand(project):
    log = logging.getLogger()
    projcmds = _postinstall_commands.get(project, None)
    if projcmds :
        for c in projcmds :
            if c[1] :
                os.chdir(c[1])
            os.system("%s" % c[0])
            log.info("Executing PostInstall for %s: \"%s\" in %s" % (project, c[0], c[1]))
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
            here = os.getcwd()
            os.chdir(dirpath)
            os.system("attrib -R -A -H /S /D")
            os.chdir(here)
    else :
        os.system("attrib -R -A -H %s" % dirpath)
    

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
                    fixWinAttrib(os.path.join(root,d))
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
    return tmp_dir.split(os.pathsep)[0]

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
    log.info('     Removing %s' % getTmpDirectory())
    removeAll(getTmpDirectory())

def cleanTmpDirectory():
    log = logging.getLogger()
    log.info('     Cleaning up %s' % getTmpDirectory())
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
def createDir(here , logname):
    log = logging.getLogger()

    log.info('create necessary sub-directories')

    this_log_dir = log_dir.split(os.pathsep)[0]

    good = False
    if checkWriteAccess(here) :
        good = True
        if not os.path.isdir(this_log_dir):
            os.mkdir(this_log_dir)
        elif logname:
            if os.path.exists(os.path.join(this_log_dir, logname + '_old')):
                os.remove(logname + '_old')
            if os.path.exists(logname):
                os.rename(logname, logname + '_old')

        for dirnm in site_subdirs:
            if os.path.isdir(os.path.join(here, dirnm)):
                log.debug('%s exists in %s ' % (dirnm, here))
            else:
                os.mkdir(dirnm)
                log.info('%s is created in %s ' % (dirnm, here))
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
                            os.mkdir(os.path.join(dirnm, 'PARAM'))
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
    else :
        log.warning("Cannot write in %s" % here)
        good = False

    return good

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
            rc = os.system('call INSTALL.bat')
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
        for cd in contrib_dir.split(os.pathsep) :
            if cd.startswith(location) :
                that_contrib_dir = cd
                break
        os.environ['CMTROOT'] = os.path.join(that_contrib_dir, 'CMT', cmtvers)

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
    if not url.endswith("/") :
        url = url + "/"
    log.debug('%s%s ' % (url, fname))

    if fname.find('.tar.gz') != -1:
        filetype = 'x-gzip'
        this_targz_dir = targz_dir.split(os.pathsep)[0]
        dest = os.path.join(this_targz_dir, fname)
        if url.find('scripts') != -1 or url.find('system') != -1:
            if os.path.exists(dest) :
                try:
                    os.remove(dest)
                except:
                    log.info('can not remove fname %s' % dest)
    elif fname.find('.html') != -1 or fname.find('.htm') != -1 :
        this_html_dir = html_dir.split(os.pathsep)[0]
        dest = os.path.join(this_html_dir, fname)
        filetype = 'html'

    else:
        filetype = 'text'
        if fname.find('.py') != -1:
            dest = fname
            if fname == 'install_project.py' : dest = 'latest_install_project.py'
            if os.path.exists(dest):
                try:
                    os.remove(dest)
                except:
                    log.warning('can not remove fname %s' % dest)


    exist_flag = False
    log.debug('%s on %s' % (fname, dest))

    if not os.path.exists(dest):
        local_retries = nb_retries + 1
        hasbeendownloaded = False
        while (not hasbeendownloaded and (local_retries > 0)) :
            h = retrieve(url + '/' + fname, dest)[1]
            if h.type.find(filetype) == -1:
                log.warning('cannot download %s - retry' % fname)
                os.remove(dest)
            else:
                if md5_check and isTarBall(dest):
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
            sys.exit('Cannot download %s after %s attempts - exit ' % (fname, nb_retries) + '\n')

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
            log.warning('cannot remove file %s' % filename)

def getReferenceMD5(url, filen, dest):
    log = logging.getLogger()
    md5name = getMD5FileName(filen)
    filename = os.path.join(dest, md5name)
    if md5name :
        if os.path.exists(filename) :
            os.remove(filename)
        filename = retrieve(url + '/' + md5name, filename)[0]
    else:
        log.warning('cannot retrieve %s' % md5name)
    for line in open(filename, "r").readlines():
        md5sum = line.split(" ")[0]
    return md5sum

def checkMD5(url, filenm, dest):
    log = logging.getLogger()
    isok = False
    log.info("Checking %s tar ball consistency ..." % filenm)
    refmd5 = getReferenceMD5(url, filenm, dest)
    log.debug("   reference md5 sum is: %s" % refmd5)
    compmd5 = calculateMD5(os.path.join(dest, filenm))
    log.debug("       local md5 sum is: %s" % compmd5)
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
def getPackVer(fname):

    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]


    # get the binary if any
    if fname.find("LBSCRIPTS") == -1 :
        import LbConfiguration.Platform
        binary_list = LbConfiguration.Platform.binary_dbg_list + LbConfiguration.Platform.binary_opt_list
        for b in binary_list:
            if fname.find(b) != -1:
                bin = b
                if fname.find('_' + b) != -1 :
                    ffile = fname[:fname.find('_' + b)]
                else :
                    ffile = fname[:fname.find('.tar.gz')]
                break
            else:
                bin = None
                ffile = fname[:fname.find('.tar.gz')]
    else :
        bin = None
        ffile = fname[:fname.find('.tar.gz')]
    packver = ffile.split('_')
    vers = packver[-1]
    name = packver[0]
    file_path = os.path.join(this_lhcb_dir, name, name + '_' + vers)
    base_dir = lhcb_dir.split(os.pathsep)
    file_base = []
    for bd in base_dir :
        file_base.append(os.path.join(bd, name, name + '_' + vers))
    if name == "LBSCRIPTS" :
        file_base = []
        file_base.append(os.path.join(base_dir[0], name, name + '_' + vers))
    if name == "LCGCMT" or name == "GENSER" or name == "LCGGrid" or name == "LCGGanga":
        if len(packver) >= 2:
            vers = '_'.join(packver[1:])
            file_path = os.path.join(this_lcg_dir, name, name + '_' + vers)
            base_dir = lcg_dir.split(os.pathsep)
            file_base = []
            for bd in base_dir :
                file_base.append(os.path.join(bd, name, name + '_' + vers))
    if name == "OpenScientist" or name == "osc_vis" or name == "DIM" :
        file_path = os.path.join(this_contrib_dir, name, vers)
        base_dir = contrib_dir.split(os.pathsep)
        file_base = []
        for bd in base_dir :
            file_base.append(os.path.join(bd, name, vers))
    if packver[0] == "DBASE" or packver[0] == "PARAM":
        if len(packver) == 3:
            name = packver[1]
        else:
            name = os.path.join(packver[1], packver[2])
        file_path = os.path.join(this_lhcb_dir, packver[0], name, vers)
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
def getProjectList(name, version, binary=None):
    log = logging.getLogger()
    log.debug('get list of projects to install %s %s %s' % (name, version, binary))
    here = os.getcwd()

    import LbConfiguration.Platform
    import LbConfiguration.Package

    if name in LbConfiguration.Package.package_names :
        p = LbConfiguration.Package.getPackage(name)
        tar_file = p.tarBallName(version)
    else:
        tar_file = name.upper() + '_' + name.upper()
        if version != 0 :
            tar_file = tar_file + '_' + version
        if binary:
            tar_file = tar_file + '_' + binary

    this_html_dir = html_dir.split(os.pathsep)[0]

    if not check_only :
        checkWriteAccess(this_html_dir)
    os.chdir(this_html_dir)
    if not check_only :
        getFile(url_dist + 'html/', tar_file + '.html')

    disthtm = os.path.join(this_html_dir, "distribution.htm")

    if not check_only :
        if os.path.exists(disthtm) :
            os.remove(disthtm)
        getFile(url_dist + "html", disthtm)

    # loop over projects to be downloaded
    project_list = {}
    html_list = []
    tar_file_html = tar_file + '.html'
    if os.path.exists(tar_file_html) :
        fd = open(tar_file_html)
    else :
        log.error("File %s doesn't exist" % os.path.join(this_html_dir, tar_file_html))
        log.warning("%s %s %s is not installed" % (name, version, binary))
        sys.exit("some projects are not installed. Exiting ...")

    fdlines = fd.readlines()
    for fdline in fdlines:
        if fdline.find('was not found on this server') != -1:
            log.info('the required project %s %s %s is not available' % (name, version, binary))
            log.info('remove %s.html and exit ' % tar_file)
            fd.close()
            os.remove(tar_file + '.html')
            sys.exit(' %s %s %s is not available' % (name, version, binary) + '\n')
        if fdline.find('HREF=') != -1:
            eq_sign = fdline.find('HREF=')
            gt_sign = fdline.find('>')
            slash_sign = eq_sign + fdline[eq_sign:].find('/')
            source = fdline[eq_sign + 5:slash_sign]
            fname = fdline[slash_sign + 1:gt_sign]
            project_list[fname] = source
            html_list.append(fname)


    for fname in project_list.keys():
        if project_list[fname] == "source":
            pack_ver = getPackVer(fname)
            if pack_ver[2] != cmtconfig:
                del project_list[fname]
                html_list.remove(fname)
                if cmtconfig in LbConfiguration.Platform.binary_list:
                    newbin = cmtconfig
                    if LbConfiguration.Platform.isBinaryDbg(newbin) :
                        newbin = LbConfiguration.Platform.getBinaryOpt(newbin)
                    fname = fname.replace(pack_ver[2], newbin)
                project_list[fname] = "source"
                html_list.append(fname)
    log.debug('project_list %s' % project_list)
    log.debug('html_list %s' % html_list)

    os.chdir(here)

    return project_list, html_list

#----------------------------------------------------------------------------------
# check installation
def isInstalled(fname):
    installed = False
    if not overwrite_mode :
        # special case: the LbScripts project has to be installed locally anyway
        if fname.find("LbScripts") != -1 or fname.find("LBSCRIPTS") != -1 :
            installedfilename = os.path.join(log_dir.split(os.pathsep)[0], fname.replace(".tar.gz", ".installed"))
            if os.path.exists(installedfilename) :
                installed = True
        else :
            # regular case: the project can be installed in different locations
            for ld in log_dir.split(os.pathsep) :
                installedfilename = os.path.join(ld, fname.replace(".tar.gz", ".installed"))
                if os.path.exists(installedfilename) :
                    installed = True
                    break
    return installed

def getInstallLocation(fname) :
    location = None
    if isInstalled(fname) :
        for ld in log_dir.split(os.pathsep) :
            installedfilename = os.path.join(ld, fname.replace(".tar.gz", ".installed"))
            if os.path.exists(installedfilename) :
                location = os.path.dirname(ld)
                break

    return location

def setInstalled(filenm):
    this_log_dir = log_dir.split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir, filenm.replace(".tar.gz", ".installed"))

    if os.path.exists(installedfilename) :
        os.remove(installedfilename)

    f = open(installedfilename, "w")
    f.write("Done\n")
    f.close()

def delInstalled(fname):
    this_log_dir = log_dir.split(os.pathsep)[0]

    installedfilename = os.path.join(this_log_dir, fname.replace(".tar.gz", ".installed"))
    os.remove(installedfilename)

# check installed project
def checkInstalledProjects(project_list):
    log = logging.getLogger()
    log.info('check all project in the list %s' % project_list)
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

#
#  download project tar files ================================================
#
def getProjectTar(tar_list, already_present_list=None):
    log = logging.getLogger()
    here = os.getcwd()

    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]
    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_targz_dir = targz_dir.split(os.pathsep)[0]


    for fname in tar_list.keys():
        log.info('---------------------------------------------------------------------------------------------------------')
        if not isInstalled(fname) or overwrite_mode :
            log.debug(fname)
            if tar_list[fname] == "source":
                if fname.find('LCGCMT') != -1 or fname.find('LCGGrid') != -1 or fname.find('LCGGanga') != -1:
                    checkWriteAccess(os.path.join(this_lcg_dir, '..'))
                    os.chdir(os.path.join(this_lcg_dir, '..'))
                elif fname.find('GENSER') != -1:
                    checkWriteAccess(this_lcg_dir)
                    os.chdir(this_lcg_dir)
                else:
                    checkWriteAccess(this_contrib_dir)
                    os.chdir(this_contrib_dir)
                getFile(url_dist + 'source/', fname)
            elif tar_list[fname] == "system":
                log.info("Obsolete package %s will not be installed. The Compat project is replacing it" % fname)
            else:
                checkWriteAccess(this_lhcb_dir)
                os.chdir(this_lhcb_dir)
                getFile(url_dist + tar_list[fname] + '/', fname)


            # untar the file
            log.debug('untar file %s' % fname)
            rc = unTarFileInTmp(os.path.join(this_targz_dir, fname), os.getcwd(), overwrite=overwrite_mode)
            pack_ver = getPackVer(fname)
            if rc != 0 and (pack_ver[0] != 'LCGGrid' or pack_ver[0] != 'LCGGanga') :
                removeAll(pack_ver[3])
                log.info('Cleaning up %s' % pack_ver[3])
                sys.exit("getProjectTar: Exiting ...")
            if pack_ver[0] in LbConfiguration.External.external_projects:
                # if it is a ext_lhcb project
                # create a ext_lhcb project/vers/binary directory
                # to remember which binary tar file has been untar
                if fname.find('GENSER_v') != -1:
                # the GENSER project as such does not exist anylonger in LCG
                    os.chdir(this_lcg_dir)
                    if not os.path.exists('GENSER'): os.mkdir('GENSER')
                    os.chdir('GENSER')
                    if not os.path.exists('GENSER_' + pack_ver[1]): os.mkdir('GENSER_' + pack_ver[1])
                os.chdir(pack_ver[3])
                if not os.path.exists(pack_ver[2]):
                    os.mkdir(pack_ver[2])
                    log.info('mkdir %s in %s ' % (pack_ver[2], pack_ver[3]))

            if os.getcwd() == this_lhcb_dir :
                # if binary is requested and InstallArea does not exist : set it
                if pack_ver[2] :
                    os.chdir(os.path.join(this_lhcb_dir, pack_ver[0], pack_ver[0] + '_' + pack_ver[1]))
                    if not os.path.exists(os.path.join('InstallArea', pack_ver[2])):
                        log.debug('mkdir InstallArea')
                        if not os.path.exists('InstallArea'):
                            os.mkdir ('InstallArea')
                        os.chdir('InstallArea')
                        os.mkdir(pack_ver[2])
                if sys.platform != 'win32' :
                    try :
                        from LbLegacy.ProjectLinks import fixLinks
                        log.debug("Fixing links in %s" % pack_ver[3])
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
                                changePermissions(os.path.join('EXTRAPACKAGES', f), recursive=True)
                            shutil.rmtree(extradir, ignore_errors=True)
                if pack_ver[0] == "LBSCRIPTS" :
                    genlogscript = os.path.join(pack_ver[3], "InstallArea", "scripts", "generateLogin")
                    log.debug("Running: %s --without-python --no-cache -m %s --login-version=%s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                    os.environ["LHCBPROJECTPATH"] = os.pathsep.join([os.path.join(os.environ["MYSITEROOT"], "lhcb"), os.path.join(os.environ["MYSITEROOT"], "lcg", "external")])
                    log.debug("LHCBPROJECTPATH: %s" % os.environ.get("LHCBPROJECTPATH", None))
                    os.system("python %s --without-python --no-cache -m %s --login-version=%s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                    registerPostInstallCommand("LCGCMT", "python %s --no-cache -m %s %s" % (genlogscript, os.environ["MYSITEROOT"], pack_ver[1]))
                    prodlink = os.path.join(os.path.dirname(pack_ver[3]), "prod")
                    if sys.platform != "win32" :
                        if os.path.exists(prodlink) :
                            if os.path.islink(prodlink) :
                                os.remove(prodlink)
                                os.symlink(pack_ver[0] + '_' + pack_ver[1], prodlink)
                                log.debug("linking %s to %s" % (pack_ver[0] + '_' + pack_ver[1], prodlink))
                            else :
                                log.error("%s is not a link. Please remove this file/directory" % prodlink)
                    my_dir = os.path.dirname(this_lhcb_dir)
                    for f in os.listdir(os.path.join(pack_ver[3], "InstallArea", "scripts")) :
                        if f.startswith("LbLogin.") and not (f.endswith(".zsh") or f.endswith(".py")):
                            sourcef = os.path.join(pack_ver[3], "InstallArea", "scripts", f)
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
                                log.debug("linking %s to %s" % (sourcef, targetf))


            setInstalled(fname)
        else :
            log.info('%s already installed' % fname)
            if already_present_list != None:
                already_present_list.append(tar_list[fname])
    for fname in tar_list.keys():
        pack_ver = getPackVer(fname)
        prj = pack_ver[0]
        if isProjectRegistered(prj) :
            callPostInstallCommand(prj)

    os.chdir(here)

# Autoupdate myself
def getMySelf():
    log = logging.getLogger()
    new_install = 'latest_install_project.py'
    if os.path.exists("latest_install_project.py") :
        os.remove("latest_install_project.py")
    getFile(url_dist, 'install_project.py')
    if fix_perm :
        changePermissions('latest_install_project.py', recursive=False)
    latest_version = os.popen("python %s --version" % new_install).read()[:-1]
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
    if isInstalled(scripttar) and not overwrite_mode :
        location = getInstallLocation(scripttar)
        log.debug("LbScripts %s is already installed in %s" % (lbscripts_version, location))
        for l in lhcb_dir.split(os.pathsep) :
            if l.startswith(location) :
                that_lhcb_dir = l
                break
        sys.path.insert(0, os.path.join(that_lhcb_dir, "LBSCRIPTS", "LBSCRIPTS_%s" % lbscripts_version, "InstallArea", "python"))
        log.debug("sys.path is %s" % os.pathsep.join(sys.path))
    else :
        log.info("LbScripts %s is not installed. Dowloading it." % lbscripts_version)
        getFile(url_dist + 'LBSCRIPTS/', scripttar)
        this_bootscripts_dir = bootscripts_dir.split(os.pathsep)[0]
        this_targz_dir = targz_dir.split(os.pathsep)[0]
        if not os.path.isdir(this_bootscripts_dir) :
            os.mkdir(this_bootscripts_dir)
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

def showCompatibleConfigs():
    from LbConfiguration.Platform import NativeMachine
    m = NativeMachine()
    print " ".join(m.CMTSupportedConfig(debug=True))
#
#  list available versions ==============================================================================
#
def getVersionList(pname, ver=None):
    from LbConfiguration.Version import sortStrings
    from LbConfiguration.Package import package_names, getPackage

    datapackage = False

    log = logging.getLogger()
    log.debug('Browsing versions for %s ' % pname)

    if pname.upper() in [ x.upper() for x in package_names] :
        p = getPackage(pname)
        PROJECT = p.project().upper()
        datapackage = True
    else :
        PROJECT = pname.upper()

    webpage = urlopen(url_dist + '/' + PROJECT)
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
#                        log.info(filename)
                else :
                    plist.append(filename)
#                    log.info(filename)

    atexit.register(urlcleanup)
    return sortStrings(plist, safe=True)

def listVersions(pname, ver=None):
    log = logging.getLogger()
    for l in getVersionList(pname, ver) :
        log.info(l)

def doesVersionExist(vlist, pname, version, cmt_config=None):
    from LbConfiguration.Package import package_names, getPackage
    from LbConfiguration.Project import project_names, getProject

    exist = False

    isproj = False

    if pname in package_names :
        p = getPackage(pname)
    elif pname in project_names:
        p = getProject(pname)
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

    if pname in package_names :
        ispack = True
    elif pname in project_names:
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

def getLatestVersion(pname, cmt_config=None):
    vlist = getProjectVersions(pname, cmt_config)
    return vlist[-1]

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

    this_lhcb_dir = lhcb_dir.split(os.pathsep)[0]
    this_html_dir = html_dir.split(os.pathsep)[0]
    this_targz_dir = targz_dir.split(os.pathsep)[0]
    this_lcg_dir = lcg_dir.split(os.pathsep)[0]
    this_log_dir = log_dir.split(os.pathsep)[0]
    this_contrib_dir = contrib_dir.split(os.pathsep)[0]

    import LbConfiguration.Project

    PROJECT = project.upper()

    if PROJECT in [ p.NAME() for p in LbConfiguration.Project.project_list ] :
        head = this_lhcb_dir
        VERSION = PROJECT + '_' + pvers

        flist = os.listdir(this_html_dir)
        for file in flist:
            if file.find(VERSION + '.') != -1 or file.find(VERSION + '_') != -1 :
                os.remove(os.path.join(this_html_dir, file))
                log.info('remove %s' % os.path.join(this_html_dir, file))

        flist = os.listdir(this_targz_dir)
        for file in flist:
            if file.find(VERSION + '.') != -1 or file.find(VERSION + '_') != -1:
                os.remove(os.path.join(this_targz_dir, file))
                log.info('remove %s' % os.path.join(this_targz_dir, file))

        flist = os.listdir(this_log_dir)
        for file in flist:
            if file.find(VERSION + '.') != -1 or file.find(VERSION + '_') != -1:
                os.remove(os.path.join(this_log_dir, file))
                log.info('remove %s' % os.path.join(this_log_dir, file))

        if os.path.isdir(os.path.join(head, PROJECT, VERSION)):
            shutil.rmtree(os.path.join(head, PROJECT, VERSION))
            log.info('remove %s' % os.path.join(head, PROJECT, VERSION))
        if os.path.isdir(os.path.join(head, project + 'Env', pvers)):
            shutil.rmtree(os.path.join(head, project + 'Env', pvers))
            log.info('remove %s' % os.path.join(head, project + 'Env', pvers))
    else:
        proj_vers = project + '_' + pvers
        flist = os.listdir(this_html_dir)
        for file in flist:
            if file.find(proj_vers) != -1 :
                os.remove(os.path.join(this_html_dir, file))
                log.info('remove %s' % os.path.join(this_html_dir, file))

        flist = os.listdir(this_targz_dir)
        for file in flist:
            if file.find(proj_vers + '.') != -1 or file.find(proj_vers + '_') != -1:
                os.remove(os.path.join(this_targz_dir, file))
                log.info('remove %s \n' % os.path.join(this_targz_dir, file))

        flist = os.listdir(this_log_dir)
        for file in flist:
            if file.find(proj_vers + '.') != -1 or file.find(proj_vers + '_') != -1:
                os.remove(os.path.join(this_log_dir, file))
                log.info('remove %s' % os.path.join(this_log_dir, file))

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
    if os.path.isdir(os.path.join(head, project, pvers)):
        shutil.rmtree(os.path.join(head, project, pvers))
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
    lbscriptspydir = os.path.join(this_lhcb_dir, "LBSCRIPTS", "LBSCRIPTS_" + lbscripts_version, "InstallArea", "python")
    if os.path.exists(lbscriptspydir) :
        sys.path.append(lbscriptspydir)
    else :
        log.error("%s doesn't exist" % lbscriptspydir)
    # setup the base LHCb environment
    lbloginscript = os.path.join(lbscriptspydir, "LbConfiguration", "LbLogin.py")
    log.debug("Using LbLogin from %s" % lbloginscript)
    if os.path.exists(lbloginscript) :
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

def createBaseDirs(pname, pversion):
    global multiple_mysiteroot
    global cmtconfig
    global log_dir, contrib_dir, lcg_dir, lhcb_dir, html_dir
    global bootscripts_dir, targz_dir, tmp_dir


    log = logging.getLogger()

    mysiteroot = None
    # removes the trailing "/" at the end of the path
    if os.environ.has_key("MYSITEROOT") :
        mysiteroot = os.environ["MYSITEROOT"]
    else:
        if os.environ.has_key("VO_LHCB_SW_DIR") :
            candidate_dir = os.path.join(os.environ["VO_LHCB_SW_DIR"], "lib")
            if os.path.isdir(candidate_dir) :
                log.warning("Using no MYSITEROOT defined. Using MYSITEROOT=$VO_LHCB_SW_DIR/lib")
                log.warning("With VO_LHCB_SW_DIR=%s" % candidate_dir)
                mysiteroot = candidate_dir

    if not mysiteroot :
        sys.exit('please set $MYSITEROOT == $INSTALLDIR:$MYSITEROOT before running the python script \n')

    path_list = []
    for p in  mysiteroot.split(os.pathsep) :
        while p.endswith(os.sep) :
            p = p[:-1]
        path_list.append(p)
    if len(path_list) > 1 :
        multiple_mysiteroot = True
    else :
        multiple_mysiteroot = False
    mysiteroot = os.pathsep.join(path_list)
    os.environ["MYSITEROOT"] = mysiteroot

        
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


    log_dir = _multiPathJoin(mysiteroot, "log")
    contrib_dir = _multiPathJoin(mysiteroot, "contrib")
    lcg_dir = _multiPathJoin(mysiteroot, os.path.join("lcg", "external"))
    lhcb_dir = _multiPathJoin(mysiteroot, "lhcb")
    html_dir = _multiPathJoin(mysiteroot, "html")
    bootscripts_dir = _multiPathJoin(mysiteroot, "bootscripts")
    targz_dir = _multiPathJoin(mysiteroot, "targz")

    if sys.platform != "win32" :
        tmp_dir = _multiPathJoin(mysiteroot, os.path.join("tmp", os.environ["USER"]))
    else :
        tmp_dir = _multiPathJoin(mysiteroot, "tmp")

    logname = None

    if pversion :
        if logname :
            logname = os.path.join(log_dir.split(os.pathsep)[0], pname + '_' + pversion + '.log')
        else :
            logname = os.path.join(log_dir.split(os.pathsep)[0], pname + '.log')
    if not check_only :
        done = createDir(mypath.split(os.pathsep)[0], logname)
    else :
        done = False
    if not done :
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
    getBootScripts()

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

    getCMT(cmtversion)

    if not pversion:
        if binary:
            pversion = getLatestVersion(pname, binary)
        else :
            pversion = getLatestVersion(pname)

    from LbConfiguration.Platform import isBinaryDbg, getBinaryOpt, getBinaryDbg


    if not check_only :
        if pname != 'LbScripts' :
            script_project_list = getProjectList('LbScripts', lbscripts_version)[0]
            getProjectTar(script_project_list)

    project_list, html_list = getProjectList(pname, pversion)


    cmtconfig_dbg = getBinaryDbg(cmtconfig)
    cmtconfig_opt = cmtconfig
    if isBinaryDbg(cmtconfig) :
        cmtconfig_dbg = cmtconfig
        cmtconfig_opt = getBinaryOpt(cmtconfig)

    if not check_only :
        if pname != 'LHCbGrid' and sys.platform != "win32" and cmtconfig.find("slc3") != -1 and cmtconfig.find("sl3") != -1 :
            grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version)
            project_list.update(grid_project_list)
            html_list += grid_html_list
            grid_project_list, grid_html_list = getProjectList('LHCbGrid', grid_version, cmtconfig_opt)
            project_list.update(grid_project_list)
            html_list += grid_html_list
        if pname != 'Compat' :
            if not compat_version:
                new_compat_version = getLatestVersion("Compat", cmtconfig_opt)
            else :
                new_compat_version = compat_version
            compat_project_list, compat_html_list = getProjectList('Compat', new_compat_version)
            project_list.update(compat_project_list)
            html_list += compat_html_list
            compat_project_list, compat_html_list = getProjectList('Compat', new_compat_version, cmtconfig_opt)
            project_list.update(compat_project_list)
            html_list += compat_html_list


    if binary :
        binary_project_list, binary_html_list = getProjectList(pname, pversion, binary)
        project_list.update(binary_project_list)
        html_list += binary_html_list


    if full_flag :
        log.info('download debug version and reconfigure it')
        binary_dbg = getBinaryDbg(binary)
        binary_opt = binary
        if isBinaryDbg(binary) :
            binary_dbg = binary
            binary_opt = getBinaryOpt(binary)
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
        checkInstalledProjects(project_list)



    getProjectTar(project_list)





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


def safeMove(src, dst):
    shutil.move(src, dst)
    if os.path.isdir(dst) and sys.platform.startswith("win32") :
        fixWinAttrib(dst)

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
            dst = src.replace(srcdir + os.sep, "")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
                safeMove(src, dst)
                dirstoremove.append(d)
        for f in files :
            src = os.path.join(root, f)
            dst = src.replace(srcdir + os.sep, "")
            dst = os.path.join(dstdir, dst)
            if not os.path.exists(dst) :
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
                safeMove(src, dst)
            elif overwrite :
                if os.path.exists(dst) :
                    os.remove(dst)
                pdst = os.path.dirname(dst)
                if not os.path.exists(pdst) :
                    os.makedirs(pdst)
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

def untarFile(fname):
    log = logging.getLogger()

    retcode = 0

    log.info('%s on %s ' % (fname, os.getcwd()))

    this_targz_dir = targz_dir.split(os.pathsep)[0]

    filename = os.path.join(this_targz_dir, fname)
    md5filename = getMD5FileName(filename)
    htmlfilename = os.path.join(html_dir, getHTMLFileName(fname))
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
                            tar.extract(tarinfo)
                    else:
                        if fname.find('script') != -1:
                            j = j + 1
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
    import LbConfiguration.Platform

    log = logging.getLogger()

    if not binary :
        log.warning("No CMTCONFIG has been provided on the command line")
        if os.environ.has_key("CMTCONFIG") :
            binary = os.environ["CMTCONFIG"]
            log.warning("Extracting CMTCONFIG from the environment: %s" % binary)
        else :
            m = LbConfiguration.Platform.NativeMachine()
            plist = m.CMTSupportedConfig(debug=True)
            if plist :
                binary = plist[0]
            else :
                binary = m.CMTNativeConfig(debug=False)
            log.warning("No CMTCONFIG in the environment")
            log.warning("Guessed CMTCONFIG is %s" % binary)
 

    os.environ['CMTCONFIG'] = binary
    os.environ['CMTDEB'] = binary
    if LbConfiguration.Platform.isBinaryOpt(binary) :
        os.environ['CMTDEB'] = LbConfiguration.Platform.getBinaryDbg(binary)
    # if a win32 binary is installed from a non win32 platform then do not cmt config
    if sys.platform != 'win32' and binary.find('win32') != -1 :
        make_flag = None

    if binary not in LbConfiguration.Platform.binary_list:
        print 'BE CAREFUL - your CMTCONFIG %s is not part of the lhcb_binary %s' % (os.getenv('CMTCONFIG'), LbConfiguration.Platform.binary_list)
        print 'do you want to continue? [yes|no]'
        next = sys.stdin.readline()
        if next.lower()[0] != 'y':
            sys.exit()

    return binary

def parseArgs():
    global debug_flag, full_flag, list_flag, remove_flag
    global cmtversion, md5_check, grid_version, nb_retries
    global setup_script, check_only, overwrite_mode
    global _retry_time, fix_perm
    global show_compatible_configs
    global install_binary
    global compat_version



    pname = None
    pversion = None
    binary = None

    arguments = sys.argv[1:]
    
    if not arguments :
        print "No enough argument passed"
        usage()
        sys.exit(2)
    try:
        opts, args = getopt.getopt(arguments, 'hdflrbp:v:c:ng:s:C',
            ['help', 'debug', 'full', 'list', 'remove', 'binary=',
             'project=', 'cmtversion=', 'nocheck',
             'retry=', 'grid=', 'setup-script=', 'check', 'overwrite',
             'compatversion=', 'retrytime=', 'nofixperm', 'version',
             'compatible-configs'])

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

    if not pname and len(args) > 0 :
        pname = args[0]
    if pname.find("/") != -1 :
        plist = pname.split("/")
        pname = "/".join(plist[1:])
        
    if not pversion and len(args) > 1 :
        pversion = args[1]

    return pname, pversion, binary

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

    thelog.info((' %s  python %s starts install_project.py - version no %s ' % (start_time, txt_python_version, script_version)).center(120))


    if not os.environ.has_key("MYSITEROOT") :
        sys.exit('please set $MYSITEROOT == $INSTALLDIR:$MYSITEROOT before running the python script \n')


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
    thelog.info((' %s end install_project.py -version no %s ' % (end_time, script_version)).center(120))


if __name__ == "__main__":
    main()
