""" LHCb version style definition and massaging """
# $Id: Version.py,v 1.5 2010-03-11 13:47:34 hmdegaud Exp $

from fnmatch import fnmatch

import re
import os

# This is the version style used in the releases: vXrY[pZ]
_txt_version_style = r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?'
version_style = re.compile(_txt_version_style)

# This is the version style for the LCGCMT releases: 56[a]
_txt_lcg_version_style = r'([0-9]+)([a-z]?)'
lcg_version_style  = re.compile(_txt_lcg_version_style)

# user version style : hmd_091008
_txt_user_cvs_version_style = r'(?:\w+_([0-9]{4})([0-9]{2})([0-9]{2})[a-z]?)'
user_cvs_version_style = re.compile(_txt_user_cvs_version_style)

# full version style used in CVS: the regular style with the possible "pre" suffix or the user ones.
_txt_cvs_version_style = r'%s(?:-pre([0-9]+))?|%s'% (_txt_version_style, _txt_user_cvs_version_style)

cvs_version_style = re.compile(_txt_cvs_version_style)

class NotAVersion(Exception):
    pass

class CoreVersion:
    version_style = re.compile("%s$" % _txt_version_style)
    def __init__(self, vname):
        self._vname = vname
        self._version = None
        self._patchversion = False
        try :
            m = self.version_style.match(self._vname)
        except TypeError:
            raise NotAVersion
        if m :
            a, b, c = m.groups()
            if a is None or b is None :
                raise NotAVersion
            a = int(a)
            b = int(b)
            if c is not None:
                self._patchversion = True
                c = int(c)
            self._version = (a, b, c)
        else :
            raise NotAVersion
    def __str__(self):
        return self._vname
    def __repr__(self):
        return "%s %s" % (self.__class__, self._vname)
    def __cmp__(self, other):
        return cmp(self._version, other.version())
    def name(self):
        return self._vname
    def version(self):
        return self._version


def sortVersions(versionlist, versiontype=CoreVersion, safe=False, reverse=False):
    if not safe :
        vlist = [ versiontype(x) for x in versionlist ]
    else :
        vlist = []
        for v in versionlist :
            try :
                v = versiontype(v)
            except NotAVersion :
                continue
            vlist.append(v)
    vlist.sort()
    if reverse :
        vlist.reverse()
    return [ x.name() for x in vlist]

def extractVersion(strname, versiontype=CoreVersion):
    result = None
    m = version_style.search(strname)
    if m :
        result = versiontype(m.group())
    return result


def sortStrings(strlist, versiontype=CoreVersion, safe=False, reverse=False):
    versionlist = [ (extractVersion(s, versiontype=versiontype), s) for s in strlist ]
    if safe :
        versionlist = [ t for t in versionlist if t[0]]
    versionlist.sort()
    if reverse :
        versionlist.reverse()
    return [ x[1] for x in versionlist ]
        
def getVersionsFromDir(dirname, pattern=None, versiontype=CoreVersion, reverse=False):
    strlist = []
    for o in os.listdir(dirname) :
        if pattern :
            if fnmatch(o, pattern) :
                strlist.append(o)
        else :
            strlist.append(o)
    versionlist = [ extractVersion(s, versiontype=versiontype) for s in strlist ]
    versionlist = [ v for v in versionlist if v ]
    versionlist.sort()
    if reverse :
        versionlist.reverse()
    return versionlist
