#@PydevCodeAnalysisIgnore
""" manipulation of files and directories module """

from LbUtils.afs.directory import AFSLock

from shutil import copy2

import md5
import logging
import fnmatch
import stat
import os

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
                        copy2(srcname, dstname)
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
        m = md5.new()
        buf = f.read(buffer_size)
        while(buf):
            m.update(buf)
            buf = f.read(buffer_size)
    else :
        m = md5.new(f.read())
    f.flush()
    os.fsync(f.fileno())
    f.close()
    lock.unlock()
    return m.hexdigest()

def checkMD5Sum(fname, md5sum):
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

    lock = AFSLock(md5fname)
    lock.lock(force=False)

    mdf = open(md5fname, "w")
    mdf.write("%s  %s" % (md5sum, targetname))
    mdf.flush()
    os.fsync(mdf.fileno())
    mdf.close()
    lock.unlock()


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