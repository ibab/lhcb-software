#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.16 2008-10-28 15:27:48 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.16 $"
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
from Configurables       import LoKi__HDRFilter   as HltFilter
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import addHlt1Prop
from HltConf.HltLine     import Hlt1Line   as Line

# add a few thing to our printout
for i in [ 'routingBitDefinitions', 'Accept', 'FilterDescriptor', 'Code', 'OutputSelection' ] :
    addHlt1Prop(i)

importOptions('$HLTCONFROOT/options/HltInit.opts')
importOptions('$HLTCONFROOT/options/HltLumiInit.opts')
# importOptions('$HLTCONFROOT/options/HltMain.py')


Hlt1Global = Line('Global', HLT = 'HLT_DECISION' )

Hlt1 = Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
               , Members = [ i.sequencer() for i in  hlt1Lines() ]
               )

# TODO: remove summaryWriter in next release
## needed to feed HltVertexReportsMaker... needed for Velo!
# run for all selections which have 'velo' in them
veloVertices = [ i for i in hlt1Selections()['All'] if i.startswith('Hlt1Velo') ]
summaryWriter = HltSummaryWriter(      Save             = veloVertices ) 
vertexMaker   = HltVertexReportsMaker( VertexSelections = veloVertices )
vertexWriter  = HltVertexReportsWriter( )

# TODO: remove summaryWriter in next release
veloVertex = Sequencer( 'VeloVertex',  Members = [ summaryWriter, vertexMaker, vertexWriter ])

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

rawbankLumiStripper = Sequence( 'LumiStripper' , Members = 
                              [ HltFilter('LumiOnlyFilter' , Code = "HLT_PASS('Hlt1LumiDecision') & ( HLT_NPASS==1 ) " ) 
                              , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                              ])

Hlt  = Sequence('Hlt')
Hlt.Members = [ Hlt1
              , Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False
                        , Members = [ HltGlobalMonitor( Hlt1Decisions = list( hlt1Decisions() ) )
                                    , HltDecReportsWriter()
                                    , veloVertex
                                    , HltRoutingBitsWriter( routingBitDefinitions = routingBits )
                                    , HltLumiWriter()
                                    , rawbankLumiStripper
                                    ] )
              ]
