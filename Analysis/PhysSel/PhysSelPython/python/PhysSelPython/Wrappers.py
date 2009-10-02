#$Id: Wrappers.py,v 1.10 2009-10-02 08:36:39 jpalac Exp $
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:
   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataonDemand       Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand', 'Selection', 'SelectionSequence')

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import LHCbConfigurableUser

class DataOnDemand(LHCbConfigurableUser) :
    """
    Simple wrapper for a Data-On-Demand location. Returns output location
    via outputLocation() method. Can be used as a Selection in
    RequiredSelections field of other Selections.

    Example: wrap StdLoosePions

    SelStdLoosePions = DataOnDemand('SelStdLoosePions',
                                    Location = 'Phys/StdLoosePions')
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    __slots__ = {
        "Location"            : ""   , 
        "RequiredSelections"  : []
        }

    def outputLocation(self) :
        return self.getProp('Location')

    def algName(self) :
        loc = self.outputLocation()
        loc = loc[loc.rfind("/")+1:] # grab the last string after the last '/'
        return loc
    
class Selection(LHCbConfigurableUser) :
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

    __slots__ = {
        "Algorithm"           : "",
        "RequiredSelections"  : [],
        "OutputBranch" : "Phys"
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

    def outputLocation(self) :
        branch = self.getProp('OutputBranch')
        return branch + "/" + self.algName()
    
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

    algos = []
    
    def __apply_configuration__(self) :
        print "Adding Algo ", self.algorithm().name(),  " to ", self.sequence().name()
        self.algos.insert(0, self.algorithm())
        sel = self.topSelection()
        sel.__apply_configuration__()
        self.sequence().Members.insert(0,self.algorithm())
        self.buildSelectionList( self.topSelection().RequiredSelections )
        
    def topSelection(self) :
        return self.getProp('TopSelection')

    
    def algorithm(self) :
        return self.topSelection().algorithm()
    
    def sequence(self) :
        from Configurables import GaudiSequencer
        return GaudiSequencer("GaudiSeq"+self.name() )

    def algName(self) :
        return self.algorithm().name()

    def outputLocation(self) :
        return self.topSelection().outputLocation()

    def outputLocations(self) :
        return [self.outputLocation()]

    def buildSelectionList(self, selections) :
        for sel in selections :
            print "Adding Algo ", sel.algName(), " to ", self.sequence().name()
            if type(sel) == DataOnDemand :
                print "DataOnDemand: do nothing"
            else :
                self.algos.insert(0, sel.algorithm())
                self.sequence().Members.insert( 0, sel.algorithm() )
                sel.__apply_configuration__()
                self.buildSelectionList( sel.requiredSelections() )



