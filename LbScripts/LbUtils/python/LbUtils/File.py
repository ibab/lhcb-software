#@PydevCodeAnalysisIgnore
""" manipulation of files and directories module """

from LbUtils.afs.directory import AFSLock, AFSLockFile

import shutil

import logging
import fnmatch
import stat
import os, sys

import __builtin__
if "set" not in dir(__builtin__):
    # pylint: disable-msg=W0622
    from sets import Set as set

if sys.version_info >= (2,6) :
    from hashlib import md5
    from hashlib import sha1
else :
    from md5 import md5
    from sha import sha as sha1


try:
    WindowsError
except NameError:
    WindowsError = None


def isFilePathExcluded(filepath, excludelist=None):
    if not excludelist :
        excludelist = []
    isexcluded = False
    for e in excludelist :
        if fnmatch.fnmatch(filepath, e) :
            isexcluded = True
            break
    return isexcluded



def copyStat(src, dst, symlinks=True):
    """Copy all stat info (mode bits, atime, mtime, flags) from src to dst"""
    if symlinks and os.path.islink(src) :
        st = os.lstat(src)
    else :
        st = os.stat(src)
    mode = stat.S_IMODE(st.st_mode)
    if hasattr(os, 'utime'):
        os.utime(dst, (st.st_atime, st.st_mtime))
    if hasattr(os, 'chmod'):
        os.chmod(dst, mode)
    if hasattr(os, 'chflags') and hasattr(st, 'st_flags'):
        os.chflags(dst, st.st_flags)#IGNORE:E1101

def copyTree(src, dst, symlinks=False, ignore=None):
    """Recursively copy a directory tree using copy2().
    """
    if src[-1] != os.sep:
        src = src + os.sep

    errors = []
    if not os.path.exists(dst) :
        os.makedirs(dst)
        copyStat(src, dst)
    for root, dirs, files in os.walk(src):
        dirs_to_remove = []
        relroot = root.replace(src, "")
        dstroot = os.path.join(dst, relroot)
        ignored_names = set()
        if ignore is not None :
            ignored_names |= ignore(root, dirs)
            ignored_names |= ignore(root, files)
        try :
            for d in dirs :
                srcname = os.path.join(root, d)
                dstname = os.path.join(dstroot, d)
                if d not in ignored_names :
                    os.makedirs(dstname)
                    copyStat(srcname, dstname)
                else :
                    dirs_to_remove.append(d)
            for f in files :
                srcname = os.path.join(root, f)
                dstname = os.path.join(dstroot, f)
                if f not in ignored_names :
                    if symlinks and os.path.islink(srcname) :
                        linkto = os.readlink(srcname)
                        os.symlink(linkto, dstname)
                        copyStat(srcname, dstname)
                    else :
                        shutil.copy2(srcname, dstname)
            copyStat(root, dstroot)
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        except OSError, why:
            if WindowsError is not None and isinstance(why, WindowsError):
                # Copying file access times may fail on Windows
                pass
            else:
                errors.append((root, dstroot, str(why)))
        for d in dirs_to_remove :
            dirs.remove(d)

def move(src, dst):
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


def getEmpyFiles(topdir, filterfunc=None, extlist=None):
    """
    generator function that looks for empty files The filter takes has argument
    the full file name path.
    @param filterfunc: function that returns True or False. it takes one
    argument which is the full file name path.
    @param extlist: list of files extension to check for
    @return: yields empty files
    """
    if not extlist :
        extlist = []
    if not filterfunc :
        filterfunc = lambda x: True
    for data in os.walk(topdir) :
        root = data[0]
        files = data[2]
        for f in files :
            fn = os.path.join(root, f)
            fnext = os.path.splitext(fn)[1]
            if fnext :
                fnext = fnext[1:]
            if os.path.isfile(fn) and not os.path.islink(fn) and filterfunc(fn) :
                if extlist and fnext in extlist:
                    if os.path.getsize(fn) == 0 :
                        yield fn
                else :
                    if os.path.getsize(fn) == 0 :
                        yield fn

def checkEmptyFiles(topdir, filterfunc=None, extlist=None):
    """
    Checks for empty files. The filter takes has argument
    the full file name path.
    @param filterfunc: function that returns True or False. it takes one
    argument which is the full file name path.
    @param extlist: list of files extension to check for
    @return: True if there was no empty file found, otherwise false.
    """
    log = logging.getLogger()
    good = True
    for f in getEmpyFiles(topdir, filterfunc, extlist) :
        log.warning("%s is null sized" % f)
        good = False

    return good


def computeMD5Sum(fname, buffer_size=2**13):
    lock = AFSLock(fname)
    lock.lock(force=False)
    f = open(fname, "rb")
    if buffer_size :
        m = md5()
        buf = f.read(buffer_size)
        while(buf):
            m.update(buf)
            buf = f.read(buffer_size)
    else :
        m = md5(f.read())
    f.flush()
    os.fsync(f.fileno())
    f.close()
    lock.unlock()
    return m.hexdigest()

def checkMD5Sum(fname, md5sum):
    """
    Check a file against a reference md5 sum
    @param fname:
    @param md5sum:
    """
    good = False
    loc_md5sum = computeMD5Sum(fname)
    if loc_md5sum == md5sum :
        good = True
    return good

def createMD5File(fname, md5fname, targetname=None):
    """ create a md5 sum file from fname
    @param fname: initial file name to be processed
    @param md5fname: name of the create md5 sum file
    @param targetname: name of the file to appear in the md5 sum file
    """
    md5sum = computeMD5Sum(fname)
    if not targetname :
        targetname = fname

    locked_file = AFSLockFile(md5fname, "w", force=False)
    locked_file.write("%s  %s" % (md5sum, targetname))
    locked_file.close()




def getMD5Info(md5fname):
    """ return the internal information of a md5 file
    @param md5fname: the md5 sum file
    @return: the md5 sum and the internal name of the summed up file
    """
    refmd5sum = None
    iname = None
    lock = AFSLock(md5fname)
    lock.lock(force=False)
    f = open(md5fname, "r")
    for line in f.readlines():
        if line.endswith("\n") :
            line = line[:-1]
        refmd5sum, iname = line.split("  ")[0:2]
    f.flush()
    os.fsync(f.fileno())
    f.close()
    lock.unlock()
    return refmd5sum, iname

def checkMD5File(md5fname, fname=None):
    """ Check the md5 sum file
    @param md5fname: file containing the checksum and the filename
    @param fname: optional override of the file name in the md5 sum file
    @param checkfilename: if the fname is passed check that the internal name
    is equal to it
    """
    refmd5sum, iname = getMD5Info(md5fname)
    if not fname:
        fname = iname
    if not os.path.exists(fname) :
        fname = os.path.join(os.path.dirname(md5fname), fname)

    return checkMD5Sum(fname, refmd5sum)

def checkMD5Info(md5fname, target_name):
    """ check the md5 file informations. It checks the md5sum and the target
    name as well
    @param md5fname: the md5 sum file to be checked
    @param target_name: the internal target name
    """
    iname = getMD5Info(md5fname)[1]
    return (checkMD5File(md5fname) and (target_name == iname))


def genTemplate(tmpl_file, string_dict):
    ft = open(tmpl_file)
    txt = ft.read()
    ft.close()
    return txt % string_dict

def genTemplateFile(tmpl_file, string_dict, output_file):
    """ generate a file from a template using a dictionary of strings """
    fo = open(output_file, "w")
    fo.write(genTemplate(tmpl_file, string_dict))
    fo.close()





