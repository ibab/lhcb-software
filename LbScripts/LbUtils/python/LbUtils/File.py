#@PydevCodeAnalysisIgnore
""" manipulation of files and directories module """

from shutil import copy2

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
