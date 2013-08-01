'''
Core module of SvnChecker.

Contains the foundation classes for the algebra of checkers, a class to simulate
transactions in the tests and the 'run' function (the core of a check script).

Created on Nov 24, 2010
'''
__author__ = "Marco Clemencic <marco.clemencic@cern.ch>"

__all__ = ("Transaction",
           "Checker",
           "And",
           "Or",
           "Not",
           "Success",
           "Failure",
           "run")

from svn.core import svn_path_canonicalize, svn_stream_read, svn_stream_close
from svn.repos import svn_repos_fs, svn_repos_open
from svn.fs import svn_fs_open_txn, svn_fs_txn_root, svn_fs_paths_changed
from svn.fs import svn_fs_copied_from, svn_fs_check_path, svn_fs_txn_prop, svn_fs_node_prop
from svn.fs import svn_fs_file_contents, svn_fs_file_length

import logging

class Transaction(object):
    """
    Simple class to wrap some informations about a Subversion transaction.
    """
    def __init__(self, repos, txn):
        """
        Initialize with the repository path and the transaction id (as strings).

        These are usually the arguments passed to the svn hooks.
        """
        self._repos_path = svn_path_canonicalize(repos)
        self._txn_name = txn

        self._repos = None
        self._txn = None
        self._root = None
        self._changes = None

    @property
    def repos(self):
        """
        Repository instance.
        """
        if self._repos is None:
            self._repos = svn_repos_fs(svn_repos_open(self._repos_path))
        return self._repos
    @property
    def txn(self):
        """
        Transaction instance.
        """
        if self._txn is None:
            self._txn = svn_fs_open_txn(self.repos, self._txn_name)
        return self._txn
    @property
    def root(self):
        """
        Root of the transaction.
        """
        if self._root is None:
            self._root = svn_fs_txn_root(self.txn)
        return self._root
    @property
    def changes(self):
        """
        Hash table of the changes in the transaction.
        """
        if self._changes is None:
            self._changes = svn_fs_paths_changed(self.root)
        return self._changes

    def change_kind(self, path):
        change = self.changes[path]
        return "MADR"[change.change_kind]

    def copied_from(self, path):
        """
        Tell the source of a file that is copied in the transaction.
        """
        return svn_fs_copied_from(self.root, path)

    def check_path(self, path):
        """
        Return the type of entry pointed to by the path 'p'.
        """
        return [None, "file", "dir", "unknown"][svn_fs_check_path(self.root, path)]

    def __getitem__(self, name):
        """
        Return the value of a given property.
        """
        return svn_fs_txn_prop(self.txn, name)

    def node_property(self, path, propname):
        """
        Return the value of a property of a path.
        """
        return svn_fs_node_prop(self.root, path, propname)

    def file_contents(self, path):
        """
        Return a string with the content of a file.
        """
        contents = svn_fs_file_contents(self.root, path)
        length = int(svn_fs_file_length(self.root, path))
        data = svn_stream_read(contents, length)
        svn_stream_close(contents)
        return data

    def formatChange(self, path):
        """
        Return a string representing the change in a file.
        """
        k = self.change_kind(path)
        s = "%s %s" % (k, path)
        if k != "D":
            id_n, src = self.copied_from(path)
            if src:
                s += " (from %s:%d)" % (src, id_n)
        return s

    def __str__(self):
        """String representation of a transaction.
        Return a string with one line per path in the transaction.
        """
        return "\n".join(map(self.formatChange, self.changes))

class FakeTransaction(object):
    """
    Emulates the transaction API for testing.
    """

    def __init__(self, changes = {}, base = {}, properties = {}, node_properties = {}, files = {}):
        """
        The argument 'changes' must be a dictionary of changes in the form of
          {path: (kind, (id, src), type), ...}
        E.g.:
          {'/Project/tags/Package/v1r0': ('A', (1234, '/Project/trunk/Package'), 'dir'),
           '/Project/tags/PROJECT/PROJECT_v1r1': ('A', (-1, None), 'dir'),
           '/Project/trunk/cmt/requirements': ('M', (-1, None), 'file')}

        'base' must be a dictionary representing the base on which the changes are applied.
        E.g.:
          {'/Project/trunk/Package': 'dir'}

        'properties' is a dictionary of the properties specific to the transaction itself
        (like the 'svn:author).

        'node_properties' is a dictionary of properties specific to nodes.
        E.g.:
          {'/Project/trunk/Package': {'prop': 'value'}}

        'files' is used to specify the content of files for the tests that require it.
        """
        self.changes = changes
        self.base = base
        self.properties = properties
        self.node_properties = node_properties
        self.files = files

    def change_kind(self, path):
        return self.changes[path][0]

    def copied_from(self, path):
        return self.changes[path][1]

    def check_path(self, path):
        if path not in self.changes:
            return self.base.get(path)
        else:
            return self.changes[path][2]

    def __getitem__(self, name):
        return self.properties.get(name)

    def __repr__(self):
        return "%s(%r, %r, %r)" % (self.__class__.__name__, self.changes, self.base, self.properties)

    def node_property(self, path, propname):
        """
        Return the value of a property of a path.
        """
        return self.node_properties[path].get(propname)

    def file_contents(self, path):
        """
        Return a string with the content of a file.
        """
        return self.files[path]

    def formatChange(self, path):
        """
        Return a string representing the change in a file.
        """
        k = self.change_kind(path)
        s = "%s %s" % (k, path)
        if k != "D":
            id_, src = self.copied_from(path)
            if src:
                s += " (from %s:%d)" % (src, id_)
        return s

    def __str__(self):
        return "\n".join(map(self.formatChange, sorted(self.changes)))

class Checker(object):
    """
    Base class for a checker object.
    It defines the minimal API and a default behavior.
    """
    def __init__(self):
        self.log = logging.getLogger(self.__class__.__name__)

    def __call__(self, txn):
        """
        Validate the transaction.

        Must be overridden and the implementation must return a tuple
          (boolean, message)
        where the boolean tells if the transaction can be accepted and the message
        tells why the transaction was not good.
        """
        raise RuntimeError("Method not implemented")

    def __add__(self, other):
        """
        '+' operator, implements the 'or' semantics.
        """
        return Or(self, other)

    def __mul__(self, other):
        """
        '*' operator, implements the 'and' semantics.
        """
        return And(self, other)

    def __neg__(self):
        """
        '-' unary operator, implements the 'not' semantics.
        """
        return Not(self)

    def __repr__(self):
        raise RuntimeError("Method not implemented")
        return "%s()" % self.__class__.__name__

class PathChecker(Checker):
    def __call__(self, txn, path):
        """
        Validate the transaction.

        Must be overridden and the implementation must return a tuple
          (boolean, message)
        where the boolean tells if the transaction can be accepted and the message
        tells why the transaction was not good.

        The path argument tells which path in the transaction is to be checked.
        """
        raise RuntimeError("Method not implemented")

class Or(Checker):
    """
    Checker implementing the 'or' logical combination of checkers.
    """
    def __init__(self, a, b):
        super(Or, self).__init__()
        self._operands = (a, b)
    def __call__(self, *args):
        self.log.debug("Start OR block")
        msgs = []
        for o in self._operands:
            r, m = o(*args)
            if r:
                msgs = [m] # keep only the success message
                break # stop checking on the first success
            else:
                msgs.append(m) # record all the failure messages
        self.log.debug("End OR block (%s)", r)
        return (r, '\n'.join(msgs))
    def __repr__(self):
        return "%s(%s)" % (self.__class__.__name__, ", ".join(map(repr, self._operands)))

class And(Checker):
    """
    Checker implementing the 'and' logical combination of checkers.
    """
    def __init__(self, a, b):
        super(And, self).__init__()
        self._operands = (a, b)
    def __call__(self, *args):
        self.log.debug("Start AND block")
        for o in self._operands:
            r, m = o(*args)
            if not r:
                break # stop checking on the first failure
        self.log.debug("End AND block (%s)", r)
        return (r, m)
    def __repr__(self):
        return "%s(%s)" % (self.__class__.__name__, ", ".join(map(repr, self._operands)))

class Not(Checker):
    """
    Checker implementing the 'not' (negation) logical operation on checkers.
    """
    def __init__(self, a):
        super(Not, self).__init__()
        self._operand = a
    def __call__(self, *args):
        r, m = self._operand(*args)
        self.log.debug("Inverse result %s -> %s", r, not r)
        return (not r, m)
    def __repr__(self):
        return "%s(%r)" % (self.__class__.__name__, self._operand)

class Success(Checker):
    """
    Constant checker always successful.
    A message can be passed to the constructor to be reported in the result.
    """
    def __init__(self, msg = ""):
        super(Success, self).__init__()
        self.msg = msg
    def __call__(self, *args):
        self.log.debug("Forced success")
        return (True, self.msg)
    def __repr__(self):
        return "%s(%r)" % (self.__class__.__name__, self.msg)

class Failure(Checker):
    """
    Constant checker always failing.
    A message can be passed to the constructor to be reported in the result.
    """
    def __init__(self, msg = ""):
        super(Failure, self).__init__()
        self.msg = msg
    def __call__(self, *args):
        self.log.debug("Forced failure")
        return (False, self.msg)
    def __repr__(self):
        return "%s(%r)" % (self.__class__.__name__, self.msg)

class Rephrase(Checker):
    """
    Checker to override the message returned by a checker, keeping the same
    result.
    """
    def __init__(self, a, msg):
        super(Rephrase, self).__init__()
        self._operand = a
        self._msg = msg
    def __call__(self, *args):
        r, m = self._operand(*args)
        self.log.debug("Rephrase '%s' -> '%s'", m, self._msg)
        return (r, self._msg)
    def __repr__(self):
        return "%s(%r, %r)" % (self.__class__.__name__,
                               self._operand, self._msg)

def run(checker, argv = None):
    """
    Main function for checker scripts.
    """
    import sys
    if argv == None:
        argv = sys.argv
    # create the transaction
    txn = Transaction(sys.argv[1], sys.argv[2])
    # check
    res, msg = checker(txn)
    if not res:
        sys.stderr.write("\nFailure: %s\n\nTransaction:\n%s" % (msg, txn))
        sys.exit(1)
