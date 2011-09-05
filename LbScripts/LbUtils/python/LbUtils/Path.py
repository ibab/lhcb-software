# $Id$
""" module for path walk utilities """

import os
import sys
from subprocess import Popen, PIPE

class Path(object):
    def __init__(self, varname=None):
        if varname :
            self._varname = varname
        self._sublevel = False
        self._pattern_separator = [ "/" ]
        if sys.platform == "win32" :
            self.addPatSep("\\")
        self._pathlist = []
        if self._varname and self._varname in os.environ :
            self._pathlist = os.environ[self._varname].split(os.pathsep)
    def addPatSep(self, separator):
        "add one sublevel separator for the pattern to match"
        self._pattern_separator.append(separator)
    def removePatSep(self, separator):
        "remove one sublevel separator for the pattern to match"
        if separator in self._pattern_separator :
            self._pattern_separator.remove(separator)
    def patSep(self):
        "returns the sublevel separators for the pattern to match"
        return self._pattern_separator
    def enableSubLevel(self):
        self._sublevel = True
    def disableSubLevel(self):
        self._sublevel = False
    def subLevel(self):
        return self._sublevel
    def varName(self):
        return self._varname
    def setVarName(self, varname):
        self._varname = varname

class ExePath(Path):
    def __init__(self):
        super(ExePath, self).__init__("PATH")

class LibPath(Path):
    def __init__(self):
        if sys.platform != "win32" :
            varname = "LD_LIBRARY_PATH"
        else :
            varname = "PATH"
        super(LibPath, self).__init__(varname)


class PyPath(Path):
    def __init__(self, varname):
        super(PyPath, self).__init__("PYTHONPATH")
        self.addPatSep(".")
        self.enableSubLevel()

class OptSearchPath(PyPath):
    def __init__(self):
        super(OptSearchPath, self).__init__("JOBOPTSEARCHPATH")
        self.addPatSep(".")
        self.enableSubLevel()

class HelpPath(Path):
    def __init__(self, varname):
        super(HelpPath, self).__init__("HPATH")

class CMTPath(Path):
    def __init__(self):
        super(CMTPath, self).__init__("CMTPATH")

class CMTProjectPath(Path):
    def __init__(self):
        super(CMTProjectPath, self).__init__("CMTPROJECTPATH")

def pathPrepend(path, dirname, exist_check=False):
    path_list = path.split(os.pathsep)
    if exist_check :
        if os.path.exists(dirname) :
            path_list.insert(0, dirname)
    else :
        path_list.insert(0, dirname)
    return os.pathsep.join(path_list)

def pathAppend(path, dirname, exist_check=False):
    path_list = path.split(os.pathsep)
    if exist_check :
        if os.path.exists(dirname) :
            path_list.append(dirname)
    else :
        path_list.append(dirname)
    return os.pathsep.join(path_list)

def pathRemove(path, path_components):
    """
    remove the entry matching the path components from the path
    @param path: orginal path
    @type path: string
    @param path_components: list of component of an entry path ["/usr","lib"]
    @type path_components: list
    """
    path_match = []
    for c in path_components :
        if os.sep in c :
            path_match += c.split(os.sep)
        else :
            path_match.append(c)
    path_match = [ x for x in path_match if x ]
    final_path = []
    for p in path.split(os.pathsep) :
        if os.sep.join(path_match) not in p :
            final_path.append(p)
    return os.pathsep.join(final_path)

def pathAdd(path1, path2, exist_check=False):
    result = path1
    for d in path2.split(os.pathsep) :
        result = pathAppend(result, d, exist_check)
    return result

def cleanPath(path_value, normalize=False):
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


def envPathPrepend(pathname, dirname, env_dict=None, exist_check=False):
    if not env_dict :
        env_dict = os.environ
    env_dict[pathname] = pathPrepend(env_dict[pathname], dirname, exist_check)

def envPathAppend(pathname, dirname, env_dict=None, exist_check=False):
    if not env_dict :
        env_dict = os.environ
    env_dict[pathname] = pathAppend(env_dict[pathname], dirname, exist_check)

def multiPathJoin(path, subdir, exist_check=False):
    pathlist = []
    for d in path.split(os.pathsep) :
        jd = os.path.join(d, subdir)
        if exist_check :
            if os.path.exists(jd) :
                pathlist.append(jd)
        else :
            pathlist.append(jd)
    return os.pathsep.join(pathlist)

def multiPathUpdate(path, dirlist, interleaved=False, exist_check=False):
    """ update the path with the list of subdirectories
    @param path: original path
    @param dirlist: list of subdirectories to be collated to each entry of
                    path
    @param interleaved:
      - False : path=pth1:pth2 and dirlist=[dir1,dir2] -> pth1/dir1:pth2/dir1:pth1/dir2:pth2/dir2
      - True  : path=pth1:pth2 and dirlist=[dir1,dir2] -> pth1/dir1:pth1/dir2:pth2/dir2:pth2/dir2
    @return: the updated path
    """
    pathlist = []
    if not interleaved :
        for d in dirlist :
            pathlist += multiPathJoin(path, d, exist_check).split(os.pathsep)
    else :
        for p in path.split(os.pathsep) :
            for d in dirlist :
                jd = os.path.join(p, d)
                if exist_check :
                    if os.path.exists(jd) :
                        pathlist.append(jd)
                else :
                    pathlist.append(jd)

    return os.pathsep.join(pathlist)


def multiPathGetFirst(path, subdir, default=None):
    """ Quick way to find a file or dir in a series of paths """
    result = default
    for d in path.split(os.pathsep) :
        sd = os.path.join(d, subdir)
        if os.path.exists(sd) :
            result = sd
            break
    return result

def multiPathGet(path, subdir, alloccurences=False):
    """ Find all occurences of a file or dir in a series of paths """
    result = []
    if not alloccurences :
        item = multiPathGetFirst(path, subdir)
        if item :
            result.append(item)
    else :
        for d in path.split(os.pathsep) :
            sd = os.path.join(d, subdir)
            if os.path.exists(sd) :
                result.append(sd)
    return result

def _FSType(path):
    try:
        p = Popen(["df", "--print-type", "--portability", path], stdout = PIPE, stderr = PIPE)
        out, _ = p.communicate()
        fstype = out.splitlines()[-1].split()[1]
    except:
        fstype = "Unknown"
    return fstype

def isCVMFS(path):
    """
    Tells if a path is on a CVMFS mount.
    It works only on Linux, for other OSs it returns always false.
    """
    return _FSType(path) == "cvmfs"
