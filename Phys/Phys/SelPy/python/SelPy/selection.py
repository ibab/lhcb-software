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
           'UniquelyNamedObject',
           'Selection',
           'EventSelection',
           'flatAlgorithmList',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations')

from SelPy.utils import (flatSelectionList,
                         update_dict_overlap,
                         connectToRequiredSelections,
                         NamedObject,
                         UniquelyNamedObject,
                         NameError,
                         NonEmptyInputLocations,
                         IncompatibleInputLocations)

class AutomaticData(NamedObject) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    >>> SelStdLoosePions = AutomaticData(Location = 'Phys/StdLoosePions/Particles')
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions/Particles'
    >>> SelStdLoosePions.name()
    'Phys_SelStdLoosePions_Particles'
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    def __init__ (self,
                  Location) :

        NamedObject.__init__(self, Location.replace('/', '_'))
        self._location = Location
        self.requiredSelections = []

    def algorithm(self) :
        return None

    def outputLocation(self) :
        return self._location

class Selection(UniquelyNamedObject) :
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

    def __init__(self,
                 name,
                 ConfGenerator,
                 RequiredSelections = [],
                 OutputBranch = "Phys",
                 InputDataSetter = "InputLocations",
                 Extension='Particles') :

        UniquelyNamedObject.__init__(self, name)

        self.__ctor_dict__ = dict(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']
        
        self._alg = ConfGenerator( self.name() ) # get the configurable.
        self.requiredSelections = list(RequiredSelections)

        connectToRequiredSelections(self, InputDataSetter)

        self._outputLocation = self.name()
        if OutputBranch != '' :            
            self._outputLocation = OutputBranch + '/' + self.name()
        if Extension != '' :
            self._outputLocation = self._outputLocation + '/' + Extension
        self._outputLocation.replace('//','/')
        if self._outputLocation.endswith('/') :
            self._outputLocation = self._outputLocation[:self._outputLocation.rfind('/')]
            
    def algorithm(self) :
        return self._alg

    def outputLocation(self) :
        return self._outputLocation

    def clone(self, name, **args) :
        new_dict = update_dict_overlap(self.__ctor_dict__, args)
        return Selection(name, **new_dict)

class EventSelection(UniquelyNamedObject) :
    """
    Selection wrapper class for event selection algorithm configurable
    generator.
    Algorithm produces no output data and is assumed to be correctly
    configured. Only action expected from it is that is selects or
    rejects an event. Can be used just like a Selection object.
    Example:
    from PhysSelPython.Wrappers import EventSelection, SelectionSequence
    from GaudiConfUtils.ConfigurableGenerators import SomeEventSelectionAlg
    evtSel = EventSelection('MyEvtSel',
                            ConfGenerator=SomeEventSelectionAlg(A=1, B=2))
    help(SelectionSequence)
    selSeq = SelectionSequence('MyEvtSelSeq', TopSelection = evtSel)
    """
    def __init__(self,
                 name,
                 ConfGenerator ) :

        UniquelyNamedObject.__init__(self, name)
        self._alg = ConfGenerator(self.name())
        self.requiredSelections = []    

    def algorithm(self) :
        return self._alg

    def outputLocation(self) :
        return ''

class PassThroughSelection(UniquelyNamedObject) :
    """
    Selection wrapper class for event selection algorithm Configurable generator.
    Algorithm produces no output data and is assumed to be correctly
    configured. Only action expected from it is that is selects or
    rejects an event, based on the contents of the outputLocation() of ots
    RequiredSelection. Can be used just like a Selection object.
    Example:
    from PhysSelPython.Wrappers import PassThroughSelection, SelectionSequence
    from GaudiConfutils.ConfigurableGenerators import SomeEventSelectionAlg
    evtSel = PassThorughSelection('MyPassThroughSel',
                                  ConfGenerator=SomeEventSelectionAlg(A=1, B=2, C=3))
    help(SelectionSequence)
    selSeq = SelectionSequence('MyEvtSelSeq', TopSelection = evtSel)
    """
    def __init__(self,
                 name,
                 ConfGenerator ,
                 RequiredSelection,
                 InputDataSetter = None ) :

        UniquelyNamedObject.__init__(self, name)
        self._alg = ConfGenerator(self.name())
        self.requiredSelection  = RequiredSelection 
        self.requiredSelections = [RequiredSelection]

        connectToRequiredSelections(self, InputDataSetter)

    def algorithm(self) :
        return self._alg

    def outputLocation(self) :
        return self.requiredSelection.outputLocation()

class SelSequence(UniquelyNamedObject) :
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

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = []) :

        UniquelyNamedObject.__init__(self, name)

        self.__ctor_dict__ = dict(locals())
        del self.__ctor_dict__['self']
        del self.__ctor_dict__['name']

        self.algos = list(EventPreSelector)
        self._topSelection = TopSelection
        self.algos += flatAlgorithmList(TopSelection)
        self.algos += PostSelectionAlgs

    def algorithm(self) :
        return self._topSelection.algorithm()
        
    def outputLocation(self) :
        return self._topSelection.outputLocation()

    def outputLocations(self) :
        return [self.outputLocation()]

    def clone(self, name, **args) :
        new_dict = update_dict_overlap(self.__ctor_dict__, args)
        return SelSequence(name, **new_dict)

    def selection(self) :
        """
        Return the sequence's top Selection.
        """
        return self._topSelection

    def __getitem__(self, index) :
        return self.algos[index]
    
    
def flatAlgorithmList(selection) :
    """
    Return a flat list with all the necessary algorithms for
    the selection to run.
    """
    _selList = flatSelectionList(selection)
    return  [sel.algorithm() for sel in _selList]



