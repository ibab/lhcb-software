#$Id: Wrappers.py,v 1.20 2009-10-28 14:09:15 jpalac Exp $
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:
   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataOnDemand       Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
"""
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand', 'AutomaticData', 'Selection', 'SelectionSequence')

class DataOnDemand(object) :
    """
    Simple wrapper for a data location. To be used for locations
    that are guaranteed to be populated. This could be a location
    on a DST or a location registered to the DataOnDemandSvc.
    Returns output location via outputLocation() method.
    Can be used as a Selection in RequiredSelections field of other
    Selections.

    Example: wrap StdLoosePions

    SelStdLoosePions = DataOnDemand('SelStdLoosePions',
                                    Location = 'Phys/StdLoosePions')
    """

    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    def __init__ (self,
                  name,
                  Location = "", 
                  RequiredSelections = [] ) :
        self._name = name
        self.requiredSelections = []
        self.Location = Location

    def name(self) :
        return self._name

    def algorithm(self) :
        return None

    def __apply_configuration__(self) :
        print self.name(), ".__apply_configuration(): NOT A CONFIGURABLE."
        
    def outputLocation(self) :
        return self.Location

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

    def __init__(self,
                 name,
                 Algorithm,
                 RequiredSelections = [],
                 OutputBranch = "Phys") :
        self.requiredSelections = []
        print "Instantiating ", name
        for sel in RequiredSelections :
            print "Adding required selection ", sel.name()
            self.requiredSelections.append(sel)
        self.alg = Algorithm
        self._name = name
        self.OutputBranch = OutputBranch
        
        for sel in self.requiredSelections :
            print "Algo ", self.algName(),  " adding InputLocation ", sel.outputLocation()
            self.algorithm().InputLocations += [sel.outputLocation()]
        print "Required Selection Algorithms: ", self.requiredSelections

    def name(self) :
        return self._name

    def __apply_configuration__(self) :
        print self.name(), ".__apply_configuration(): NOT A CONFIGURABLE."

    def algorithm(self) :
        return self.alg

    def algName(self) :
        return self.algorithm().name()

    def outputLocation(self) :
        return self.OutputBranch + "/" + self.algName()
    
class SelectionSequence(object) :
    """
    Wrapper class for offline selection sequence. Takes a Selection object
    corresponding to the top selection algorithm, and recursively uses
    Selection.requiredSelections to for a GaudiSequences with all the required
    selecitons needed to run the top selection. Can add list of event selection
    algorithms to be added at the beginning of the sequence, and a list of
    algorithms to be run straight after the selection algoritms.

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
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = [] ) :
        self._name = name
        self.TopSelection = TopSelection
        self.algos = []
        self.algos += EventPreSelector
        self.alg = self.TopSelection.algorithm()
        self.sels = [self.alg]
        if (self.alg != None) :
            self.buildSelectionList( self.TopSelection.requiredSelections )
        self.gaudiseq = None
        self.algos += self.sels
        for postAlg in PostSelectionAlgs :
            self.algos.append(postAlg)
            
    def name(self) :
        return self._name

    def __apply_configuration__(self) :
        print self.name(), ".__apply_configuration(): NOT A CONFIGURABLE."

    def algorithm(self) :
        return self.alg
        
    def sequence(self) :
        if self.gaudiseq == None :
            from Configurables import GaudiSequencer
            self.gaudiseq = GaudiSequencer(self.name(), Members = self.algos)
        return self.gaudiseq

    def algName(self) :
        return self.algorithm().name()

    def outputLocation(self) :
        return self.TopSelection.outputLocation()

    def outputLocations(self) :
        return [self.outputLocation()]

    def buildSelectionList(self, selections) :
        for sel in selections :
            if type(sel) == DataOnDemand :
                print "DataOnDemand: do nothing"
            else :
                self.sels.insert(0, sel.algorithm())
                self.buildSelectionList( sel.requiredSelections )



