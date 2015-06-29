"""This module implements an interface to GiantRootFileIO"""

from ..utils.rootutils import ROOT
from ..utils.utils import flatten, unflatten
from veloview.giantrootfile.gui_tree import Tree
import os

class GRFIO(object):
    """Interface for versioned objects implemented by GiantRootFileIO."""

    def __init__(self, fname, mode = 'read', treeName = 'DQTree', title ='',  branches = None):
        """
        Instantiate a new GRFIO object for interfacing with an (un)versioned
        ROOT tree.

        fname    -- The ROOT file with versioned objects
        mode     -- Mode to open the file (create/new, read, update)
        treeName -- Name of tree with versioned objects
        branches -- List of branches, or dictionary of name and types
        """

        if not isinstance(fname, str):
            raise ValueError('File name must be a string')

        self.mode = mode.lower()
        if self.mode == 'new':
            self.mode = 'create' #internally store 'new' as 'create' for easier parsing

        if self.mode not in ['create', 'read', 'update']:
            raise ValueError('File mode {} not allowed, must be one of create, new, read or update'.format(self.mode))

        if self.mode == 'create':
            if not isinstance(branches, dict):
                raise ValueError('Branch name-type dictionary mandatory for creation')
            if os.path.isfile(fname):
                raise ValueError("Can't create existing file {}".format(fname))

        self._open_file(fname, self.mode)
        self._open_tree(treeName, title, flatten(branches) if branches else None)

    def _open_file(self, fname, mode):
        """Internal method to open file"""
        #self.lock = ROOT.DotLock(fname)
        self.rfile = ROOT.TFile.Open(fname, mode)
        if not self.rfile:
            raise IOError("Giant ROOT file {} not found".format(fname))

    def _open_tree(self, treeName, title, branches):
        """Internal method to open tree"""
        self.title = title
        self.branches = branches
        if self.mode == 'create':
            try:
                self.tree = Tree(treeName, treetitle = title, branches = branches)
            except TypeError:
                raise ValueError('Cannot find valid tree: {}'.format(treeName))
        else:
            getTree = self.rfile.Get(treeName)
            self.tree = Tree(treeName)

    @classmethod
    def __getBranch(cls, tree, branchname):
        """
        Returns a tuple (isVersioned, branch), where isVersioned is a bool
        specifiying whether the branch is versioned, and branch is the branch
        of the specified tree corresponding to the branch name.
        """
        branch = getattr(tree, branchname)
        return (hasattr(branch, 'value'), branch)

    def fill(self, dqdict):
        """Flatten and fill dictionary"""
        if self.mode == 'read':
            raise RuntimeError('Cannot fill GRF in READ mode.')
        self._fill(self.tree, dqdict)

    def _fill(self, tree, dqdict):
        """
        Internal method: flatten and fill dictionary

        FIXME: doesn't check if branches match type'
        """
        now = ROOT.TimeStamp()
        dqflat = flatten(dqdict)
        # FIXME: verify dqflat matches branch scheme
        for key, value in dqflat.iteritems():
            isVersioned, branch = self.__class__.__getBranch(tree, key)
            if isVersioned:
                branch[now] = value
            else:
                setattr(tree, key, value)
        tree.Fill()

    def read(self, branches, version = None):
        """Read branches and return unflattened dictionaries."""
        res = {}
        for br in branches:
            isVersioned, branch = self.__class__.__getBranch(self.tree, br)
            # FIXME: check if requested version is valid
            if version and isVersioned:
                value = branch[version]
            else:
                value = branch

            if isVersioned:
                res[br] = value.value()
            else:
                res[br] = value
        return unflatten(res)

    def edit(self, entry_p, dqdict):
        if self.mode == 'read':
            raise RuntimeError('Cannot edit GRF in READ mode.')

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
        #del self.lock
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
