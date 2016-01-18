## @file GUITree.py
#
# @brief smart TTree replacement.
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-02
#
## @package GUITree
#
# @brief smart TTree replacement.
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-02
#
import ROOT, ctypes, re
from veloview.giantrootfile.type_helper import getTypeFactory
from veloview.giantrootfile import draw_helper

## smart TTree replacement
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2013-05-02
#
# This class exposes the interface of the underlying TTree object, and
# provides access to branches by effectively making the objects used to save
# the data in a branch member variables of the instance.
#
# It also provides improved drawing functionality compared to TTree.
#
# @todo provide a way to switch off branches that are not needed
class Tree:
    ## constructor
    #
    # @param tree       name of tree or TTree instance to wrap
    # @param treetitle  title of tree (use treename if unset)
    # @param branches   (optional) either list of regexes of branches to
    #                   activate (when reading a tree), or dictionary of
    #                   branch name - types (when writing a new tree)
    def __init__(self, tree, treetitle = None, branches = None):
        ## active branches; dictionary branch name - object saven on branch
        self.branches = { }
        ## TTree holding the branch
        self.tree = None
        if None == treetitle: treetitle = tree
        if None == branches:
            # open for reading, get all branches from Tree
            branches = [ '.*']
        if type(branches) == type([]):
            if type(tree) == type(''):
                # open for reading, get matching branches from Tree
                self.tree = ROOT.gROOT.FindObject(tree)
                if not self.tree or not self.tree.InheritsFrom('TTree'):
                    raise Exception('Unsupported type (\'%s\') used for tree!'
                            % str(type(tree)))
                treetitle = self.tree.GetTitle()
            else:
                try:
                    if tree.InheritsFrom('TTree'):
                        self.tree = tree
                        tree = self.tree.GetName()
                except:
                    raise Exception('Unsupported type (\'%s\') used for tree!'
                            % str(type(tree)))
                if type(treetitle) != type(self):
                    # we misuse the second argument when we're cloning
                    # a tree
                    raise Exception('Unsupported type ' \
                            '(\'%s\') used for original Tree!'
                            % str(type(treetitle)))
                self.branches = treetitle.branches
                return
            if None == self.tree:
                raise Exception('Tree \'%s\' not found!' % tree)
            # modify list of branches
            matchlist = [ re.compile(s) for s in branches ]
            bl = self.tree.GetListOfBranches()
            it = bl.MakeIterator()
            ROOT.SetOwnership(it, True)
            b = it.Next()
            while None != b:
                bname = b.GetName()
                match = False
                for m in matchlist:
                    match = None != m.match(bname)
                    if match: break
                if not match: continue
                self.BranchObj(bname)
                b = it.Next()
            del matchlist
            del it
            del b
            del bl
        elif type(branches) == type({}):
            # open tree for writing, create the specified branches
            self.tree = ROOT.TTree(tree, treetitle)
            if type(tree) != type(''):
                raise Exception('Unsupported type (\'%s\') used for tree!' %
                        str(type(tree)))
            for bn in branches:
                self.BranchObj(bn, branches[bn])
        else:
            raise Exception('Unknown type for branches: %s' % \
                    str(type(branches)))

    ## map ctype types to type specifiers for TTree leaves
    __typedictPOD__ = {
        ctypes.c_bool:  'O',
        ctypes.c_int8:  'B', ctypes.c_uint8:    'b',
        ctypes.c_int16: 'S', ctypes.c_uint16:   's',
        ctypes.c_int32: 'I', ctypes.c_uint32:   'i',
        ctypes.c_int64: 'L', ctypes.c_uint64:   'l',
        ctypes.c_float: 'F', ctypes.c_double:   'D'
        }

    ## return the object associated with a branch of the given tree.
    #
    # There are two modes of operation:
    # - addition of a (non-existing) branch to the tree, in this case, the
    #   (C++ data) type of the branch must be specified in typestr
    # - reading an existing branch, in which case typestr must remain None or
    #   unspecified.
    #
    # This method also adds the new branch as a class member, i.e. if you add
    # a branch named 'foo' to a tree you have in a variable t, the
    # corresponding object can be accessed either through t.foo or
    # t.branches['foo'].
    #
    # @param bname              branch name
    # @param typestr    (optional) string specifying the type of the branch to
    #                   be written
    # @returns the object associated with the branch
    def BranchObj(self, bname, typestr = None):
        obj = None
        if None == typestr:
            # ok, branch for reading
            b = self.tree.FindBranch(bname)
            if None == b:
                raise Exception('Unknown branch name \'%s\' in ' \
                        'TTree \'%a\' requested.' % (bname, self.tree.GetName()))
            l = b.GetLeaf(bname)
            if None == l:
                raise Exception('Unknown leaf in branch in \'%s\' in ' \
                        'TTree \'%a\' requested.' % (bname, self.tree.GetName()))
            typestr = l.GetTypeName()
            obj = getTypeFactory(typestr)()
            self.tree.SetBranchAddress(bname, obj)
        else:
            # new branch for writing
            b = self.tree.FindBranch(bname)
            if None != b:
                raise Exception('Branch \'%s\' already exists in ' \
                        'TTree \'%s\'.' % (bname, self.tree.GetName()))
            obj = getTypeFactory(typestr)()
            bcallargs = (bname, obj)
            if isinstance(obj, ctypes._SimpleCData):
                # ok a POD type most likely, so ROOT needs telling what kind of
                # branch it's supposed to create
                if type(obj) not in self.__typedictPOD__:
                    raise Exception('Unknown data type for branch \'%s\' in '\
                            'tree \'%s\'' % (bname, self.tree.GetName()))
                bcallargs += ('%s/%s' % (bname, self.__typedictPOD__[type(obj)]),)
            self.tree.Branch(*bcallargs)
        self.branches[bname] = obj
        return obj

    ## implement attribute lookup
    #
    # @param name       name of attribut
    # @return value of attribute
    #
    # lookup order is:
    # - branch objects (used most often, so these come first)
    # - real instance attributes (allowing us to override TTree methods)
    # - attributes of the underlying tree
    def __getattr__(self, name):
        if 'branches' in self.__dict__ and name in self.__dict__['branches']:
            obj = self.__dict__['branches'][name]
            # for POD branches, we need special treatment
            if isinstance(obj, ctypes._SimpleCData):
                return obj.value
            else:
                return obj
        elif name in self.__dict__:
            return self.__dict__[name]
        else:
            return self.__dict__['tree'].__getattribute__(name)

    ## implement setting attributes
    #
    # @param name       name of attribute
    # @param value      value to be assigned
    #
    # assignment priority is:
    # - branch objects
    # - attributes of the class
    #
    # no support for changing attributes of the underlying tree is given on
    # purpose, this can be circumvented with the tree attribute of the class
    def __setattr__(self, name, value):
        if 'branches' in self.__dict__ and name in self.__dict__['branches']:
            obj = self.__dict__['branches'][name]
            # for POD branches, we need special treatment
            if isinstance(obj, ctypes._SimpleCData):
                obj.value = value
            else:
                # will this call the assignment operator of the underlying C++
                # object which is what we want
                obj = value
        else:
            self.__dict__[name] = value

    ## execute TTree's CloneTree method, wrap the result in a new Tree
    #
    # clones the structure of the tree, and make old and new tree share all
    # branch buffers
    #
    # @param nentries   number of entries to copy (-1 means all)
    # @param options    options for cloning (see TTree documentation)
    # @return   wrapped instance of cloned tree
    def CloneTree(self, nentries = -1, options = ''):
        return Tree(self.tree.CloneTree(nentries, options), self)

    ## helper class to make trees iterable
    #
    # @author Manuel Schiller <manuel.schiller@nikhef.nl>
    # @date 2013-05-06
    class __TreeIter__:
        ## constructor
        def __init__(self, tree):
            self.__tree__ = tree
            self.__entry__ = 0
            self.__nentries__ = tree.GetEntries()

        ## return the iterator
        def __iter__(self):
            return self

        ## return the next entry in the tree
        def next(self):
            if self.__entry__ >= self.__nentries__:
                raise StopIteration()
            self.__tree__.GetEntry(self.__entry__)
            self.__entry__ += 1
            return self.__tree__

    ## make tree iterable
    def __iter__(self):
        return self.__TreeIter__(self)

    ## Check whether a value appears in a specific branch
    def IsPresent(self, branchName, value):
        for entry in self:
            if value == self.__getattr__(branchName):
                return True
        return False

    ## plot Tree contents (see draw_helper.Draw for details)
    Plot = draw_helper.Draw

    ## get list of Tree contents (see draw_helper.Data for details)
    Data = draw_helper.Data

# vim: sw=4:tw=78:ft=python
