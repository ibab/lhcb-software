# $Id: Path.py,v 1.2 2010-01-13 12:51:36 marcocle Exp $
""" module for path walk utilities """

import logging
import os
import sys

class Path(object):
    _varname = None
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
    def setVarName(cls, varname):
        cls._varname = varname
    setVarName = classmethod(setVarName)

class ExePath(Path):
    _varname = "PATH"
    def __init__(self, varname=None):
        super(ExePath, self).__init__(varname)

class LibPath(Path):
    if sys.platform != "win32" :
        _varname = "LD_LIBRARY_PATH"
    else : 
        _varname = "PATH"
    def __init__(self, varname=None):
        super(LibPath, self).__init__(varname)
        

class PyPath(Path):
    _varname = "PYTHONPATH"
    def __init__(self, varname):
        super(PyPath, self).__init__(varname)
        self.addPatSep(".")
        self.enableSubLevel()
        
class OptSearchPath(PyPath):
    _varname = "JOBOPTSEARCHPATH"
    def __init__(self, varname):
        super(OptSearchPath, self).__init__(varname)
        self.addPatSep(".")
        self.enableSubLevel()

class HelpPath(Path):
    _varname = "HPATH"
    def __init__(self, varname):
        super(HelpPath, self).__init__(varname)
        
class CMTPath(Path):
    _varname = "CMTPATH"
    def __init__(self, varname):
        super(CMTPath, self).__init__(varname)
        
class CMTProjectPath(Path):
    _varname = "CMTPROJECTPATH"
    def __init__(self, varname):
        super(CMTProjectPath, self).__init__(varname)


def multiPathJoin(path, subdir):
    pathlist = []
    for d in path.split(os.pathsep) :
        pathlist.append(os.path.join(d,subdir))
    return os.pathsep.join(pathlist)

def multiPathGetFirst(path, subdir):
    """ Quick way to find a file or dir in a series of paths """
    result = None
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
        result.append(multiPathGetFirst(path, subdir))
    else :
        for d in path.split(os.pathsep) :
            sd = os.path.join(d, subdir)
            if os.path.exists(sd) :
                result.append(sd)
    return result

