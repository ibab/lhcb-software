"""This module implements an interface to GiantRootFileIO"""

from ..utils.rootutils import ROOT
from ..GiantRootFileIO.GUITree import Tree

def flatten(pydict):
    from copy import deepcopy
    pydict = deepcopy(pydict)
    if not isinstance(pydict, dict):
        raise ValueError('Only dictionaries are supported: {}'.format(type(pydict)))
    children = {}
    for key in pydict:
        if isinstance(pydict[key], dict):
            children[key] = flatten(pydict[key])
    for key, value in children.iteritems():
        pydict.update([('.'.join([key,k]), v) for k,v in value.iteritems()])
        del pydict[key]
    return pydict

def unflatten(pydict):
    from copy import deepcopy
    pydict = deepcopy(pydict)
    if not isinstance(pydict, dict):
        raise ValueError('Only dictionaries are supported: {}'.format(type(pydict)))
    def nestkey(key, value):
        keys = key.split('.')
        res = {}
        for k in reversed(keys):
            res = {k: value}
            value = res
        return res
    def mergedicts(res, new):
        for k in new:
            if res.has_key(k):
                mergedicts(res[k], new[k])
            else:
                res.update({k: new[k]})
    rows = [nestkey(key, pydict[key]) for key in pydict]
    res = {}
    for row in rows:
        mergedicts(res, row)
    return res

class GRFIO(object):
    """interface for versioned objects implemented by GiantRootFileIO."""

    def __init__(self, fname, mode='read', tree='DQTree', title='', branches = None):
        """Instantiating GRFIO needs the filename, mode, and tree name.

        fname    -- The ROOT file with versioned objects
        mode     -- Mode to open the file (create/new, read, update)
        tree     -- Name of tree with versioned objects
        branches -- List of branches, or dictionary of name and types

        """
        self.mode = mode.lower()
        if self.mode == 'recreate':
            raise ValueError('recreating not allowed')
        if ((self.mode == 'create' or self.mode == 'new') and
            not isinstance(branches, dict)):
            raise ValueError('Branch name-type dictionary mandatory for creation')
        self._open_file(fname, self.mode)
        self._open_tree(tree, title, branches)

    def _open_file(self, fname, mode):
        """Internal method to open file"""
        self.lock = ROOT.DotLock(fname)
        self.rfile = ROOT.TFile.Open(fname, mode)

    def _open_tree(self, tree, title, branches):
        """Internal method to open tree"""
        self.title = title
        self.branches = branches
        testtree = self.rfile.Get(tree)
        try:
            self.tree = Tree(tree, treetitle = title, branches = branches)
        except TypeError:
            raise ValueError('Cannot find valid tree: {}'.format(tree))

    @staticmethod
    def if_versioned(tree, branchname):
        """Check if branch is versioned"""
        branch = getattr(tree, branchname)
        return (hasattr(branch, 'value'), branch)

    def fill(self, dqdict):
        """Flatten and fill dictionary"""
        if 'read' == self.mode:
            raise RuntimeError('Cannot fill GRF in READ mode.')
        self._fill(self.tree, dqdict)

    def _fill(self, tree, dqdict):
        """Internal method: flatten and fill dictionary

        FIXME: doesn't check if branches match type'

        """
        now = ROOT.TimeStamp()
        dqflat = flatten(dqdict)
        # FIXME: verify dqflat matches branch scheme
        for key, value in dqflat.iteritems():
            versioned, branch = self.if_versioned(tree, key)
            if versioned: branch[now] = value
            else: setattr(tree, key, value)
        tree.Fill()

    def read(self, branches, version = None):
        """Read branches and return unflattened dictionaries."""
        res = {}
        for br in branches:
            versioned, branch = self.if_versioned(self.tree, br)
            # FIXME: check if requested version is valid
            if version and versioned: value = branch[version]
            else: value = branch
            if versioned: res[br] = value.value()
            else: res[br] = value
        return unflatten(res)

    def edit(self, entry_p, dqdict):
        if 'read' == self.mode:
            raise RuntimeError('Cannot fill GRF in READ mode.')

        newfilename = '{}.new'.format(self.rfile.GetName())
        newlock = ROOT.DotLock(newfilename)
        newfile = ROOT.TFile.Open(newfilename, 'recreate')
        newfile.SetCompressionSettings(self.rfile.GetCompressionSettings())
        newtree = self.tree.CloneTree(0)

        for dummy in self.tree:
            if entry_p(self.tree): # new
                self._fill(newtree, dqdict)
            else:               # old
                self._fill(newtree, self.read([br for br in dqdict]))
        newtree.Write()
        treename = newtree.GetName()

        del newtree
        newfile.Close()
        del newfile
        filename = self.rfile.GetName()
        del self.tree
        self.rfile.Close()
        del self.rfile

        from os import rename
        rename(newfilename, filename)
        del self.lock
        del newlock

        self.mode = 'update'
        self._open_file(filename, self.mode)
        self._open_tree(treename, self.title, [br for br in self.branches])

    def write(self):
        """Write tree to disk"""
        self.tree.Write()

    def close(self):
        """Close ROOT file"""
        self.rfile.Close()
