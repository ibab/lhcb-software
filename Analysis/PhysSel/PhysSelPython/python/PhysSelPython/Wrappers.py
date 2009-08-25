#$Id: Wrappers.py,v 1.5 2009-08-25 13:37:17 jpalac Exp $
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

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import LHCbConfigurableUser

class DataOnDemand(LHCbConfigurableUser) :
    """
    Simple wrapper for a Data-On-Demand location. Returns output location
    via algName() method. Can be used as a Selection in RequiredSelections
    field of other Selections.

    Example: wrap StdLoosePions

    SelStdLoosePions = DataOnDemand('SelStdLoosePions',
                                    Location = 'StdLoosePions')
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    __slots__ = {
        "Algorithm"           : ""   ,
        "Location"            : ""   , 
        "RequiredSelections"  : []
        }
    def algName(self) :
        return self.getProp('Location')
    
class Selection(LHCbConfigurableUser) :
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
    SelA2B2bbC2cc = Selection('SelA2B2bbC2cc',
                              Algorithm = A2B2bbC2cc,
                              RequiredSelections = [SelB, SelC]    
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    __slots__ = {
        "Algorithm"           : ""   ,
        "RequiredSelections"  : []
        }
    
    def __apply_configuration__(self) :
        print "Set Algo"
        print "Set required selections"
        for sel in self.requiredSelections() :
            print self.algName(),  " adding InputLocation ", sel.algName()
            self.algorithm().InputLocations += [sel.algName()]
        print "Required Selection Algorithms: ", self.requiredSelections()

    def requiredSelections(self) :
        return self.getProp('RequiredSelections')

    def algorithm(self) :
        return self.getProp('Algorithm')

    def dealWithStringSelection(self, selection) :
        print "Selection.dealWithStringSelection(", selection, "): DO NOTHING"

    def algName(self) :
        return self.algorithm().name()

class SelectionSequence(LHCbConfigurableUser) :
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
    SeqA2B2bbC2cc = SelectionSequence('SeqA2B2bbC2cc',
                                      TopSelection = SelA2B2bbC2cc)
    # use it
    mySelSeq = SeqA2B2bbC2cc.sequence()
    dv = DaVinci()
    dv.UserAlgorithms = [mySelSeq]
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    __slots__ = {
        "TopSelection" : ""
        }

    def __apply_configuration__(self) :
        print "Adding Algo ", self.algorithm().name(),  " to ", self.sequence().name()
        self.sequence().Members.insert(0,self.algorithm())
        self.buildSelectionList( self.topSelection().RequiredSelections )

    def topSelection(self) :
        return self.getProp('TopSelection')

    def algorithm(self) :
        return self.topSelection().algorithm()
    
    def sequence(self) :
        return GaudiSequencer("GaudiSeq"+self.name() )

    def algName(self) :
        return self.algorithm().name()

    def outputLocations(self) :
        return ['Phys/' + self.algName()]

    def buildSelectionList(self, selections) :
        for sel in selections :
            print "Adding Algo ", sel.algName(), " to ", self.sequence().name()
            if type(sel) == DataOnDemand :
                print "DataOnDemand: do nothing"
            else :
                self.sequence().Members.insert( 0, sel.algorithm() )
                self.buildSelectionList( sel.requiredSelections() )
