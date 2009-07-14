#$Id: Wrappers.py,v 1.2 2009-07-14 12:30:33 jpalac Exp $
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:

   - Selection          Wraps a selection configurable and the selections it
                        requires
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

class Selection :
    """
    Wrapper class for offline selection. Takes a top selection DVAlgorithm
    configurable plus a list of required selection configurables. It uses
    the required selections to set the InputLocations of the top selection.

    Example: selection for A -> B(bb), C(cc)

    # create and configure selection algorithm configurable:
    A2B2bbC2cc = CombineParticles('A2B2bbC2cc')
    A2B2bbC2cc.DecayDescriptor = ...
    A2B2bbC2cc.MotherCut = ...
    # now assume SelB and SelC have been defined in modules B2bb and C2cc
    # respectively
    from B2bb import SelB
    from C2cc import SelC
    # now create a Selection instance using the B and C selections:
    from PhysSelPython.Wrappers import Selection
    SelA2B2bbC2cc = Selection(A2B2bbC2cc, requiredSelecitons=[SelB, SelC]    
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    def __init__(self, algo, requiredSelections ) :
        self.algo = algo
        self.requiredSelections = []
        for sel in requiredSelections :
            if type(sel) == str :
                self.dealWithStringSelection(sel) # Std should not do anything
                self.algo.InputLocations += [sel]
            else :
                self.algo.InputLocations += [sel.name()]
                self.requiredSelections.append(sel)
        print "Required Selection Algorithms: ", self.requiredSelections
    def algorithm(self) :
        return self.algo
    def dealWithStringSelection(self, selection) :
        print "Selection.dealWithStringSelection(", selection, "): DO NOTHING"
    def name(self) :
        return self.algo.name()

class SelectionSequence :
    """
    Wrapper class for offline selection sequence. Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to for a GaudiSequences with all the required
    selecitons needed to run the top selection.

    Example: selection sequence for A -> B(bb), C(cc)

    # Assume module A2B2bbC2cc defining a Selection object for the decay
    # A -> B(bb), C(cc)
    from A2B2bbC2cc import SelA2B2bbC2cc
    from PhysSelPython.Wrappers import SelectionSequence
    SeqA2B2bbC2cc = SelectionSequence(SelA2B2bbC2cc)
    # use it
    mySelSeq = SeqA2B2bbC2cc.sequence()
    dv = DaVinci()
    dv.UserAlgorithms = [mySelSeq]
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    def __init__(self, TopSelection ) :
        self.algo = TopSelection.algorithm()
        self.seq = GaudiSequencer("Seq"+self.algo.name())
        self.seq.Members += [TopSelection.algorithm()]
        self.buildSelectionList( TopSelection.requiredSelections )
        self.seq.Members.reverse()
    def sequence(self) :
        return self.seq
    def name(self) :
        return self.seq.name()
    def topAlgName(self) :
        return self.algo.name()
    def buildSelectionList(self, selections) :
        for sel in selections :
            print "Adding Algo ", sel.name(), " to ", self.seq.name()
            self.seq.Members += [sel.algorithm()]
            self.buildSelectionList( sel.requiredSelections )
