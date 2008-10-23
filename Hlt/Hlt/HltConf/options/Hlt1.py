#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.13 2008-10-23 13:45:45 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.13 $"
# =============================================================================

from Gaudi.Configuration import * 
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSummaryWriter
from Configurables       import HltSelectionFilter, HltSelectionToTES
from Configurables       import HltDecisionFilter
from Configurables       import HltDecReportsMaker, HltDecReportsWriter
from Configurables       import HltVertexReportsMaker, HltVertexReportsWriter
from Configurables       import HltRoutingBitsWriter
from Configurables       import HltLumiWriter
from Configurables       import HltGlobalMonitor
from Configurables       import bankKiller
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import addHlt1Prop
from HltConf.HltLine     import Hlt1Line   as Line

# add a few thing to our printout
for i in [ 'routingBitDefinitions', 'Accept', 'FilterDescriptor', 'Code' ] :
    addHlt1Prop(i)

importOptions('$HLTCONFROOT/options/HltInit.opts')
importOptions('$HLTCONFROOT/options/HltLumiInit.opts')
# importOptions('$HLTCONFROOT/options/HltMain.py')


Hlt1Global = Line('Global', HLT = 'HLT_DECISION' )

Hlt1 = Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
               , Members = [ i.sequencer() for i in  hlt1Lines() ]
               )

## needed to feed HltVertexReportsMaker... needed for Velo!
# run for all selections which have 'velo' in them
summaryWriter = HltSummaryWriter( Save = list(hlt1Selections()['All']) )
vertexMaker = HltVertexReportsMaker( VertexSelections = [ 'Hlt1VeloASideVFDecision','Hlt1VeloCSideVFDecision']  )
vertexWriter =  HltVertexReportsWriter( )

veloVertex = Sequencer( 'VeloVertex',  Members = [ summaryWriter, vertexMaker, vertexWriter ])

def XOnly( dec ) :
    return dec + '&!(' + '|'.join([ i for i in hlt1Decisions() if i != dec ]) + ') '

def AnyIgnoring( dec ) :
    return '|'.join([ i for i in hlt1Decisions() if i != dec ])

## set triggerbits
#  0-31: reserved for L0  // need to add L0DU support to routing bit writer
# 32-63: reserved for Hlt1
# 64-91: reserved for Hlt2

#### TODO: check that the used lines are actually in use!!!
### non-existant strings always evaluate to false, and are not an error (yet)
routingBits = { 32 : 'Hlt1Global'
              , 33 : 'Hlt1LumiDecision'
              , 34 : AnyIgnoring('Hlt1LumiDecision')
              , 35 : 'Hlt1VeloASideDecision|Hlt1VeloCSideDecision'
              , 36 : 'Hlt1RandomDecision'
              , 37 : 'Hlt1PhysicsDecision'
              }

triggerBits = HltRoutingBitsWriter( routingBitDefinitions = routingBits )





rawbankLumiStripper = Sequence( 'LumiStripper'
                              , IgnoreFilterPassed = True
                              , Members = 
                              [ HltDecisionFilter('LumiOnlyFilter' , Accept = XOnly( 'Hlt1LumiDecision' ) )
                              , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                              ])

Hlt  = Sequence('Hlt')
Hlt.Members = [ Hlt1
              , Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False
                        , Members = [ HltGlobalMonitor( Hlt1Decisions = list( hlt1Decisions() ) )
                                    , HltDecReportsWriter()
                                    , veloVertex
                                    , triggerBits
                                    , HltLumiWriter()
                                    #, rawbankLumiStripper
                                    ] )
              ]
