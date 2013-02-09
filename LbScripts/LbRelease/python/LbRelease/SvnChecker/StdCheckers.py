# -*- coding: utf-8 -*-
'''
Common and generic checker implementations.

Created on Nov 24, 2010
'''
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

from Core import Checker, PathChecker, Not, Failure
import re
import logging
import sys
import codecs
from StringIO import StringIO

class PropertyChecker(Checker):
    """
    Check the value of a property.

    It is a hybrid checker (the instance can be used on transactions or on paths).

    By default it requires that the requested property is present, but a checker
    function can be passed to the constructor to perform custom validation on
    the value of the property.
    The constructor argument 'mandatory' (defaulted to True) tells if the
    absence of a the requested property must be considered a failure or not.
    """
    def __init__(self, property_name, check = None, mandatory = True):
        super(PropertyChecker, self).__init__()
        self.property = property_name
        if check is None:
            self.check = lambda _: True
        else:
            self.check = check
        self.mandatory = mandatory

    def _msg(self, res, value, path):
        msg = "Value '%s' %s for property '%s'" % (value, res and "allowed" or "not allowed", self.property)
        if path:
            msg += " on node '%s'" % path
        return "Value '%s' %s for property '%s'" % (value, res and "allowed" or "not allowed", self.property)

    def __call__(self, txn, path = None):
        self.log.debug("Checking property %s on %s", self.property, path or "transaction")
        if not path:
            value = txn[self.property]
        else:
            value = txn.node_property(path, self.property)

        if value is None:
            self.log.debug("not found (required: %s)", self.mandatory)
            msg = "Property '%s' not found" % self.property
            if path:
                msg += " on node '%s'" % path
            # mandatory == True -> fail
            return (not self.mandatory, msg)

        res = self.check(value)
        self.log.debug("%s value", res and "good" or "bad")
        return (res, self._msg(res, value, path))

def AllowedUsers(users):
    """
    Shortcut to instantiate a property checker that compare the name of the
    committer with a list of authorized users.
    """
    return PropertyChecker("svn:author", lambda x: x in users)

class AllowedUsers2(PropertyChecker):
    """
    Alternative implementation of 'AllowedUsers'.
    """
    def __init__(self, users):
        super(AllowedUsers2, self).__init__("svn:author", lambda x: x in users)
    def _msg(self, res, value):
        return "User %s %s" % (value, res and "allowed" or "not allowed")

class MovePackage(Checker):
    """
    Checker to allow move of packages.
    A valid transaction must contain a change in the property 'packages' of '/',
    and move of trunk, tags and branches of the correct package.
    """
    def __call__(self, txn):
        self.log.debug("Check for move signature")
        if '/' not in txn.changes or txn.change_kind('/') != "M":
            return (False, "No change in '/'")
        # we want 'pure' moves, so we need to spot changes that we do not want
        consumed_changes = ['/']
        # get new list of packages (dict pkg->project)
        pkgs = txn.node_property('/', 'packages').splitlines()
        pkgs = dict([ l.split() for l in map(str.strip, pkgs) if l and not l.startswith("#") ])
        # find the packages that are being moved
        moves = []
        all_changes = list(txn.changes)
        for p in all_changes:
            if ('/trunk/' in p) and (txn.change_kind(p) == "D"):
                src, pkg = p.strip('/').split('/trunk/')
                moves.append((src, pkg))
                consumed_changes.append(p)
        for src, pkg in moves:
            # find the destination
            dst = pkgs.get(pkg)
            self.log.debug("Moving package %s from project %s to %s", pkg, src, dst)
            if src == dst:
                self.log.debug("'packages' property still refers to %s for package %s", src, pkg)
                return (False, "Bad change in property 'packages'")
            # look for trunk, tags and branches moves
            for t in ['/trunk/', '/tags/', '/branches/']:
                s = "/%s%s%s" % (src, t, pkg)
                if s not in txn.changes or txn.change_kind(s) != 'D':
                    return (False, "path %s not removed" % s)
                consumed_changes.append(s)
                d = "/%s%s%s" % (dst, t, pkg)
                if d not in txn.changes or txn.change_kind(d) != 'A':
                    return (False, "path %s not added" % d)
                try:
                    c = txn.copied_from(d)[1]
                except:
                    c = None
                if c != s:
                    return (False, "path %s copied from %s instead of %s" % (d, c, s))
                consumed_changes.append(d)
        # check that we consumed all changes
        extra_changes = sorted([c for c in all_changes if c not in consumed_changes])
        if extra_changes:
            self.log.debug("Changes not consumed: %s", extra_changes)
            return (False, "Cannot mix package move and other changes:\n"
                            + "\n".join(map(txn.formatChange, extra_changes)))
        return (True, "Valid move of packages")

class ForeachPath(Checker):
    """
    Wrapper to group a PathChecker instance and makes them a Checker.
    """
    def __init__(self, checker, filter_ = "", stop_if = False):
        """
        Initialize the instance.

        @param filter_: an optional callable or a regular expression to select
        the files over which to iterate.
        """
        super(ForeachPath, self).__init__()

        self.checker = checker

        self._regexp = None
        if hasattr(filter_, "__call__"): # callable
            self.filter = filter_
        elif hasattr(filter_, "match"): # regular expression
            self.filter = filter_.match
        else:
            self._regexp = filter_
            self.filter = re.compile(filter_).match

        self.stop_if = stop_if

    def __call__(self, txn):
        res, msg = (True, '')
        self.log.debug("Looping over paths")
        for path in filter(self.filter, txn.changes):
            res, msg = self.checker(txn, path)
            if self.stop_if == res:
                break # exit when requested
        self.log.debug("Loop completed (%s)", res)
        return (res, msg)

    def __repr__(self):
        r = "%s(%r" % (self.__class__.__name__, self.checker)
        if self._regexp is None:
            r += ", %r" % self.filter
        else:
            r += ", %r" % self._regexp
        if self.stop_if:
            r += ", %r" % self.stop_if
        return r + ")"

def AnyPath(checker, filter_ = ""):
    """
    Apply the PathChecker to all the paths ensuring that at least one is OK.
    """
    return ForeachPath(checker, filter_, True)

def AllPaths(checker, filter_ = ""):
    """
    Apply the PathChecker to all the paths ensuring that all of them are OK.
    """
    return ForeachPath(checker, filter_, False)

def NotContains(filter_ = ""):
    """
    Success if the transaction does not contains the required path, failure if
    it does.
    """
    # AllPaths succeeds if there is no match, so we fail if there is a match and
    # invert.
    return AllPaths(Failure(), filter_)

def Contains(filter_ = ""):
    """
    Success if the transaction contains the required path, failure if not.
    """
    # AllPaths succeeds if there is no match, so we fail if there is a match and
    # invert.
    return Not(NotContains(filter_))

class OnPath(Checker):
    """
    Apply a PathChecker to a single file.
    """
    def __init__(self, path, checker):
        self.checker = checker
        self.path = path
    def __call__(self, txn):
        return self.checker(txn, self.path)

tagsFilter = r".*/tags/.*"
_versionFormat = r"^((v\d+r\d+(p\d+)?(g\d+)?(-pre\d*)?)|(\w+_\d{8}[a-z]?))$"

class TagCheckerBase(PathChecker):
    """
    Checker class for package tags.
    """
    def __init__(self, formatString = None):
        """
        @param formatString: optional regular expression to validate the tag directory
        name (the default is the standard LHCb tag names).
        """
        super(TagCheckerBase, self).__init__()

        if formatString is None:
            self._formatString = _versionFormat
        else:
            self._formatString = formatString
        self.format = re.compile(self._formatString)

    def _validCopy(self, src, dest):
        raise RuntimeError("Method not implemented")

    def __call__(self, txn, path):
        self.log.debug("check %s", path)
        try:
            _id, src = txn.copied_from(path)
            if not src:
                self.log.debug("not a copy (no src)")
                return (False, "Only copies are allowed")
            if not self._validCopy(src, path):
                self.log.debug("not a valid copy from %s", src)
                return (False, "Invalid tag copy")
        except:
            self.log.debug("not a copy")
            # if copy_from fails, it is not a copy
            return (False, "Only copies are allowed")
        return (True, "")

class PackageTag(TagCheckerBase):
    """
    Checker class for package tags.
    It allows the following copies:
     - Proj/trunk/[Hat/]Package -> Proj/tags/[Hat/]Package/vXrY
     - Proj/{branches,tags}/[Hat/]Package/vXrY -> Proj/tags/[Hat/]Package/vXrZ
    and prevent:
     - if the projects do not match
     - if the version is not valid
     - if the packages do not match
    """
    def _validCopy(self, src, dest):
        try:
            # splitted source
            ss = src.strip("/").split("/")
            lss = len(ss)
            # splitted destination
            ds = dest.strip("/").split("/")
            lds = len(ds)
            if not self.format.match(ds[-1]): # the last fragment of dts mast be a version
                return False
            if lds < 4: # not enough levels for a tag
                return False
            # @fixme: how to prevent Proj/trunk/Hat -> Proj/tags/Hat/vXrY ?
            #         to be done at the level of the transaction?
            if ss[1] == "trunk": # first (simple case)
                if (lss + 1) == lds:
                    return ss[2:] == ds[2:-1]
            elif ss[1] in [ "tags", "branches" ]:
                if lss == lds:
                    return ss[2:-1] == ds[2:-1]
        except:
            return False
        return False

class ProjectTag(TagCheckerBase):
    """
    Checker class for project tags.
    It allows the following copies:
     - Proj/trunk/cmt -> Proj/tags/PROJ/PROJ_vXrY/cmt
     - Proj/{branches,tags}/PROJ/PROJ_vXb -> Proj/tags/PROJ/PROJ_vXrY
     - Proj/{branches,tags}/PROJ/PROJ_vXb/cmt -> Proj/tags/PROJ/PROJ_vXrY/cmt
    and prevent:
     - if the project do not match
     - if the version is not valid
     - ...

    Temporarily, it allows, independently:
     - Proj/trunk/CMakeLists.txt -> Proj/tags/PROJ/PROJ_vXrY/CMakeLists.txt
     - Proj/trunk/toolchain.cmake -> Proj/tags/PROJ/PROJ_vXrY/toolchain.cmake
    """
    def _validCopy(self, src, dest):
        try:
            # splitted source
            ss = src.strip("/").split("/")
            lss = len(ss)
            # splitted destination
            ds = dest.strip("/").split("/")
            lds = len(ds)
            proj = ss[0].upper()
            return (# lengths matches
                    ((lss, lds) in [(3, 5), (4, 4), (5, 5)])
                    and (ds[0] == ss[0]) # same project
                    and ( (lss == 4) # 3 and 5 are acceptable only if we have 'cmt', 'CMakeLists.txt' or 'toolchain.cmake'
                          or ( ss[-1] == ds[-1] and ss[-1] in ("cmt", "CMakeLists.txt", "toolchain.cmake") ) )
                    # and (ds[1] == "tags") # implicit
                    and (ds[2] == proj) # 'PROJ' subdirectory
                    and ds[3].startswith(proj + "_") # version prefix
                    and self.format.match(ds[3][len(proj) + 1:])
                    )
        except:
            return False
        return True

class TagRemoval(TagCheckerBase):
    """
    Tag checkers to allow/restrict removal of tags.
    """
    def __call__(self, txn, path):
        self.log.debug("check %s", path)
        if (txn.change_kind(path) == "D"): # removed directory
            ps = path.strip("/").split("/")
            # only tag directories can be removed
            if len(ps) >= 4:
                proj = ps[0].upper()
                # project
                if (ps[-1].startswith(proj + "_") # PROJECT_...
                    and self.format.match(ps[-1][len(proj) + 1:])): # version number is valid
                    return (True, "Removed project tag")
                elif self.format.match(ps[-1]):
                    return (True, "Removed package tag")
        return (False, "Only tag directories can be removed")

class TagIntermediateDirs(TagCheckerBase):
    """
    Tag checker to validate the creation of tag intermediate directories.
    """
    def __call__(self, txn, path):
        """
        Check if the path is a valid directory in the tags subtree.
        Succeed if it is a valid new directory, fails otherwise.
        """
        self.log.debug("check %s", path)
        if ((txn.change_kind(path) == "A") # new entry
            and (txn.check_path(path) == "dir")): # that is a directory
            # Check that the path make sense
            #  - for a package
            trunk = path.replace("/tags/", "/trunk/")
            if txn.check_path(trunk) == "dir":
                return (True, "Valid package tag intermediate directory")
            #  - for a project (/Proj/tags/PROJECT/PROJECT_version)
            ps = path.strip("/").split("/")
            proj = ps[0].upper()
            if ps[2] != proj:
                return (False, "Invalid project tag intermediate directory")
            if len(ps) > 3:
                if (ps[3].startswith(proj + "_") # PROJECT_...
                    and self.format.match(ps[3][len(proj) + 1:]) # version number is valid
                    and len(ps) == 4): # no subdirectories
                    return (True, "Valid project tag intermediate directory")
            else:
                return (True, "Valid tag intermediate directory")
        return (False, "Invalid tag intermediate directory")

class ValidXml(PathChecker):
    """
    Path checker to validate the content of XML files.

    The implementation is very simple: just check if the XML is syntactically
    correct by trying to parse it.
    """
    def __call__(self, txn, path):
        if txn.change_kind(path) == 'D':
            return (True, "No check on deleted files")
        self.log.debug("check %s", path)
        from xml.dom.minidom import parseString
        try:
            data = txn.file_contents(path)
            parseString(data)
            self.log.debug("Parsable XML")
            return (True, "Parsable XML in '%s'" % path)
        except Exception, x:
            msg = ["Error parsing '%s': %s" % (path, x)]
            try:
                lines = data.splitlines()
                line = lines[x.lineno-1]
                msg.append(line)
                # replace non-spaces with blanks to displace the marker
                # so that tabs are correctly taken into account
                marker = re.sub(r"\S", " ", line[:x.offset]) + "^"
                msg.append(marker)
            except:
                pass
            self.log.debug("Bad XML")
            return (False, "\n".join(msg))

def checkEncoding(fileObj):
    '''
    Check that a file honors the declared encoding (default ASCII for Python 2
    and UTF-8 for Python 3).

    Raises a UnicodeDecodeError in case of decoding problems and LookupError if
    the specified codec does not exists.

    See http://www.python.org/dev/peps/pep-0263/
    '''
    from itertools import islice

    # default encoding
    if sys.version_info[0] <= 2:
        enc = 'ascii'
    else:
        enc = 'utf-8'

    # find the encoding of the file, if specified (in the first two lines)
    enc_exp = re.compile(r"coding[:=]\s*([-\w.]+)")
    for l in islice(fileObj, 2):
        m = enc_exp.search(l)
        if m:
            enc = m.group(1)
            break

    if hasattr(fileObj, 'name'):
        logging.getLogger('checkEncoding').debug('checking encoding %s on %s',
                                                 enc, fileObj.name)
    else:
        logging.getLogger('checkEncoding').debug('checking encoding %s on file object',
                                                 enc)
    # try to read the file with the declared encoding
    fileObj.seek(0)
    codecs.getreader(enc)(fileObj).read()

class ValidPythonEncoding(PathChecker):
    """
    Check that a file honors the declared encoding (default ASCII for Python 2
    and UTF-8 for Python 3).

    See http://www.python.org/dev/peps/pep-0263/
    """
    def __call__(self, txn, path):
        if txn.change_kind(path) == 'D':
            return (True, "No check on deleted files")
        self.log.debug("check %s", path)
        try:
            data = txn.file_contents(path)
            checkEncoding(StringIO(data))
            self.log.debug("Valid Python encoding")
            return (True, "Valid Python encoding in '%s'" % path)
        except LookupError, x:
            self.log.debug("Bad Python encoding, %s", x)
            return (False, "Error parsing '%s': %s" % (path, x))
        except UnicodeDecodeError, x:
            self.log.debug("Bad Python data")
            msg = ["Wrong encoding in file '%s':\n    %s" % (path, x),
                   "Probably you forgot the line '# -*- coding: utf-8 -*-'",
                   "(see http://www.python.org/dev/peps/pep-0263/)"]
            return (False, '\n'.join(msg))
