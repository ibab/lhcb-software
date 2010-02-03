#$Id: Wrappers.py,v 1.30 2010-02-03 08:45:56 jpalac Exp $
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
           'NameError',
           'SelSequence')


_sequencerType = None

try :
    from Gaudi.Configuration import *
    from Configurables import GaudiSequencer
    _sequencerType = GaudiSequencer
except :
    print 'WARNING: Gaudi Configurables not found. No default sequencer type defined.'

from selection import ( Selection,
                        FlatSelectionListBuilder,
                        DataOnDemand,
                        AutomaticData,
                        NameError,
                        update_overlap,
                        SelSequence                )


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
