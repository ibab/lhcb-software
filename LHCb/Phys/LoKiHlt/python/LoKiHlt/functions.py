#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.1.1.1 2008-09-21 14:41:20 ibelyaev Exp $ 
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
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $  "
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

## @see LoKi::Types::O1_Func
O1_Func  = LoKi.Functor                ( _o1 + ',double' ) 
## @see LoKi::Types::O1_Cuts
O1_Cuts  = LoKi.Functor                ( _o1 + ',bool'   ) 
## @see LoKi::Types::O1_Fun
O1_Fun   = LoKi.FunctorFromFunctor     ( _o1 + ',double' ) 
## @see LoKi::Types::O1_Cut
O1_Cut   = LoKi.FunctorFromFunctor     ( _o1 + ',bool'   ) 

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
L0_CHANNEL       = LoKi.L0.ChannelDecision

## @see LoKi::Cuts::L0_CHDECISION
L0_CHDECISION    = LoKi.L0.ChannelDecision

## @see LoKi::Cuts::L0_CHPREDECISION
L0_CHPREDECISION = LoKi.L0.ChannelPreDecision

## @see LoKi::Cuts::L0_CONDITION
L0_CONDITION     = LoKi.L0.ConditionValue 

## @see LoKi::Cuts::L0_DATA
L0_DATA          = LoKi.L0.DataValue

## @see LoKi::Cuts::L0_DECISION
L0_DECISION      = LoKi.L0.Decision ()

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
L0_SUMDECISION   = LoKi.L0.SumDecision () 

## @see LoKi::Cuts::L0_TCK
L0_TCK           = LoKi.L0.Tck ()

## @see LoKi::Cuts::L0_TIMINGBIT 
L0_TIMINGBIT     = LoKi.L0.TimingBit ()

## @see LoKi::Cuts::L0_TRIGGER 
L0_TRIGGER       = LoKi.L0.TriggerDecision 

## @see LoKi::Cuts::L0_TRUE
L0_TRUE          = LoKi.Constant ( _l0 + ',bool'   ) ( True  )

## @see LoKi::Cuts::L0_ZERO
L0_ZERO          = LoKi.Constant ( _l0 + ',double' ) ( 0 )



# =============================================================================
## The concrete O1-functions 
# =============================================================================

## @see LoKi::Cuts::O1_ALL
O1_ALL       = LoKi.Constant( _o1 + ',bool'  ) ( True  )

## @see LoKi::Cuts::O1_BUNCH
O1_BUNCH     = LoKi.O1.BunchId      ()

## @see LoKi::Cuts::O1_BXCURRENT
O1_BXCURRENT = LoKi.O1.BunchCurrent ()

## @see LoKi::Cuts::O1_BXID 
O1_BXID      = LoKi.O1.BXId 

## @see LoKi::Cuts::O1_BXTYP
O1_BXTYP     = LoKi.O1.BXType () 

## @see LoKi::Cuts::O1_CALSTEP
O1_CALSTEP   = LoKi.O1.CalibrationStep () 

## @see LoKi::Cuts::O1_ForceBit 
O1_FORCEBIT  = LoKi.O1.ForceBit () 

## @see LoKi::Cuts::O1_EVTTYP
O1_EVTTYP    = LoKi.O1.EventType () 

## @see LoKi::Cuts::O1_ERRBITS
O1_ERRBITS   = LoKi.O1.ErrorBits () 



## @see LoKi::Cuts::O1_FALSE
O1_FALSE     = LoKi.Constant( _o1 + ',bool'  ) ( False )

## @see LoKi::Cuts::O1_NONE
O1_NONE      = LoKi.Constant( _o1 + ',bool'   ) ( False )

## @see LoKi::Cuts::O1_NULL
O1_NULL      = LoKi.Constant( _o1 + ',double') ( 0 )

## @see LoKi::Cuts::O1_ONE 
O1_ONE       = LoKi.Constant( _o1 + ',double') ( 1 )

## @see LoKi::Cuts::O1_ORBIT 
O1_ORBIT     = LoKi.O1.Orbit  () 

## @see LoKi::Cuts::O1_ROTYP
O1_ROTYP     = LoKi.O1.ReadOutType () 

## @see LoKi::Cuts::O1_RUN 
O1_RUN       = LoKi.O1.Run ()

## @see LoKi::Cuts::O1_RUNNUMBER 
O1_RUNNUMBER = LoKi.O1.RunNumber 

## @see LoKi::Cuts::O1_TCK
O1_TCK       = LoKi.O1.TrgConfKey () 

## @see LoKi::Cuts::O1_TCKEYS 
O1_TCKEYS    = LoKi.O1.Tck 

## @see LoKi::Cuts::O1_TIME
O1_TIME      = LoKi.O1.InTime

## @see LoKi::Cuts::O1_TGRTYP
O1_TRGTYP    = LoKi.O1.TriggerType () 

## @see LoKi::Cuts::O1_TRUE
O1_TRUE      = LoKi.Constant( _o1 + ',bool'  ) ( True )

## @see LoKi::Cuts::O1_ZERO
O1_ZERO      = LoKi.Constant( _o1 + ',double') ( 0 )


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
HLT_HAS      =    LoKi.HLT.HasSelection 

## @see LoKi::Cuts::HLT_NONE
HLT_NONE     =    LoKi.Constant ( _hlt + ',bool'   ) ( False )

## @see LoKi::Cuts::HLT_NPASS
HLT_NPASS    =    LoKi.HLT.NPass () 

## @see LoKi::Cuts::HLT_NULL
HLT_NULL     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )

## @see LoKi::Cuts::HLT_ONE
HLT_ONE      =    LoKi.Constant ( _hlt + ',double' ) ( 1 )

## @see LoKi::Cuts::HLT_PASS
HLT_PASS     =    LoKi.HLT.PassSelection 

## @see LoKi::Cuts::HLT_SIZE
HLT_SIZE     =    LoKi.HLT.Size () 

## @see LoKi::Cuts::HLT_TRUE
HLT_TRUE     =    LoKi.Constant ( _hlt + ',bool'   ) ( True  )

## @see LoKi::Cuts::HLT_ZERO
HLT_ZERO     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )



# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o

# =============================================================================
# The END 
# =============================================================================
