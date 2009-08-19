#@PydevCodeAnalysisIgnore
""" manipulation of files and directories module """

from shutil import copy2

import fnmatch
import stat
import os

try:
    WindowsError
except NameError:
    WindowsError = None


def isFilePathExcluded(filepath, exclude=[]):
    isexcluded = False
    for e in exclude :
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
        os.chflags(dst, st.st_flags)

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
