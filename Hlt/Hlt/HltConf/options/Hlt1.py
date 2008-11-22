#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.21 2008-11-22 23:02:25 tskwarni Exp $
# =============================================================================
## @file
#  Configuration of HLT1
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure HLT1 trigger lines
 Note: the actual line definitions are NOT in this file, 
 this script relies on them being available through hlt1Lines()
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.21 $"
# =============================================================================

from Gaudi.Configuration import * 
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSelectionFilter, HltSelectionToTES
from Configurables       import HltDecisionFilter
from Configurables       import HltSelReportsMaker, HltSelReportsWriter
from Configurables       import HltDecReportsMaker, HltDecReportsWriter
from Configurables       import HltVertexReportsMaker, HltVertexReportsWriter
from Configurables       import HltRoutingBitsWriter
from Configurables       import HltLumiWriter
from Configurables       import HltGlobalMonitor
from Configurables       import bankKiller
from Configurables       import LoKi__HDRFilter   as HltFilter
from Configurables       import DeterministicPrescaler as Prescale
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import addHlt1Prop
from HltConf.HltLine     import Hlt1Line   as Line

# add a few thing to our printout
addHlt1Prop([ 'routingBitDefinitions', 'Accept', 'FilterDescriptor'
            , 'Code'
            , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
            , 'OutputSelection','Context' ])

importOptions('$HLTCONFROOT/options/HltInit.opts')
importOptions('$HLTCONFROOT/options/HltLumiInit.opts')


# grab the names of the velo vertices...
veloVertices = [ i for i in hlt1Selections()['All'] if i.startswith('Hlt1Velo')  and i.endswith('Decision') ]

def AnyIgnoring( dec ) :
    return '|'.join([ i for i in hlt1Decisions() if i != dec ])

## set triggerbits
#  0-31: reserved for L0  // need to add L0DU support to routing bit writer
# 32-63: reserved for Hlt1
# 64-91: reserved for Hlt2

#### TODO: check that the used lines are actually in use!!!
### non-existant strings always evaluate to false, and are not an error (yet)
### empty strings always evaluate to false, and are not an error
routingBits = { 32 : 'Hlt1Global'
              , 33 : 'Hlt1LumiDecision'
              , 34 : AnyIgnoring('Hlt1LumiDecision')
              , 35 : '|'.join(veloVertices)
              , 36 : 'Hlt1RandomDecision'
              , 37 : 'Hlt1PhysicsDecision'
              }


## add the Hlt1Global line...
Hlt1Global = Line('Global', HLT = 'HLT_DECISION' )

## the Hlt1sequence...
Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
        , Members = [ i.sequencer() for i in  hlt1Lines() ] # note : should verify order (?) -- global should be last hlt1line! 
        )
Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False , Members = 
            [ HltGlobalMonitor( Hlt1Decisions = list( hlt1Decisions() ) )
            , HltDecReportsMaker()
            , HltDecReportsWriter()
            , HltSelReportsMaker()
            , HltSelReportsWriter()
            , HltVertexReportsMaker( VertexSelections = veloVertices + [ 'PV2D' ] )
            , HltVertexReportsWriter()
            , HltRoutingBitsWriter( routingBitDefinitions = routingBits )
            , HltLumiWriter()
            , Sequence( 'LumiStripper' , Members = 
                  [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS('Hlt1LumiDecision') & ( HLT_NPASS==1 )" ) 
                  , Prescale('LumiStripperPrescaler',AcceptFraction=0.999) # don't strip on 1/1000 random triggers
                  , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                  ])
            ] )
