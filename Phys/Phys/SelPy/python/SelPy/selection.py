#$Id: selection.py,v 1.12 2010/02/25 14:09:30 jpalac Exp $
"""
Classes for a DaVinci offline physics selection. The following classes
are available:
   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataOnDemand       Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand',
           'AutomaticData',
           'Selection',
           'FlatSelectionListBuilder',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations',
           'update_overlap')

from copy import copy

class DataOnDemand(object) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    >>> SelStdLoosePions = DataOnDemand('SelStdLoosePions',
                                        Location = 'Phys/StdLoosePions')
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions'
    >>> SelStdLoosePions.name()
    'SelStdLoosePions'

    The first argument is used for the name, but can be omitted:

    >>> SelStdLoosePions = DataOnDemand(Location = 'Phys/StdLoosePions')
    >>> SelStdLoosePions.name()
    'StdLoosePions'
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    def __init__ (self,
                  name='',
                  Location = "", 
                  RequiredSelections = [] ) :
        if name == '' :
            self._name = Location[Location.rfind('/')+1:]
            print 'setting name to ', self._name
        else :
            self._name = name
        self.requiredSelections = []
        self._location = Location

    def name(self) :
        return self._name

    def algorithm(self) :
        return None

    def __apply_configuration__(self) :
        print self.name(), ".__apply_configuration(): NOT A CONFIGURABLE."
        
    def outputLocation(self) :
        return self._location

    def algName(self) :
        loc = self.outputLocation()
        loc = loc[loc.rfind("/")+1:] # grab the last string after the last '/'
        return loc

AutomaticData = DataOnDemand
    
class Selection(object) :
    """
    Wrapper class for offline selection. Takes a top selection DVAlgorithm
    configurable plus a list of required selection configurables. It uses
    the required selections to set the InputLocations of the top selection.
    Makes the output location of the data available via outputLocation(),
    a concatenation of the OutputBranch and Algorithm's name.

    Example: selection for A -> B(bb), C(cc)

    # create and configure selection algorithm configurable:
    A2B2bbC2cc = CombineParticles('A2B2bbC2cc')
    A2B2bbC2cc.DecayDescriptor = ...
    A2B2bbC2cc.MotherCut = ...
    # now assume SelB and SelC have been defined in modules B2bb and C2cc
    # respectively
    from B2bb import SelB
    from C2cc import SelC
    # now create a Selection instance using the B and C selections.
    # Output will go to 'Phys' + A2B2bbC2cc.name()
    from PhysSelPython.Wrappers import Selection
    SelA2B2bbC2cc = Selection('SelA2B2bbC2cc',
                              Algorithm = A2B2bbC2cc,
                              RequiredSelections = [SelB, SelC],
                              OutputBranch = 'Phys')
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    __used_names = []

    def __init__(self,
                 name,
                 Algorithm,
                 RequiredSelections = [],
                 OutputBranch = "Phys") :

        if name in Selection.__used_names :
            raise NameError('Selection name ' + name + ' has already been used. Pick a new one.')
        Selection.__used_names.append(name)
        self.__ctor_dict__ = copy(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']
        
        self.requiredSelections = []
        print "Instantiating Selection", name
        for sel in RequiredSelections :
            print "Selection: Adding Required Selection ", sel.name()
            self.requiredSelections.append(sel)
        self._name = name
        _outputLocations = [sel.outputLocation() for sel in self.requiredSelections]
        if len(Algorithm.InputLocations) != 0 :
            if not compatibleSequences(_outputLocations,
                                       Algorithm.InputLocations) :
                raise IncompatibleInputLocations('InputLocations of input algorithm incompatible with RequiredSelections!'\
                                                 '\nInputLocations: '+str(Algorithm.InputLocations)+\
                                                 '\nRequiredSelections: '+str(_outputLocations))
        self.alg = Algorithm.clone(self._name, InputLocations = [])
        print "Selection: cloned", type(self.alg) , Algorithm.name(), "to", self.alg.name()
        self._outputBranch = OutputBranch
        
        for loc in _outputLocations :
            print "\tAlgo ", self.algName(),  ": adding InputLocation ", loc
            self.algorithm().InputLocations += [loc]
        print self._name, "Required Selection Algorithms: ", self.requiredSelections

    def name(self) :
        return self._name

    def __apply_configuration__(self) :
        print self.name(), ".__apply_configuration(): NOT A CONFIGURABLE."

    def algorithm(self) :
        return self.alg

    def algName(self) :
        return self.algorithm().name()

    def outputLocation(self) :
        return self._outputBranch + "/" + self.algName()

    def clone(self, name, **args) :
        new_dict = update_overlap(self.__ctor_dict__, args)
        return Selection(name, **new_dict)

class SelSequence(object) :
    """
    Class for offline selection sequence. Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to form a flat list with all the required
    selections needed to run the top selection. Can add list of event selection
    algorithms to be added at the beginning of the sequence, and a list of
    algorithms to be run straight after the selection algoritms.

    Example: selection sequence for A -> B(bb), C(cc). Add pre-selectors alg0
             and alg1, and counter counter0.

    # Assume module A2B2bbC2cc defining a Selection object for the decay
    # A -> B(bb), C(cc)
    from A2B2bbC2cc import SelA2B2bbC2cc
    from PhysSelPython.Wrappers import SelSequence
    SeqA2B2bbC2cc = SelSequence('SeqA2B2bbC2cc',
                                TopSelection = SelA2B2bbC2cc,
                                EventPreSelector = [alg0, alg1],
                                PostSelectionAlgs = [counter0])
    # use it
    mySelList = SeqA2B2bbC2cc.algos
    print mySelList
    dv = DaVinci()
    dv.UserAlgorithms = [mySelSeq]
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    __used_names = []

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = []) :

        if name in SelSequence.__used_names :
            raise NameError('SelSequence name ' + name + ' has already been used. Pick a new one.')
        SelSequence.__used_names.append(name)
        self.__ctor_dict__ = copy(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']

        self.algos = copy(EventPreSelector)
        self._name = name
        self._topSelection = TopSelection
        self.algos += FlatSelectionListBuilder(TopSelection).selectionList
        self.algos += PostSelectionAlgs
        
    def name(self) :
        return self._name

    def algorithm(self) :
        return self._topSelection.algorithm()
        
    def algName(self) :
        return self.algorithm().name()

    def outputLocation(self) :
        return self._topSelection.outputLocation()

    def outputLocations(self) :
        return [self.outputLocation()]

    def clone(self, name, **args) :
        new_dict = update_overlap(self.__ctor_dict__, args)
        return SelSequence(name, **new_dict)

    def __getitem__(self, index) :
        return self.algos[index]
    
class FlatSelectionListBuilder(object) :
    """
    Builds a flat selection list. Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to make a flat list with all the required
    selecitons needed to run the top selection. 

    Example: selection sequence for A -> B(bb), C(cc). Add pre-selectors alg0
             and alg1, and counter counter0.

    # Assume module A2B2bbC2cc defining a Selection object for the decay
    # A -> B(bb), C(cc)
    from A2B2bbC2cc import SelA2B2bbC2cc
    from PhysSelPython.selection import FlatSelectionListBuilder
    SeqA2B2bbC2cc = FlatSelectionListBuilder('SeqA2B2bbC2cc',
                                             TopSelection = SelA2B2bbC2cc)
    # use it
    mySelList = SeqA2B2bbC2cc.selectionList()
    print mySelList
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 TopSelection) :

        _alg = TopSelection.algorithm()
        self.selectionList = [_alg]
        if (_alg != None) :
            self._buildSelectionList( TopSelection.requiredSelections )
            self.selectionList.reverse()
            self.selectionList = remove_duplicates(self.selectionList)
    def _buildSelectionList(self, selections) :
        for sel in selections :
            if type(sel) == DataOnDemand :
                print "DataOnDemand: do nothing"
            else :
                self.selectionList.append(sel.algorithm())
                self._buildSelectionList( sel.requiredSelections )


def remove_duplicates(alg_list) :
    '''
    Remove all but the first instance of each algorithm from the list.
    '''
    clean_list = []
    for alg in alg_list :
        if alg not in clean_list :
            clean_list.append(alg)
    return clean_list

def update_overlap(dict0, dict1) :
    '''
    Replace entries from dict0 with those from dict1 that have
    keys present in dict0.
    '''
    overlap_keys = filter(dict1.has_key, dict0.keys())
    result = copy(dict0)
    for key in overlap_keys : result[key] = dict1[key]
    return result

def compatibleSequences( seq0, seq1) :
    if len(seq0) != len(seq1) :
        return False
    for x in seq0 :
        if x not in seq1 :
            return False
    return True



class NameError(Exception) :
    pass

class NonEmptyInputLocations(Exception) :
    pass
class IncompatibleInputLocations(Exception) :
    pass
