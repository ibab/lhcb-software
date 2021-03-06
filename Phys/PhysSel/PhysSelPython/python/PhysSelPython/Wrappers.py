#$Id$
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
   - CheckPVSelection   Specialization of PassThroughSelection to limit the selection to use CheckPV 
   - LimitSelection     Specialization of PassThroughSelection to limit the selection 
   - MomentumScaling    ``pseudo-selection'' that applyes (globally) Momentum calibration
   - SimpleSelection    simple compact 1-step way to create the selection 
   - FilterSelection    Specialization of SimpleSelection for filtering using FilterDesktop 
   - CombineSelection      Specialization of SimpleSelection for CombineParticles algorithm
   - Combine3BodySelection Specialization of SimpleSelection for DaVinci::N3BodyDecays algorithm
   - Combine4BodySelection Specialization of SimpleSelection for DaVinci::N4BodyDecays algorithm
   - TupleSelection        Specialization of SimpleSelection for DecayTreeTuple algorithm
   """
__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ( 'DataOnDemand'                  ,
            'AutomaticData'                 ,
            'Selection'                     ,
            'MergedSelection'               ,
            'EventSelection'                ,
            'PassThroughSelection'          ,
            'ChargedProtoParticleSelection' ,
            'SelectionSequence'             ,
            'MultiSelectionSequence'        ,
            'SelSequence'                   ,
            'NameError'                     ,
            'NonEmptyInputLocations'        ,
            'IncompatibleInputLocations'    ,
            ##
            'SimpleSelection'               , 
            'FilterSelection'               , 
            'PrintSelection'                , 
            'LimitSelection'                ,
            'CombineSelection'              ,
            'Combine3BodySelection'         ,
            'Combine4BodySelection'         ,
            'TupleSelection'                ,
            ##
            'CheckPVSelection'              ,  
            ## 
            'MomentumScaling'   ## ``pseudo-selection'' to aplly (globaly) momentum sclaing         
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
    
    def __init__( self                 ,
                  Location             ,
                  UseRootInTES = True  ,
                  monitor      = False ) :
        
        NamedObject.__init__(self, Location.replace('/', '_'))
        
        from Configurables import LoKi__VoidFilter as VoidFilter

        _alg = VoidFilter( 'SelFilter'+self.name() )
        
        if not monitor : 
            _alg.Code = """
            0<CONTAINS('%s',%s)
            """ % ( Location , UseRootInTES )
        else :
            _alg.Preambulo += [
                "from LoKiCore.functions import monitor"     ,
                "from LoKiCore.math      import max as lmax" ,
                ]
            ## create monitored functor 
            _alg.Code       = """
            0< monitor ( lmax ( CONTAINS('%s',%s), FZERO ) ,'# %s', 0 )
            """ % ( Location , UseRootInTES , Location )
            
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
        
        from Configurables import LoKi__VoidFilter as VoidFilter
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
def SimpleSelection (
    name     ,   ## unique selection name 
    algotype ,   ## type of algorithm to be used
    inputs   ,   ## list of input/required selections
    *args    ,   ## additional arguments to be used for algorithm
    **kwargs ) : ## additional arguments to be used for algorithm
    """Helper shortcut fot creation of 1-step selection

    It is very simple stuff, but in practice it does save a lot of typing!
    
    >>> from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
    >>> from StandardParticles                     import StdAllLooseMuons as muons 
    >>> from PhysSelPython.Wrappers                import SimpleSelection
    
    >>> good_muons = SimpleSelection (
    ...  'GoodMu'          ,
    ...  FilterDesktop     ,
    ... input = [ muons ] ,
    ... Code  = ' PT>1 * GeV '  ## will be transferred to the algorithm 
    ... )

    For 1-step decay 
    
    >>> from GaudiConfUtils.ConfigurableGenerators import CombineParticles 
    >>> from StandardParticles                     import StdAllLooseMuons as muons 
    >>> from PhysSelPython.Wrappers                import SimpleSelection

    >>> dimuons = SimpleSelection (
    ...     'DiMu'           ,
    ...      CombineParticles ,
    ...      input = [ muons ] ,
    ...      ## the rest of parameters are sent to athe algorithm:
    ...      DecayDescriptor = ' J/psi(1S) -> mu+ mu- ' ,
    ...     CombinationCut  = ' in_range( 3 * GeV  , AM , 3.2 * GeV )  ' ,
    ...      MotherCut       = ' in_range( 3 * GeV  ,  M , 3.2 * GeV )  ' ,
    ... )    
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
    ## adjust inputs 
    if not isinstance ( inputs , ( list , tuple ) ) : inputs = [ inputs ] 
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
#  selection = PrintSelection ( selection )
#    
#  @endcode 
#
#  @param input           input selection
#  @param printer         printer algorithm type
#  @param InputDataSetter davinci....
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-03-18
def PrintSelection (
    input                      ,    ## input selection 
    printer         =  None    ,    ## printer algorthm type
    InputDataSetter = 'Inputs' ,    ## it is just DaVinci. 
    *args                      ,    ## algorithm parameters 
    **kwargs                   ) :  ## algorithm parameters 
    """Helper utility to create 'Print'-selection, useful for debugging
    Such object can be easily inserted into selection flow
    
    >>> ## some input selection
    >>> selection =
    
    >>> add ``Printer''
    >>> selection = PrintSelection ( selection )    
    """
    if printer is None :
        from GaudiConfUtils.ConfigurableGenerators import PrintDecayTree as _Printer_
        printer = _Printer_ 

    #
    ## create new "algorithm"
    #
    algo = printer ( *args , **kwargs )
    
    name = '%s_PRINT' % input.name() 
    #
    ## finally construct valid "Selection"
    #
    return PassThroughSelection (
        name                                ,
        Algorithm         = algo            ,
        RequiredSelection = input           ,
        InputDataSetter   = InputDataSetter )

# ========================================================================#
## special type of PassThroughSelection, that
#  "limits" the size of selection
#  @code
#  my_selection = ....
#  my_selection_limited = LimitedSelection ( my_selection , maxsize = 100 )
#  another_selection = XXXSelection ( 'another_selection' ,
#                                       ... ,
#                       RequiredSelections = [ my_selection_limited ] )
#  @endcode
#  @see PassThroughSelection
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-02-10
def LimitSelection ( input               ,                     
                     maxsize             ,
                     minsize      = 1    , 
                     UseRootInTES = True ) :
    """Special type of PassThroughSelection, that
    'limits' the size of selection
    >>> my_selection = ....
    >>> my_selection_limit = LimitSelection ( my_selection , maxsize = 100 )
    >>> another_selection  = XXXSelection ( 'another_selection' ,
    ...                                        ... ,
    ...                 RequiredSelections = [ my_selection_limit ] )
    """
    if 1 == minsize :
        name = '%s_LIMIT_%d'
        code = "CONTAINS('%s',%s)<=%d"
        ##
        name = name  % ( input.name()                          , maxsize )
        code = code  % ( input.outputLocation() , UseRootInTES , maxsize )
    else:
        name = '%s_LIMIT_%d_%d'
        code = "in_range(%d,CONTAINS('%s',%s),%d)"
        ##
        name = name  % ( input.name()           , minsize , maxsize )
        code = code  % ( minsize ,
                         input.outputLocation() , 
                         UseRootInTES           , maxsize )
    ##
    from GaudiConfUtils.ConfigurableGenerators import LoKi__VoidFilter as _VFilter_
    return PassThroughSelection (
        name                                         ,
        Algorithm         = _VFilter_(
        Code = code , Preambulo = ['from LoKiCore.functions import in_range'] ,
        ) ,
        RequiredSelection = input                    )


# ========================================================================#
## special type of PassThroughSelection, that used CheckPV algorithm
#  @code
#  my_selection = ....
#  my_selection_pv   = CheckPVSelection ( my_selection , MinPVs = 1 )
#  another_selection = XXXSelection ( 'another_selection' ,
#                                       ... ,
#                       RequiredSelections = [ my_selection_pv ] )
#  @endcode
#  @see PassThroughSelection
#  @see LimitSelection
#  @see CheckPV 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-03-11
def CheckPVSelection ( input         ,                     
                       MinPVs   =  1 ,
                       MaxPVs   = -1 ,
                       **kwargs      ) :
    """Special type of PassThroughSelection, that used CheckPV algorithm 
    >>> my_selection = ....
    >>> my_selection_pv = LimitSelection ( my_selection , MinPVs = 1  )
    >>> another_selection  = XXXSelection ( 'another_selection' ,
    ...                                        ... ,
    ...                 RequiredSelections = [ my_selection_pv ] )
    """
    if   0 <= MinPVs <= MaxPVs : 
        name = 'CHECKPV_%d_%d' %  ( MinPVs , MaxPVs )
    elif 0 <= MinPVs : 
        name = 'CHECKPV_%d'    %    MinPVs
    else :
        raise TypeError, 'CheckPV: invalid setting: %s/%s' %( MinPVs, MaxPVs ) 
    ##
    from GaudiConfUtils.ConfigurableGenerators import CheckPV as _CHECK_PV_
    return PassThroughSelection (
        name                        ,
        Algorithm         = _CHECKPV_ ( MinPVs = MinPVs , MaxPVs = MaxPVs , **kwargs ) ,
        RequiredSelection = input   )



# ========================================================================#
## special type of PassThroughSelection, that used CheckPV algorithm
#  @code
#  my_selection = ....
#  my_selection_pv   = CheckPVSelection ( my_selection , MinPVs = 1 )
#  another_selection = XXXSelection ( 'another_selection' ,
#                                       ... ,
#                       RequiredSelections = [ my_selection_pv ] )
#  @endcode
#  @see PassThroughSelection
#  @see LimitSelection
#  @see CheckPV 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-03-11
def CheckPVSelection ( input         ,                     
                       MinPVs   =  1 ,
                       MaxPVs   = -1 ,
                       **kwargs      ) :
    """Special type of PassThroughSelection, that used CheckPV algorithm 
    >>> my_selection = ....
    >>> my_selection_pv = LimitSelection ( my_selection , MinPVs = 1  )
    >>> another_selection  = XXXSelection ( 'another_selection' ,
    ...                                        ... ,
    ...                 RequiredSelections = [ my_selection_pv ] )
    """
    if   0 <= MinPVs <= MaxPVs : 
        name = 'CHECKPV_%d_%d' %  ( MinPVs , MaxPVs )
    elif 0 <= MinPVs : 
        name = 'CHECKPV_%d'    %    MinPVs
    else :
        raise TypeError, 'CheckPV: invalid setting: %s/%s' %( MinPVs, MaxPVs ) 
    ##
    from GaudiConfUtils.ConfigurableGenerators import CheckPV as _CHECK_PV_
    return PassThroughSelection (
        name                        ,
        Algorithm         = _CHECKPV_ ( MinPVs = MinPVs , MaxPVs = MaxPVs , **kwargs ) ,
        RequiredSelection = input   )

# =============================================================================
## Useful shortcut for selection CombineParticles  algorithm
#  @code
#  from StandardParticles import StdAllLoosePions   as pions
#  from StandardParticles import StdAllLooseKaons   as kaons
#  charm = CombineSelection (
#     'charm'            , ## unique name 
#      [ pions , kaons ] , ## required selections
#      DecayDescriptor  = '[ D0 -> K- pi+]cc'    , 
#      CombinationCut   = 'in_range  (1.6 * GeV , AM , 2.0 * GeV) ' ,
#      MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
#      )
#  @endcode 
#  @see SimpleSelection
#  @see Selection
#  @see SelPy.Selection
#  @see CombineParticles 
#
#  @param name           unique selection name 
#  @param inputs         list of input/required selection
#  @param CombinationCut 'CombinationCut'-property of CombineParticles 
#  @param MotherCut           'MotherCut'-property of CombineParticles 
#  @param kwargs          additional arguments to be used for algorithm
#  @return the selection object
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-03-10 
def CombineSelection ( name  , inputs ,
                       CombinationCut ,
                       MotherCut      , 
                       **kwargs       ) :
    """ Useful shortcut for selection with CombineParticles algorithm
    >>> from StandardParticles import StdAllLoosePions   as pions
    >>> from StandardParticles import StdAllLooseKaons   as kaons
    >>> charm = CombineSelection (
    ... 'charm'            , ## unique name 
    ... [ pions , kaons ] , ## required selections
    ... DecayDescriptor  = '[ D0 -> K- pi+]cc'    , 
    ... CombinationCut   = 'in_range  (1.6 * GeV , AM , 2.0 * GeV) ' ,
    ... MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
    ... )
    """
    #
    from GaudiConfUtils.ConfigurableGenerators import CombineParticles as _COMBINER_
    ## check decay descriptor(s)
    if not kwargs.has_key ('DecayDescriptor' ) : 
        if not kwargs.has_key ('DecayDescriptors' ) :
            raise TypeError, 'CombineSelection: DecayDescriptor(s) must be specified!'
    
    ## create selection    
    return SimpleSelection ( name   , _COMBINER_ ,
                             inputs ,
                             CombinationCut = CombinationCut ,
                             MotherCut      = MotherCut      , **kwargs )

# =============================================================================
## Useful shortcut for selection with creation of 3-body decays
#  @code
#  from StandardParticles import StdAllLoosePions   as pions
#  from StandardParticles import StdAllLooseKaons   as kaons
#  charm = Combine3BodySelection (
#     'charm'            , ## unique name 
#      [ pions , kaons ] , ## required selections
#      DecayDescriptor  = '[ D+ -> K- pi+ pi+]cc'    , 
#      Combination12Cut = ' ( AM < 1.8 * GMeV ) & ( ACHI2DOCA(1,2)<10 ) ' ,
#      CombinationCut   = ''' in_range  (1.6 * GeV , AM , 2.0 * GeV)
#                         & ( ACHI2DOCA(1,3) < 10 )  
#                         & ( ACHI2DOCA(2,3) < 10 )  
#                         ''',
#      MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
#      )
#  @endcode 
#  @see SimpleSelection
#  @see Selection
#  @see SelPy.Selection
#  @see DaVinci::N3BodyDecays
#
#  @param name              unique selection name 
#  @param inputs           list of input/required selection
#  @param Combination12Cut 'Combination12Cut'-property of DaVinci::N3BodyDecays 
#  @param CombinationCut     'CombinationCut'-property of DaVinci::N3BodyDecays
#  @param MotherCut               'MotherCut'-property of DaVinci::N3BodyDecays
#  @param kwargs          additional arguments to be used for algorithm
#  @return the selection object
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-03-10 
def Combine3BodySelection ( name  , inputs   ,
                            Combination12Cut ,
                            CombinationCut   ,
                            MotherCut        , 
                            **kwargs         ) :
    """ Useful shortcut for selection with  DaVinci::N3BodyDecays algorithm
    >>> from StandardParticles import StdAllLoosePions   as pions
    >>> from StandardParticles import StdAllLooseKaons   as kaons
    >>> charm = Combine3BodySelection (
    ... 'charm'            , ## unique name 
    ... [ pions , kaons ] , ## required selections
    ... DecayDescriptor  = '[ D+ -> K- pi+ pi+]cc'    , 
    ... Combination12Cut = ' ( AM < 500 * MeV ) & ( ACHI2DOCA(1,2)<10 ) ' ,
    ... CombinationCut   = '''in_range  (1.6 * GeV , AM , 2.0 * GeV)
    ...                       & ( ACHI2DOCA(1,3)<10 ) 
    ...                       & ( ACHI2DOCA(2,3)<10 ) 
    ...                    ''' ,
    ... MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
    ... )
    """
    #
    from GaudiConfUtils.ConfigurableGenerators import DaVinci__N3BodyDecays as _COMBINER_
    ## check decay descriptor(s)
    if not kwargs.has_key ('DecayDescriptor' ) : 
        if not kwargs.has_key ('DecayDescriptors' ) :
            raise TypeError, 'Combine3BodySelection: DecayDescriptor(s) must be specified!'
        
    ## create selection    
    return SimpleSelection ( name   , _COMBINER_ ,
                             inputs ,
                             Combination12Cut = Combination12Cut ,
                             CombinationCut   = CombinationCut   ,
                             MotherCut        = MotherCut        , **kwargs )


# =============================================================================
## Useful shortcut for selection with creation of 4-body decays
#  @code
#  from StandardParticles import StdAllLoosePions   as pions
#  from StandardParticles import StdAllLooseKaons   as kaons
#  charm = Combine4BodySelection (
#     'charm'            , ## unique name 
#      [ pions , kaons ] , ## required selections
#      DecayDescriptor   = '[ D+ -> K- pi+ pi+ pi-]cc'    , 
#      Combination12Cut  = ' ( AM < 1.8 * GeV )  & ( ACHI2DOCA(1,2)<10 ) ' ,
#      Combination123Cut = '''( AM < 1.8 * GeV )
#                              & ( ACHI2DOCA(1,3)<10 )
#                              & ( ACHI2DOCA(2,4)<10 )
#                          ''', 
#      CombinationCut    = ''' in_range  (1.6 * GeV , AM , 2.0 * GeV)
#                         & ( ACHI2DOCA(1,4) < 10 )  
#                         & ( ACHI2DOCA(2,4) < 10 )  
#                         & ( ACHI2DOCA(3,4) < 10 )  
#                         ''',
#      MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
#      )
#  @endcode 
#  @see SimpleSelection
#  @see Selection
#  @see SelPy.Selection
#  @see DaVinci::N4BodyDecays
#
#  @param name              unique selection name 
#  @param inputs            list of input/required selection
#  @param Combination123Cut 'Combination123Cut'-property of DaVinci::N4BodyDecays 
#  @param Combination12Cut   'Combination12Cut'-property of DaVinci::N4BodyDecays 
#  @param CombinationCut       'CombinationCut'-property of DaVinci::N4BodyDecays
#  @param MotherCut                 'MotherCut'-property of DaVinci::N4BodyDecays
#  @param kwargs          additional arguments to be used for algorithm
#  @return the selection object
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-03-10 
def Combine4BodySelection ( name  , inputs    ,
                            Combination12Cut  ,
                            Combination123Cut ,
                            CombinationCut    ,
                            MotherCut         , 
                            **kwargs          ) :
    """ Useful shortcut for selection with  DaVinci::N3BodyDecays algorithm
    >>> from StandardParticles import StdAllLoosePions   as pions
    >>> from StandardParticles import StdAllLooseKaons   as kaons
    >>> charm = Combine4BodySelection (
    ... 'charm'            , ## unique name 
    ... [ pions , kaons ] , ## required selections
    ... DecayDescriptor   = '[ D+ -> K- pi+ pi+ pi-]cc'    , 
    ... Combination12Cut  = '  ( AM < 1.8 * GeV ) & ( ACHI2DOCA(1,2)<10 ) ' ,
    ... Combination123Cut = '''( AM < 1.8 * GeV )
    ...                        & ( ACHI2DOCA(1,3)<10 )
    ...                        & ( ACHI2DOCA(2,4)<10 )
    ...                     ''', 
    ... CombinationCut    = ''' in_range  (1.6 * GeV , AM , 2.0 * GeV)
    ...                        & ( ACHI2DOCA(1,4) < 10 )  
    ...                        & ( ACHI2DOCA(2,4) < 10 )
    ...                        & ( ACHI2DOCA(3,4) < 10 )  
    ...                     ''',
    ... MotherCut        = 'in_range  (1.6 * GeV ,  M , 2.0 * GeV) & ( CHI2VX < 10 ) ' ,
    ... )
    """
    #
    from GaudiConfUtils.ConfigurableGenerators import DaVinci__N4BodyDecays as _COMBINER_
    ## check decay descriptor(s)
    if not kwargs.has_key ('DecayDescriptor' ) : 
        if not kwargs.has_key ('DecayDescriptors' ) :
            raise TypeError, 'Combine4BodySelection: DecayDescriptor(s) must be specified!'
    
    ## create selection    
    return SimpleSelection ( name   , _COMBINER_ ,
                             inputs ,
                             Combination12Cut  = Combination12Cut  ,
                             Combination123Cut = Combination123Cut ,
                             CombinationCut    = CombinationCut    ,
                             MotherCut         = MotherCut         , **kwargs )




# =============================================================================
## Useful shortcut for "selection" bases on DecayTreeTuple 
#  @code
#  charm    = .... ## some selectiton object 
#  my_tuple = TupleSelection (
#     'charm'            , ## unique name 
#      [ charm ]         , ## required selections
#      Decay             = '[ D+ -> ^K- ^pi+ ^pi+]CC' ,
#      ToolList          = [ ... ]      
#      )
#  # get the congigurable (for subsequent modification, if needed)
#  algo = mu_tuple.algorithm()
#  # e.g. define the branches: 
#  algo.addBranches ( ... )
#  # make use of nice decoration yb Rob Lambert 
#  tool1 = algo.addTupleTool ( .....  )
#  ...
#  @endcode 
#  @see SimpleSelection
#  @see Selection
#  @see SelPy.Selection
#  @see DecayTreeTuple 
#
#  @param name              unique selection name 
#  @param inputs            list of input/required selection
#  @param Decay              'Decay'-property of DecayTreeTuple 
#  @param kwargs          additional arguments to be used for algorithm
#  @return the selection object
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2016-03-10 
def TupleSelection ( name     ,
                     inputs   ,
                     Decay    , 
                     **kwargs ) :
    """ Useful shortcut for selection with  
    >>> charm    = .... ## some selectiton object 
    >>> my_tuple = TupleSelection (
    ...            'charm'          , ## unique name 
    ...           [ charm ]         , ## required selections
    ...             Decay             = '[ D+ -> ^K- ^pi+ ^pi+]CC' ,
    ...             ToolList          = [ ... ]      
    ...             )
    >>> algo = mu_tuple.algorithm()
    >>> algo.addBranches ( ... )
    ##  make use of nice decoration yb Rob Lambert 
    >>> tool1 = algo.addTupleTool ( .....  )
    """
    #
    from   GaudiConfUtils.ConfigurableGenerators import DecayTreeTuple as _TUPLE_
    ## make use of nice decoration by Rob Lambert:
    import DecayTreeTuple.Configuration 
    ## create the selection    
    return SimpleSelection ( name          ,
                             _TUPLE_       ,
                             inputs        ,
                             Decay = Decay ,
                             **kwargs      ) 



# =============================================================================
## @class MomentumScaling
#  Pseudo-selection that allows to embedd the momentum
#  scaling algorithm into the overall flow
#  @code
#  selection = ....  ## some selection
#  selection = MomentumScaling ( selection ) 
#  @endcode
#  @see TrackScaleState
#  @attention it applies GLOBAL momentum scaling for *all* tracks!
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2016-02-25
class MomentumScaling(UniquelyNamedObject ,
                      ClonableObject      ,
                      SelectionBase       ) :
    """  Pseudo-selection that allows to embedd the momentum
    scaling algorithm into the overall flow
    
    >>> selection = ....  ## some selection
    >>> selection = MomentumScaling ( selection )
    Attention: it applies GLOBAL momentum scaling for *all* tracks!
    """
    def __init__(self              ,
                 RequiredSelection ) :

        ## construct more or less unique name 
        name = '%s_SCALE'
        name = name % RequiredSelection.name()
        
        UniquelyNamedObject . __init__( self , name     )
        ClonableObject      . __init__( self , locals() )
        
        checkName(self.name())

        from Configurables import TrackScaleState as _SCALER         
        _alg = _SCALER('SCALER')   ## NB: THE NAME is fixed, is COMMON INSTANCE
        
        SelectionBase.__init__( self                      ,
                                algorithm          = _alg ,
                                outputLocation     = RequiredSelection.outputLocation(),
                                requiredSelections = [RequiredSelection] )
        
# =============================================================================
# The END 
# =============================================================================
