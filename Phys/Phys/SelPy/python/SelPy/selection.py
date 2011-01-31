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

__all__ = ('AutomaticData',
           'AutoData',
           'Selection',
           'FlatSelectionListBuilder',
           'flatAlgorithmList',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations',
           'update_overlap')

from SelPy.utils import flatSelectionList, removeDuplicates

class AutoData(object) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    >>> SelStdLoosePions = AutoData(Location = 'Phys/StdLoosePions/Particles')
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions/Particles'
    >>> SelStdLoosePions.name()
    'Phys_SelStdLoosePions_Particles'
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    def __init__ (self,
                  Location) :

        self._name = Location.replace('/', '_')
        self._location = Location
        self.requiredSelections = []

    def name(self) :
        return self._name

    def algorithm(self) :
        return None

    def outputLocation(self) :
        return self._location

AutomaticData = AutoData

class Selection(object) :
    """
    Wrapper class for offline selection. Takes a top selection
    Configurable Generator plus a list of required selection configurables.
    It uses the required selections to set the list of input data locations
    of the top selection. The method used to set the input data locations is
    defined by the 'InputDataSetter' constructor argument
    (default 'InputLocations').
    Makes the output location of the data available via outputLocation(),
    a concatenation of

       OutputBranch: default = 'Phys'
       name
       Extension : default = 'Particles'

    Example: selection for A -> B(bb), C(cc)

       # create and configure selection algorithm configurable:
       from GaudiConfUtils.ConfigurableGenerators import CombineParticles
       A2B2bbC2cc = CombineParticles()
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
                                 ConfGenerator = A2B2bbC2cc,
                                 RequiredSelections = [SelB, SelC])
       print SelA2BsbbC2cc.outputLocation()
       'Phys/SelA2B2bbC2cc/Particles'
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    __used_names = []

    def __init__(self,
                 name,
                 ConfGenerator,
                 RequiredSelections = [],
                 OutputBranch = "Phys",
                 InputDataSetter = "InputLocations",
                 Extension='Particles') :

        if name in Selection.__used_names :
            raise NameError('Selection name ' + name + ' has already been used. Pick a new one.')

        self.__ctor_dict__ = dict(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']
        
        self.requiredSelections = []
        for sel in RequiredSelections :
            self.requiredSelections.append(sel)
        self._name = name
        _outputLocations = [sel.outputLocation() for sel in self.requiredSelections]
        _outputLocations = filter(lambda s : s != '', _outputLocations)
        if hasattr(ConfGenerator, InputDataSetter) :
            _inputLocations = getattr(ConfGenerator, InputDataSetter)
            if len(_inputLocations) != 0 :
                if not compatibleSequences(_outputLocations,
                                           _inputLocations) :
                    raise IncompatibleInputLocations('InputLocations of input algorithm incompatible with RequiredSelections!'\
                                                     '\nInputLocations: '+str(_inputLocations)+\
                                                     '\nRequiredSelections: '+str(_outputLocations))
        self.alg = ConfGenerator(self._name) # get the configurable.
        self.alg.__setattr__(InputDataSetter, list(_outputLocations))
        self._outputBranch = OutputBranch
        self._outputLocation = (self._outputBranch + '/' + self.name() + '/' + Extension).replace('//','/')

        Selection.__used_names.append(name)
        
    def name(self) :
        return self._name

    def algorithm(self) :
        return self.alg

    def outputLocation(self) :
        return self._outputLocation

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
        self.__ctor_dict__ = dict(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']

        self.algos = list(EventPreSelector)
        self._name = name
        self._topSelection = TopSelection
        self.algos += flatAlgorithmList(TopSelection)
        self.algos += PostSelectionAlgs
        
    def name(self) :
        return self._name

    def algorithm(self) :
        return self._topSelection.algorithm()
        
    def outputLocation(self) :
        return self._topSelection.outputLocation()

    def outputLocations(self) :
        return [self.outputLocation()]

    def clone(self, name, **args) :
        new_dict = update_overlap(self.__ctor_dict__, args)
        return SelSequence(name, **new_dict)

    def selection(self) :
        """
        Return the sequence's top Selection.
        """
        return self._topSelection

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

        print 'Warning: FlatSelectionListBuilder deprecated. Please use flatAlgorithmList function instead.'
        _alg = TopSelection.algorithm()
        self.selectionList = []
        if (_alg != None) :
            _selList = flatSelectionList(TopSelection)
            _selList = filter(lambda a : type(a) != AutoData, _selList)
            self.selectionList = [sel.algorithm() for sel in _selList]
            self.selectionList = removeDuplicates(self.selectionList)
    
def flatAlgorithmList(selection) :
    """
    Return a flat list with all the necessary algorithms for
    the selection to run.
    """
    _selList = flatSelectionList(selection)
    return  [sel.algorithm() for sel in _selList]

def update_overlap(dict0, dict1) :
    """
    Replace entries from dict0 with those from dict1 that have
    keys present in dict0.
    """
    overlap_keys = filter(dict1.has_key, dict0.keys())
    result = dict(dict0)
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
