#$Id: Wrappers.py,v 1.44 2010-05-27 10:16:18 jpalac Exp $
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:
   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataOnDemand       Wraps a string TES location to make it look like a Seleciton
   - AutomaticData      Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand',
           'AutomaticData',
           'Selection',
           'SelectionSequence',
           'MultiSelectionSequence',
           'SelSequence',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations'
           )

from copy import copy

_sequencerType = None

try :
    from Gaudi.Configuration import *
    from Configurables import GaudiSequencer
    _sequencerType = GaudiSequencer
except :
    print 'WARNING: Gaudi Configurables not found. No default sequencer type defined.'

from SelPy.selection import ( Selection,
                              FlatSelectionListBuilder,
                              AutomaticData,
                              NameError,
                              NonEmptyInputLocations,
                              IncompatibleInputLocations,
                              update_overlap,
                              SelSequence                )

from SelPy.selection import AutomaticData as autodata

from Configurables import LoKi__VoidFilter as VoidFilter


class AutomaticData(autodata) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    >>> SelStdLoosePions = AutomaticData(Location = 'Phys/StdLoosePions')
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions'
    >>> SelStdLoosePions.name()
    'SelStdLoosePions'
    >>> SelStdLoosePions.outputLocation()
    'Phys/StdLoosePions'
    """

    def __init__(self,
                 Location = '',
                 DataType = 'Particles') :
        if Location == '' :
            raise Exception('Empty AutomaticData Location')
        autodata.__init__(self,
                          name='',
                          Location=Location)
        self._dataType = DataType
    def algorithm(self) :
        return VoidFilter('SelFilter'+self._name,
                          Code = "CONTAINS('"+self.outputLocation()+"/"+self._dataType+"')>0")

    def algName(self) :
        return self.algorithm().name()

DataOnDemand = AutomaticData

class SelectionSequence(SelSequence) :
    """
    Wrapper class for offline selection sequence creation.
    Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to form a GaudiSequencer with all the required
    selections needed to run the top selection. Can add list of event selection
    algorithms to be added at the beginning of the sequence, and a list of
    algorithms to be run straight after the selection algoritms.
    Wraps SelSequence, simply adding a method sequence() that creates the
    GaudiSequencer on-demand.
    Example: selection sequence for A -> B(bb), C(cc). Add pre-selectors alg0
             and alg1, and counter counter0.

    # Assume module A2B2bbC2cc defining a Selection object for the decay
    # A -> B(bb), C(cc)
    from A2B2bbC2cc import SelA2B2bbC2cc
    from PhysSelPython.Wrappers import SelectionSequence
    SeqA2B2bbC2cc = SelectionSequence( 'SeqA2B2bbC2cc',
                                       TopSelection = SelA2B2bbC2cc,
                                       EventPreSelector = [alg0, alg1],
                                       PostSelectionAlgs = [counter0]   )
    # use it
    mySelSeq = SeqA2B2bbC2cc.sequence()
    dv = DaVinci()
    dv.UserAlgorithms = [mySelSeq]

    Uses selection.SelSequence and selection.FlatSelectionListBuilder
    help(SelSequence)
    help(FlatSelectionListBuilder)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = []) :
        SelSequence.__init__(self,
                             name,
                             TopSelection,
                             EventPreSelector,
                             PostSelectionAlgs)

        self.gaudiseq = None

    def sequence(self, sequencerType = _sequencerType) :
        if self.gaudiseq == None :
            self.gaudiseq = sequencerType(self.name(), Members = self.algos)
        return self.gaudiseq

    def clone(self, name, **args) :
        new_dict = update_overlap(self.__ctor_dict__, args)
        return SelectionSequence(name, **new_dict)

class MultiSelectionSequence(object) :
    """
    Wrapper class for offline multiple selection sequence creation.
    Takes a list of SelectionSequence objects and produces on demand a
    sequence with an OR of the list of sequences. 
    Uses SelectionSequence:
    help(SelSequence)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self, name, Sequences = []) :
        self.sequences = copy(Sequences)
        self.gaudiseq = None
        self._name = name
        self.algos = []
        for seq in self.sequences :
            self.algos += seq.algos
    def name(self) :
        return self._name
        
    def outputLocations(self) :
        return [seq.outputLocation() for seq in self.sequences]

    def sequence(self, sequencerType = _sequencerType) :
        if self.gaudiseq == None :
            self.gaudiseq = sequencerType(self.name(),
                                          ModeOR = True,
                                          ShortCircuit = False,
                                          Members = [seq.sequence() for seq in self.sequences])
        return self.gaudiseq
