#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.12 2010-05-17 16:01:39 ibelyaev Exp $ 
# =============================================================================
## @file LoKiHlt/functions.py
#  The full set of useful objects from LoKiHlt library 
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-09-17
# =============================================================================
"""
The full set of useful objects from LoKiHlt library
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl   "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.12 $  "
# =============================================================================


import LoKiCore.decorators as _LoKiCore 

# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb

_o1  = 'const LHCb::ODIN*'
_l0  = 'const LHCb::L0DUReport*'
_hlt = 'const LHCb::HltDecReports*'

# FIXME: ROOT 5 compatibility workaround
# ensure that L0Event dictionary is loaded
cpp.LHCb.L0DUConfig

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::L0_Func
L0_Func  = LoKi.Functor                ( _l0 + ',double' ) 
## @see LoKi::Types::L0_Cuts
L0_Cuts  = LoKi.Functor                ( _l0 + ',bool'   ) 
## @see LoKi::Types::L0_Fun
L0_Fun   = LoKi.FunctorFromFunctor     ( _l0 + ',double' ) 
## @see LoKi::Types::L0_Cut
L0_Cut   = LoKi.FunctorFromFunctor     ( _l0 + ',bool'   ) 

## @see LoKi::Types::ODIN_Func
ODIN_Func  = LoKi.Functor                ( _o1 + ',double' ) 
## @see LoKi::Types::ODIN_Cuts
ODIN_Cuts  = LoKi.Functor                ( _o1 + ',bool'   ) 
## @see LoKi::Types::ODIN_Fun
ODIN_Fun   = LoKi.FunctorFromFunctor     ( _o1 + ',double' ) 
## @see LoKi::Types::ODIN_Cut
ODIN_Cut   = LoKi.FunctorFromFunctor     ( _o1 + ',bool'   ) 

## @see LoKi::Types::HLT_Func
HLT_Func = LoKi.Functor                ( _hlt + ',double' ) 
## @see LoKi::Types::HLT_Cuts
HLT_Cuts = LoKi.Functor                ( _hlt + ',bool'   ) 
## @see LoKi::Types::HLT_Fun
HLT_Fun  = LoKi.FunctorFromFunctor     ( _hlt + ',double' ) 
## @see LoKi::Types::HLT_Cut
HLT_Cut  = LoKi.FunctorFromFunctor     ( _hlt + ',bool'   ) 



# =============================================================================
## The concrete L0-functions 
# =============================================================================

## @see LoKi::Cuts::L0_ALL 
L0_ALL           = LoKi.Constant ( _l0 + ',bool'   ) ( True  )

## @see LoKi::Cuts::L0_CHANNEL 
L0_CHANNEL        = LoKi.L0.ChannelDecision
## @see LoKi::Cuts::L0_CHANNEL_SUBSTR 
L0_CHANNEL_SUBSTR = LoKi.L0.ChannelDecisionSubString
## @see LoKi::Cuts::L0_CHANNEL_RE
L0_CHANNEL_RE     = LoKi.L0.ChannelDecisionRegex 

## @see LoKi::Cuts::L0_CHDECISION
L0_CHDECISION    = LoKi.L0.ChannelDecision

## @see LoKi::Cuts::L0_CHPREDECISION
L0_CHPREDECISION = LoKi.L0.ChannelPreDecision
## @see LoKi::Cuts::L0_CHPREDECISION_SUBSTR
L0_CHPREDECISION_SUBSTR = LoKi.L0.ChannelPreDecisionSubString
## @see LoKi::Cuts::L0_CHPREDECISION_RE
L0_CHPREDECISION_RE     = LoKi.L0.ChannelPreDecisionRegex 

## @see LoKi::Cuts::L0_CONDITION
L0_CONDITION     = LoKi.L0.ConditionValue 

## @see LoKi::Cuts::L0_DATA
L0_DATA          = LoKi.L0.DataValue

## @see LoKi::Cuts::L0_DECISION
L0_DECISION         = LoKi.L0.Decision
## @see LoKi::Cuts::L0_DECISION_PHYSICS 
L0_DECISION_PHYSICS = LoKi.L0.Decision( LHCb.L0DUDecision.Physics ) 
## @see LoKi::Cuts::L0_DECISION_BEAM1
L0_DECISION_BEAM1   = LoKi.L0.Decision( LHCb.L0DUDecision.Beam1   ) 
## @see LoKi::Cuts::L0_DECISION_BEAM2
L0_DECISION_BEAM2   = LoKi.L0.Decision( LHCb.L0DUDecision.Beam2   ) 


## @see LoKi::Cuts::L0_DIGIT
L0_DIGIT         = LoKi.L0.DataDigit

## @see LoKi::Cuts::L0_FALSE
L0_FALSE         = LoKi.Constant ( _l0 + ',bool'   ) ( False )

## @see LoKi::Cuts::L0_FORCEBIT
L0_FORCEBIT      = LoKi.L0.ForceBit () 

## @see LoKi::Cuts::L0_NONE 
L0_NONE          = LoKi.Constant ( _l0 + ',bool'   ) ( False )

## @see LoKi::Cuts::L0_NULL
L0_NULL          = LoKi.Constant ( _l0 + ',double' ) ( 0 )

## @see LoKi::Cuts::L0_ONE 
L0_ONE           = LoKi.Constant ( _l0 + ',double' ) ( 1 )


## @see LoKi::Cuts::L0_SUMET
L0_SUMET         = LoKi.L0.SumEt

## @see LoKi::Cuts::L0_SUMET0
L0_SUMET0        = LoKi.L0.SumEt ( 0 ) 

## @see LoKi::Cuts::L0_SUMDECISION
L0_SUMDECISION          = LoKi.L0.SumDecision  
## @see LoKi::Cuts::L0_SUMDECISION_PHYSICS 
L0_SUMDECISION_PHYSICS  = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Physics )
## @see LoKi::Cuts::L0_SUMDECISION_BEAM1 
L0_SUMDECISION_BEAM1    = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Beam1   )
## @see LoKi::Cuts::L0_SUMDECISION_BEAM2
L0_SUMDECISION_BEAM2    = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Beam2   )


## @see LoKi::Cuts::L0_TCK
L0_TCK           = LoKi.L0.Tck ()

## @see LoKi::Cuts::L0_TIMINGBIT 
L0_TIMINGBIT     = LoKi.L0.TimingBit ()

## @see LoKi::Cuts::L0_TRIGGER 
L0_TRIGGER       = LoKi.L0.TriggerDecision 
## @see LoKi::Cuts::L0_TRIGGER_SUBSTR 
L0_TRIGGER_SUBSTR = LoKi.L0.TriggerDecisionSubString
## @see LoKi::Cuts::L0_TRIGGER 
L0_TRIGGER_RE     = LoKi.L0.TriggerDecisionRegex 

## @see LoKi::Cuts::L0_TRUE
L0_TRUE          = LoKi.Constant ( _l0 + ',bool'   ) ( True  )

## @see LoKi::Cuts::L0_ZERO
L0_ZERO          = LoKi.Constant ( _l0 + ',double' ) ( 0 )

## @see LoKi::Cuts::L0_VALID
L0_VALID         = LoKi.L0.Valid() 


# =============================================================================
## The concrete O1-functions 
# =============================================================================

## @see LoKi::Cuts::ODIN_ALL
ODIN_ALL       = LoKi.Constant( _o1 + ',bool'  ) ( True  )



## @see LoKi::Cuts::ODIN_BUNCH
ODIN_BUNCH     = LoKi.Odin.BunchId      ()


## @see LoKi::Cuts::ODIN_BXCURRENT
ODIN_BXCURRENT = LoKi.Odin.BunchCurrent ()

## @see LoKi::Cuts::ODIN_BXID 
ODIN_BXID      = LoKi.Odin.BXId 

## @see LoKi::Cuts::ODIN_BXTYP
ODIN_BXTYP     = LoKi.Odin.BXType () 

## @see LoKi::Cuts::ODIN_CALSTEP
ODIN_CALSTEP   = LoKi.Odin.CalibrationStep () 

## @see LoKi::Cuts::ODIN_ForceBit 
ODIN_FORCEBIT  = LoKi.Odin.ForceBit () 

## @see LoKi::Cuts::ODIN_EVTTYP
ODIN_EVTTYP    = LoKi.Odin.EventType () 

## @see LoKi::Cuts::ODIN_ERRBITS
ODIN_ERRBITS   = LoKi.Odin.ErrorBits () 

## @see LoKi::Cuts::ODIN_FALSE
ODIN_FALSE     = LoKi.Constant( _o1 + ',bool'  ) ( False )

## @see LoKi::Cuts::ODIN_NONE
ODIN_NONE      = LoKi.Constant( _o1 + ',bool'   ) ( False )

## @see LoKi::Cuts::ODIN_NULL
ODIN_NULL      = LoKi.Constant( _o1 + ',double') ( 0 )

## @see LoKi::Cuts::ODIN_ONE 
ODIN_ONE       = LoKi.Constant( _o1 + ',double') ( 1 )

## @see LoKi::Cuts::ODIN_ORBIT 
ODIN_ORBIT     = LoKi.Odin.Orbit  () 

## @see LoKi::Cuts::ODIN_PRESCALE
ODIN_PRESCALE  = LoKi.Odin.Prescale

## @see LoKi::Cuts::ODIN_ROTYP
ODIN_ROTYP     = LoKi.Odin.ReadOutType () 


## @see LoKi::Cuts::ODIN_RUN 
ODIN_RUN       = LoKi.Odin.Run ()

## @see LoKi::Cuts::ODIN_RUNNUMBER 
ODIN_RUNNUMBER = LoKi.Odin.RunNumber 

## @see LoKi::Cuts::ODIN_EVTNUMBER 
ODIN_EVTNUMBER = LoKi.Odin.EvtNumber 

## @see LoKi::Cuts::ODIN_RUNEVTNUMBER 
ODIN_RUNEVTNUMBER = LoKi.Odin.RunEvtNumber 
## @see LoKi::Cuts::ODIN_RUNEVT
ODIN_RUNEVT       = LoKi.Odin.RunEvtNumber 

## @see LoKi::Cuts::ODIN_EVT1
ODIN_EVT1 = LoKi.Odin.Evt1

## @see LoKi::Cuts::ODIN_EVT2
ODIN_EVT2 = LoKi.Odin.Evt2



## @see LoKi::Cuts::ODIN_TCK
ODIN_TCK       = LoKi.Odin.TrgConfKey () 

## @see LoKi::Cuts::ODIN_TCKEYS 
ODIN_TCKEYS    = LoKi.Odin.Tck 

## @see LoKi::Cuts::ODIN_TIME
ODIN_TIME      = LoKi.Odin.InTime

## @see LoKi::Cuts::ODIN_TGRTYP
ODIN_TRGTYP    = LoKi.Odin.TriggerType () 

## @see LoKi::Cuts::ODIN_TRUE
ODIN_TRUE      = LoKi.Constant( _o1 + ',bool'  ) ( True )

## @see LoKi::Cuts::ODIN_ZERO
ODIN_ZERO      = LoKi.Constant( _o1 + ',double') ( 0 )

## @see LoKi::Cuts::ODIN_ROUTINBITS
ODIN_ROUTINGBITS    = LoKi.Odin.RoutingBits


# =============================================================================
# concrete HLT functions 
# =============================================================================

## @see LoKi::Cuts::HLT_ALL 
HLT_ALL      =    LoKi.Constant ( _hlt + ',bool'   ) ( True  )

## @see LoKi::Cuts::HLT_DECISION
HLT_DECISION =    LoKi.HLT.Decision ()  

## @see LoKi::Cuts::HLT_FALSE 
HLT_FALSE    =    LoKi.Constant ( _hlt + ',bool'   ) ( False )

## @see LoKi::Cuts::HLT_HAS
HLT_HAS        =    LoKi.HLT.HasDecision
## @see LoKi::Cuts::HLT_HAS_SUBSTR
HLT_HAS_SUBSTR =    LoKi.HLT.HasDecisionSubString
## @see LoKi::Cuts::HLT_HAS_RE
HLT_HAS_RE     =    LoKi.HLT.HasDecisionRegex

## @see LoKi::Cuts::HLT_NONE
HLT_NONE     =    LoKi.Constant ( _hlt + ',bool'   ) ( False )

## @see LoKi::Cuts::HLT_NPASS
HLT_NPASS    =    LoKi.HLT.NPass () 

## @see LoKi::Cuts::HLT_NULL
HLT_NULL     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )

## @see LoKi::Cuts::HLT_ONE
HLT_ONE      =    LoKi.Constant ( _hlt + ',double' ) ( 1 )

## @see LoKi::Cuts::HLT_PASS
HLT_PASS        =    LoKi.HLT.PassDecision 
## @see LoKi::Cuts::HLT_PASS_SUBSTR
HLT_PASS_SUBSTR =    LoKi.HLT.PassDecisionSubString
## @see LoKi::Cuts::HLT_PASS_RE
HLT_PASS_RE     =    LoKi.HLT.PassDecisionRegex
## @see LoKi::Cuts::HLT_TURBOPASS_RE
HLT_TURBOPASS_RE     =    LoKi.HLT.TurboPass
## @see LoKi::Cuts::HLT_NONTURBOPASS_RE
HLT_NONTURBOPASS_RE     =    LoKi.HLT.NonTurboPass

## @see LoKi::Cuts::HLT_PASSIGNORING 
HLT_PASSIGNORING         =    LoKi.HLT.DecisionBut
## @see LoKi::Cuts::HLT_PASSIGNORING_SUBSTR 
HLT_PASSIGNORING_SUBSTR  =    LoKi.HLT.DecisionButSubString
## @see LoKi::Cuts::HLT_PASSIGNORING_RE
HLT_PASSIGNORING_RE      =    LoKi.HLT.DecisionButRegex

## @see LoKi::Cuts::HLT_SIZE
HLT_SIZE     =    LoKi.HLT.Size () 

## @see LoKi::Cuts::HLT_TRUE
HLT_TRUE     =    LoKi.Constant ( _hlt + ',bool'   ) ( True  )

## @see LoKi::Cuts::HLT_ZERO
HLT_ZERO     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )


## @see LoKi::Cuts::HLT_ERRORBITS 
HLT_ERRORBITS        = LoKi.HLT.ErrorBits

## @see LoKi::Cuts::HLT_EXECUTIONSTAGE
HLT_EXECUTIONSTAGE   = LoKi.HLT.ExecutionStage

## @see LoKi::Cuts::HLT_NCANDIDATES
HLT_NCANDIDATES       = LoKi.HLT.NumberOfCandidates

## @see LoKi::Cuts::HLT_SATURATED
HLT_SATURATED          = LoKi.HLT.Saturated 

## @see LoKi::Cuts::HLT_COUNTERRORBITS 
HLT_COUNT_ERRORBITS    = LoKi.HLT.CountErrorBits  

## @see LoKi::Cuts::HLT_COUNTERRORBITS_RE 
HLT_COUNT_ERRORBITS_RE = LoKi.HLT.CountErrorBitsRegex  

## @see LoKi::Cuts::HLT_ROUTINBITS
HLT_ROUTINGBITS        = LoKi.HLT.HltRoutingBits

## @see LoKi::Cuts::HLT_TCK
HLT_TCK                = LoKi.HLT.TrgConfKey ()


## @see LoKi::Cuts::ROUTINBITS
ROUTINGBITS            = LoKi.HLT.RoutingBits

## @see LoKi::Numbers::EvtNum 
EvtNum     = LoKi.Numbers.EvtNum 
## @see LoKi::Numbers::EvtNumList
EvtNumList = LoKi.Numbers.EvtNumList 
## @see LoKi::Numbers::RunEvt
RunEvt     = LoKi.Numbers.RunEvt
## @see LoKi::Numbers::RunEvtList
RunEvtList = LoKi.Numbers.RunEvtList 

## Make event-number list from fragments :
def _add_1_ ( o1 , o2 ) :
    """
    Make event-number list from fragments :
    >>> o1 = ...
    >>> o2 = ...
    >>> o  = o1 + o2 
    """
    ##
    if isinstance ( o1 , ( int , long ) ) : o1 = EvtNum ( o1 )
    if isinstance ( o2 , ( int , long ) ) : o2 = EvtNum ( o2 )
    ##
    return LoKi.Numbers.add1 ( o1 , o2 )

## Make event-number list from fragments :
def _add_2_ ( o1 , o2 ) :
    """
    Make run/event list from fragments :
    >>> o1 = ...
    >>> o2 = ...
    >>> o  = o1 + o2 
    """
    ##
    if isinstance ( o1 , tuple ) and 2 == len ( o1 ) : o1 = RunEvt ( o1[0] , o1[1] ) 
    if isinstance ( o2 , tuple ) and 2 == len ( o2 ) : o2 = RunEvt ( o2[0] , o2[1] ) 
    ##
    return LoKi.Numbers.add2 ( o1 , o2 )

_add_1_  . __doc__ += '\n' + LoKi.Numbers.add1.__doc__
_add_2_  . __doc__ += '\n' + LoKi.Numbers.add2.__doc__

def _iter_ ( s ) :
    """
    Iterate over (run)event list
    """
    _s = len ( s )
    _i = 0
    while _i < _s :
        yield s.at( _i )
        _i += 1
        
for t in ( EvtNum , EvtNumList ) :
    t.__add__  = _add_1_
    t.__radd__ = _add_1_
    
for t in ( RunEvt , RunEvtList ) :
    t.__add__  = _add_2_
    t.__radd__ = _add_2_

for t in ( EvtNumList , RunEvtList ) :
    t.__len__      = t.size 
    t.__iter__     = _iter_
    t.__contains__ = t.contains  

for t in ( EvtNum , EvtNumList ,
           RunEvt , RunEvtList ) :
    t.__str__  = t.toString
    t.__repr__ = t.toString
    t.__hash__ = t.hash
    if hasattr ( t , '__cpp_eq__' ) :  t.__eq__   = t.__cpp_eq__
    if hasattr ( t , '__cpp_ne__' ) :  t.__ne__   = t.__cpp_ne__

        
## @see LoKi::TES::Get 
GET = LoKi.TES.Get
#
GET.toString = lambda s : s.printOut ()
GET.__repr__ = lambda s : s.printOut ()
GET.__str__  = lambda s : s.printOut ()
#
## merge the getter with the subsequent calls 
def _get_rshift_ ( self , func ) :
    """
    Merge ``getter'' with other functions
    
    >>> getter = GET('/Event/DAQ/ODIN')
    >>> func   = getter >> ODIN_TCK

    In particular it allows insertion of  various functors (ODIN,L0,Hlt)
    into Hlt1-streamers.
    
    """
    return LoKi.TES.TESGet.get ( self , func ) 

_get_rshift_ . __doc__ += '\n' + LoKi.TES.TESGet.get .__doc__
GET. __rshift__ = _get_rshift_

# =============================================================================
## build the vector of event-numbers from fragments 
def evtnum_lst ( arg1 , *arg ) :
    """
    Build the vector of event-numbers from fragments
    """
    from LoKiCore.functions import _make_vct_ , _append_ 
    vct = EvtNumList () 
    return _make_vct_ ( vct      ,
                        arg1     ,
                        _append_ , 
                        *arg     )

# =============================================================================
## build the vector of event-numbers from fragments 
def runevt_lst ( arg1 , *arg ) :
    """
    Build the vector of event-numbers from fragments
    """
    from LoKiCore.functions import _make_vct_ , _append_
    vct = RunEvtList ()
    ## 
    return _make_vct_ ( vct      ,
                        arg1     ,
                        _append_ , 
                        *arg     ) 
    
# =============================================================================
## helper function to define properly ODIN_EVTNUMBER predicate
#  @see LoKi::Cuts::ODIN_EVTNUMBER
#  @see LoKi::Odin::EvtNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_events ( arg1 , *arg ) :
    """
    Helper function to define properly ODIN_EVTNUMBER predicate
    """
    if   0 == len ( arg ) and issubclass ( type(arg1) , ( int , long , EvtNum ) ) : 
        return ODIN_EVTNUMBER ( EvtNum ( arg1 ) )
    elif 1 == len ( arg ) :
        if issubclass ( type(arg1) , ( int , long , EvtNum ) ) :
            arg2=arg[0]
            if issubclass ( type(arg2) , ( int , long , EvtNum ) ) :
                return ODIN_EVTNUMBER ( EvtNum ( arg1 )  , EvtNum ( arg2  ) )
        raise TypeError ("invalid signature")
    
    ##
    evts = evtnum_lst ( arg1 , *arg ) 
    ##
    return ODIN_EVTNUMBER ( evts )

# =========================================================================
## helper function to define properly ODIN_RUNNUMBER predicate
#  @see LoKi::Cuts::ODIN_RUNNUMBER
#  @see LoKi::Odin::RunNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_runs ( arg1 , *args ) :
    """
    Helper function to define properly ODIN_RUNNUMBER predicate
    """
    
    if   0 == len ( args ) and issubclass ( type(arg1) , ( int , long ) ) : 
        return ODIN_RUNNUMBER ( arg1 )
    elif 1 == len ( args ) :
        if issubclass ( type(arg1) , ( int , long ) ) :
            arg2=args[0]
            if issubclass ( type(arg2) , ( int , long ) ) :
                return ODIN_RUNNUMBER ( arg1 , arg2 )
        raise TypeError ("invalid signature") 
                        
    from LoKiCore.functions import uings 

    runs = uints ( arg1 , *args )
    
    return ODIN_RUNNUMBER ( runs )


# =========================================================================
## helper function to define properly ODIN_RUNEVTNUMBER predicate
#  @see LoKi::Cuts::ODIN_RUNEVTNUMBER
#  @see LoKi::Odin::RunEvtNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_runevts ( arg1 , *arg ) :
    """
    Helper function to define properly ODIN_RUNEVTNUMBER predicate
    """
    if   0 == len ( arg ) and issubclass ( type(arg1) ,  ( RunEvt , RunEvtList ) ) : 
        return ODIN_RUNEVT ( arg1 )
    elif 1 == len ( arg ) :
        if issubclass ( type(arg1) , EvtRun  ) :
            arg2=arg[0]
            if issubclass ( type(arg2) , EvtRun  ) :
                return ODIN_RUNEVT ( arg1 , arg2 )
        raise TypeError ("invalid signature") 
                        
    evts = runevt_lst ( arg1 , *arg ) 
        
    return ODIN_RUNEVT ( evts )
    
# =============================================================================
## helper function to define (void)functor  for routing bits 
#  @see LoKi::Cuts::ROUTINGBITS
#  @see LoKi::Cuts::ROUTINGBIT
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def routingBits ( arg1 , *args ) :
    """
    Helper function to define (void)functor  for routing bits
    """
    from LoKiCore.functions import uints
    bits = uints ( arg1 , *args )
    return ROUTINGBITS ( bits )

# =============================================================================
## helper function to define HLT-functor  for routing bits 
#  @see LoKi::Cuts::HLT_ROUTINGBITS
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def HLT_routingBits ( arg1 , *args ) :
    """
    Helper function to define HLT-functor  for routing bits
    """
    return HLT_ROUTINGBITS ( routingBits ( arg1 , *args ) ) 

# =============================================================================
## helper function to define ODIN-functor  for routing bits 
#  @see LoKi::Cuts::ODIN_ROUTINGBITS
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def ODIN_routingBits ( arg1 , *args ) :
    """
    Helper function to define ODIN-functor  for routing bits
    """
    return ODIN_ROUTINGBITS ( routingBits ( arg1 , *args ) ) 

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o

# =============================================================================
# The END 
# =============================================================================
