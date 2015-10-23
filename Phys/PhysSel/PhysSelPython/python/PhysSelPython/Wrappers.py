#$Id: Wrappers.py,v 1.50 2010-08-11 08:42:14 ibelyaev Exp $
"""
Wrapper classes for a DaVinci offline physics selection. The following classes
are available:

   - Selection          Wraps a selection configurable and the selections it
                        requires
   - DataOnDemand       Wraps a string TES location to make it look like a Seleciton
   - AutomaticData      Wraps a string TES location to make it look like a Seleciton
   - SelectionSequence  Creates a sequence from a selection such that all the
                        sub-selections required are executed in the right order
   - MergedSelection    Merges a set of selections in OR mode, writing all data to a single location.
   - EventSelection     Wraps an algorithm that selects an event and produces no output data.
   - VoidEventSelection Selects event based on TES container properties, applying a LoKi__VoidFilter compatible cut to the data from a required selection.
   - PassThroughSelection
   - PrintSelection     Specialization of PassThroughSelection to print the objects
   """
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('DataOnDemand',
           'AutomaticData',
           'Selection',
           'MergedSelection',
           'EventSelection',
           'PassThroughSelection',
           'ChargedProtoParticleSelection',
           'SelectionSequence',
           'MultiSelectionSequence',
           'SelSequence',
           'NameError',
           'NonEmptyInputLocations',
           'IncompatibleInputLocations',
           'PrintSelection'
           )

from copy import copy

from SelPy.selection import ( flatAlgorithmList,
                              NamedObject,
                              UniquelyNamedObject,
                              ClonableObject,
                              SelectionBase,
                              AutomaticData,
                              NameError,
                              NonEmptyInputLocations,
                              IncompatibleInputLocations)

from SelPy.utils import update_dict_overlap
from SelPy.utils import CloneCallable
from SelPy.utils import connectToRequiredSelections
from SelPy.utils import makeOutputLocation

from SelPy.selection import Selection as Sel
from SelPy.selection import SelectionSequence as SelSequence
from SelPy.selection import EventSelection as EvtSel
from SelPy.selection import PassThroughSelection as PassThroughSel
from SelPy.selection import AutomaticData as autodata

from Configurables import LoKi__VoidFilter as VoidFilter
from GaudiConfUtils import configurableExists, isConfigurable
from GaudiConfUtils import ConfigurableGenerators
import Configurables

def checkName(name) :
    if configurableExists(name) :
        raise NameError('Could not instantiate Selection '+name+' because Configurable with the same name already exists. Try a different name for your Selection')

def checkConfigurable(obj) :
    if type(obj).__name__ == obj.name().replace('::','__') :
        raise NameError('Could not instantiate Selection because input Configurable '+obj.name()+' has default name. This is too unsafe to be allowed.')

def selectionWrapper(selType, name, *args, **kwargs) :
    """
    Construct a selection of type selType with construction arguments.
    Checks if name already belongs to an existing Configurable.
    Checks whether input is Configurable and if so, extracts its non-default
    parameters and passes them as a configurableGenerator.
    """
    checkName(name)

    algorithm = kwargs.pop('Algorithm')
    if isConfigurable( algorithm )  :
        checkConfigurable( algorithm)
        algGen=CloneCallable(algorithm)
        kwargs['ConfGenerator'] = algGen
    else :
        kwargs['ConfGenerator'] = algorithm
    return selType(name, *args, **kwargs)

def Selection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.Selection. Since SelPy.Selection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.selection.Selection.
    """
    return selectionWrapper(Sel, name, *args, **kwargs)

Selection.__doc__ += '\nSelPy.selection.Selection:\n'+Sel.__doc__

def EventSelection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.EventSelection. Since SelPy.EventSelection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.selection.EventSelection.
    """
    return selectionWrapper(EvtSel, name, *args, **kwargs)
EventSelection.__doc__ += '\nSelPy.selection.EventSelection:\n'+EvtSel.__doc__

def PassThroughSelection(name, *args, **kwargs) :
    """
    Wrapper around SelPy.PassThroughSelection. Since
    SelPy.PassThroughSelection takes a
    ConfigurableGenerator as input and constructs a Configurable 
    with it it's own name, check first whether a configurable
    with that name already exists and raise a NameError if that is the case.
    If not, construct and return a SelPy.selection.PassThroughSelection.
    """
    return selectionWrapper(PassThroughSel, name, *args, **kwargs)    
PassThroughSelection.__doc__ += '\nSelPy.selection.PassThroughSelection:\n'+PassThroughSel.__doc__

class AutomaticData(NamedObject, SelectionBase) :
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
    'Phys_StdLoosePions'
    >>> pions = AutomaticData(Location='Phys/StdLoosePions/Particles')
    >>> pions.outputLocation()
    'Phys/StdLoosePions/Particles'
    """
    
    def __init__( self                ,
                  Location            ,
                  UseRootInTES = True ) :
        
        NamedObject.__init__(self, Location.replace('/', '_'))
        
        _alg = VoidFilter('SelFilter'+self.name(),
                          Code = "CONTAINS('"+Location+"',%s)>0" % UseRootInTES )
        
        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation=Location,
                               requiredSelections = [] )

DataOnDemand = AutomaticData


class VoidEventSelection(UniquelyNamedObject,
                         ClonableObject,
                         SelectionBase) :
    """
    Selection class for event selection based on contents of TES location.
    Can be used just like a Selection object.
    Constructor argument Code is LoKi__VoidFilter compatible single-location
    cut, where the location is expressed as '<Location>' and determined
    from the RequiredSelection's outputLocation(). The outputLocation() of
    this algorithm is the same as that of the RequiredSelection.
    Example:
    from PhysSelPython.Wrappers import VoidEventSelection, SelectionSequence
    evtSel = VoidEventSelection( name='MyEvtSel',
                                 Code="CONTAINS('<Location>')>0",
                                 RequiredSelection = AutomaticData(....) )
    help(SelectionSequence)
    selSeq = SelectionSequence('MyEvtSelSeq', TopSelection = evtSel)
    """

    __author__ = "Juan Palacios palacios@physik.uzh.ch"
    
    def __init__(self,
                 name,
                 Code,
                 RequiredSelection ) :

        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())
        checkName(self.name())
        
        _code = Code.replace('<Location>',
                             "'"+RequiredSelection.outputLocation()+"'")
        _code = _code.replace('\"\'', '\'').replace('\'\"', '\'').replace('\'\'','\'')
        _alg = VoidFilter(self.name(), Code = _code)

        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation = RequiredSelection.outputLocation(),
                               requiredSelections = [RequiredSelection])

    
class MergedSelection(NamedObject,
                      ClonableObject,
                      SelectionBase) :
    """
    Selection wrapper class for merging output of various Selections.
    Can be used just like any Selection object.
    Example:
    
    from PhysSelPython.Wrappers import MergedSelection, AutomaticData, SelectionSequence
    sel00 = AutomaticData(Location = 'Phys/Sel00')
    sel01 = AutomaticData(Location = 'Phys/Sel01')
    sel02 = AutomaticData(Location = 'Phys/Sel02')
    sel03 = AutomaticData(Location = 'Phys/Sel03')
    merge = MergedSelection('SelMerger', RequiredSelections = [sel00, sel01, sel02, sel03])
    seqMerge = SelectionSequence('SeqMergeSelections, TopSelection = merge)

    This will OR all the selections and place the output in merge.outputLocation().
    """
    def __init__(self,
                 name,
                 RequiredSelections = [],
                 OutputBranch = "Phys",
                 sequencerType=Configurables.GaudiSequencer) :

        NamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())

        self._sel = Selection(name,
                              Algorithm = ConfigurableGenerators.FilterDesktop(Code='ALL'),
                              RequiredSelections = RequiredSelections,
                              OutputBranch = OutputBranch)
        
        self._selections = RequiredSelections 
        
        ## it was 
        #self._algos1 = flatAlgorithmList(self._sel)

        ## it now 
        self._algos = [] 
        for s in RequiredSelections :
            lst = flatAlgorithmList ( s )
            if   not lst : continue 
            elif 1 == len ( lst ) :
                self._algos.append( lst[0] ) 
            else :
                _seq  =  sequencerType  ( 'SEQ:' + s.name() , Members = lst ) 
                self._algos.append  ( _seq )
                
        self._algos += [ self._sel.algorithm() ]

        _alg = sequencerType('Seq'+self.name(),
                             Members = self._algos,
                             ModeOR = True,
                             ShortCircuit = False)

        SelectionBase.__init__(self,
                               algorithm = _alg,
                               outputLocation = self._sel.outputLocation(),
                               requiredSelections = [])


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

    Uses selection.SelSequence and selection.flatAlgorithmList
    help(SelSequence)
    help(flatAlgorithmList)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 TopSelection,
                 EventPreSelector = [],
                 PostSelectionAlgs = [],
                 sequencerType = Configurables.GaudiSequencer) :

        checkName(name)
        SelSequence.__init__(self,
                             name,
                             TopSelection,
                             EventPreSelector,
                             PostSelectionAlgs,
                             sequencerType)

    def outputLocations(self) :
        return [self.outputLocation()]
    
class MultiSelectionSequence(UniquelyNamedObject,
                             ClonableObject) :
    """
    Wrapper class for offline multiple selection sequence creation.
    Takes a list of SelectionSequence objects and produces on demand a
    sequence with an OR of the list of sequences. 
    Uses SelectionSequence:
    help(SelSequence)
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"

    def __init__(self,
                 name,
                 Sequences = [],
                 sequencerType = Configurables.GaudiSequencer) :

        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())
        self._sequences = list(Sequences)
        self._algos = []
        for seq in self._sequences :
            self._algos += seq.algorithms()

        checkName(self.name())
        self._gaudiseq = sequencerType(self.name(),
                                      ModeOR = True,
                                      ShortCircuit = False,
                                      Members = [seq.sequence() for seq in self._sequences])

    def algorithms(self) :
        return list(self._algos)
        
    def outputLocations(self) :
        return [seq.outputLocation() for seq in self._sequences]

    def sequence(self) :
        return self._gaudiseq

class ChargedProtoParticleSelection(UniquelyNamedObject,
                                    ClonableObject,
                                    SelectionBase) :

    """
    Simple wrapper class to construct charged ProtoParticles. Can add PID
    information for Rich, Muon, Ecal, Brem, Hcal, Prs, Spd and Velo.
    Usage:
    tracks = AutomaticData(Location='Some/Location/With/Tracks')
    ppSel = ChargedProtoParticleSelection('TestPP', RequiredSelections=[tracks],  PIDElements=['Rich', 'Muon', 'Hcal'])
    """
    __allowedPIDs = ('Rich',
                     'Muon',
                     'Ecal',
                     'Brem',
                     'Hcal',
                     'Prs',
                     'Spd',
                     'Velo')

    def __init__(self,
                 name,
                 RequiredSelections = [],
                 OutputBranch="Rec",
                 Extension="ProtoParticles",
                 InputDataSetter="Inputs",
                 PIDElements= []) :

        checkName(name)
        checkName(name+'PPMaker')
        self.checkPIDElements(PIDElements)
        UniquelyNamedObject.__init__(self, name)
        ClonableObject.__init__(self, locals())

        _outputLocation = makeOutputLocation(name = self.name(),
                                             branch    = OutputBranch,
                                             leaf      = Extension)

        _ppMaker = Configurables.ChargedProtoParticleMaker(name+'PPMaker')
        _ppMaker.Output = _outputLocation
        _inputs =  [sel.outputLocation() for sel in RequiredSelections]
        _ppMaker.__setattr__(InputDataSetter, _inputs)

        alg = ConfigurableGenerators.GaudiSequencer(Members=[_ppMaker])
        
        for pid in PIDElements :
            _pidAlgName='ChargedProtoParticleAdd'+pid+'Info'
            _conf = getattr(Configurables, _pidAlgName)(name+pid+'Info')
            alg.Members+= [_conf]
        _dll = Configurables.ChargedProtoCombineDLLsAlg(name+'CombDLLs')
        alg.Members+=[_dll]
        
        SelectionBase.__init__(self,
                               algorithm =alg( self.name() ) ,
                               outputLocation=_outputLocation,
                               requiredSelections = RequiredSelections )

        

    def checkPIDElements(self, pids) :
        for pid in pids :
            if pid not in ProtoParticleSelection.__allowedPIDs :
                raise Exception(pid+" not in allowed set of PID infos")


# ========================================================================
## helper shortcut fot creation of 1-step selection
#
#  It is very simple stuff, but in practice it does save a lot of typing!
# 
#  E.g. for 1-step filtering:
#
#  @code
#
#  from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
#  from StandardParticles                     import StdAllLooseMuons as muons 
#  from PhysSelPython.Wrappers                import SimpleSelection
#
#  good_muons = SimpleSelection (
#       'GoodMu'          ,
#       FilterDesktop     ,
#       input = [ muons ] ,
#       Code  = " PT>1 * GeV "  ## will be transferred to the algorithm 
#       )
#
#  @endcode
# 
#  For 1-step decay 
#
#  @code
#
#  from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
#  from StandardParticles                     import StdAllLooseMuons as muons 
#  from PhysSelPython.Wrappers                import SimpleSelection
#
#  dimuons = SimpleSelection (
#       'DiMu'           ,
#       CombineParticles ,
#       input = [ muons ] ,
#       ## the rest of parameters are sent to athe algorithm:
#       DecayDescriptor = " J/psi(1S) -> mu+ mu-" ,
#       CombinationCut  = " in_range( 3 * GeV  , AM , 3.2 * GeV )  " ,
#       MotherCut       = " in_range( 3 * GeV  ,  M , 3.2 * GeV )  " ,
#       )
#
#  @endcode
# 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-12
# 
#  @param name          unique selection name 
#  @param algotype      type of algorithm to be used
#  @param inputs        list of input/required selection
#  @param args          additional arguments to be used for algorithm
#  @param kwargs        additional arguments to be used for algorithm
#  @return the selection object
# 
def SimpleSelection (
    name     ,   ## unique selection name 
    algotype ,   ## type of algorithm to be used
    inputs   ,   ## list of input/required selections
    *args    ,   ## additional arguments to be used for algorithm
    **kwargs ) : ## additional arguments to be used for algorithm
    """
    Helper shortcut fot creation of 1-step selection

    It is very simple stuff, but in practice it does save a lot of typing!
    
    
    #  from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
    #  from StandardParticles                     import StdAllLooseMuons as muons 
    #  from PhysSelPython.Wrappers                import SimpleSelection
    #
    #  good_muons = SimpleSelection (
    #       'GoodMu'          ,
    #       FilterDesktop     ,
    #       input = [ muons ] ,
    #       Code  = ' PT>1 * GeV '  ## will be transferred to the algorithm 
    #       )
    #

    For 1-step decay 
    
    #
    #  from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
    #  from StandardParticles                     import StdAllLooseMuons as muons 
    #  from PhysSelPython.Wrappers                import SimpleSelection
    #
    #  dimuons = SimpleSelection (
    #       'DiMu'           ,
    #       CombineParticles ,
    #       input = [ muons ] ,
    #       ## the rest of parameters are sent to athe algorithm:
    #       DecayDescriptor = ' J/psi(1S) -> mu+ mu- ' ,
    #       CombinationCut  = ' in_range( 3 * GeV  , AM , 3.2 * GeV )  ' ,
    #       MotherCut       = ' in_range( 3 * GeV  ,  M , 3.2 * GeV )  ' ,
    #       )
    
    """

    ## get selection's properties: 
    output_branch = kwargs.pop ( 'OutputBranch'     , 'Phys'      )
    input_setter  = kwargs.pop ( 'InputDataSetter'  , 'Inputs'    ) 
    output_setter = kwargs.pop ( 'OutputDataSetter' , 'Output'    ) 
    extension     = kwargs.pop ( 'Extension'        , 'Particles' ) 
    
    #
    ## create new algorithm or algorithm generator 
    #
    alg = algotype ( *args , **kwargs )
    #
    ## create selection 
    return Selection (
        name                               , 
        Algorithm          = alg           ,
        RequiredSelections = inputs        ,
        OutputBranch       = output_branch ,
        InputDataSetter    = input_setter  ,
        OutputDataSetter   = output_setter ,
        Extension          = extension             
        )

# =========================================================================
## helper utility to create 'Print'-selection, useful for debugging
#  Such object can be easily inserted into selection flow
#
#  @code 
#
#  ## some input selection
#  selection =
#    
#  ## add ``Printer''
#  selection = PrintSelection ( 'QUQU' , selection )
#    
#  @endcode 
#
#  @param name            unique selection name
#  @param input           input selection
#  @param printer         printer algorithm type
#  @param InputDataSetter davinci....
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-03-18
def PrintSelection (
    name                       ,    ## UNIQUE seelction name 
    input                      ,    ## input selection 
    printer         =  None    ,    ## printer algorthm type
    InputDataSetter = 'Inputs' ,    ## it is just DaVinci. 
    *args                      ,    ## algorithm parameters 
    **kwargs                   ) :  ## algorithm parameters 
    """
    
    ## some input selection
    selection =
    
    ## add ``Printer''
    selection = PrintSelection ( 'QUQU' , selection )
    
    """
    if printer is None :
        from GaudiConfUtils.ConfigurableGenerators import PrintDecayTree as _Printer_
        printer = _Printer_ 

    #
    ## create new "algorithm"
    #
    algo = printer ( *args , **kwargs )

    #
    ## finally construct valid "Selection"
    #
    return PassThroughSelection (
        name                                ,
        Algorithm         = algo            ,
        RequiredSelection = input           ,
        InputDataSetter   = InputDataSetter )

# ========================================================================
# The END 
# ========================================================================
