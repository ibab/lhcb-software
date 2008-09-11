#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.3 2008-09-11 19:50:09 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================

from Gaudi.Configuration import * 
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSummaryWriter
from Configurables       import HltSelectionFilter, HltSelectionToTES
from Configurables       import HltDecisionFilter
from Configurables       import HltDecReportsMaker, HltDecReportsWriter
from Configurables       import HltRoutingBitsWriter
from Configurables       import HltLumiWriter
from Configurables       import bankKiller
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import addHlt1Prop

# add a few thing to our printout
addHlt1Prop('routingBitDefinitions')
addHlt1Prop('Accept')

importOptions('$HLTCONFROOT/options/HltInit.opts')
# importOptions('$HLTCONFROOT/options/HltMain.py')

summaryWriter = HltSummaryWriter( Save = list(hlt1Selections()['All']) )

Hlt1Decision =  HltDecisionFilter('Hlt1Decision'
                                , Accept = ' | '.join( hlt1Decisions() )
                                )

### TODO: check dependencies, and re-order if needed! 
Hlt1 = Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
               , Members = [ i.sequencer() for i in  hlt1Lines() ] + [ Hlt1Decision ] 
               )

triggerBits = HltRoutingBitsWriter( routingBitDefinitions = 
                                  { 1 : 'Hlt1Decision'
                                  , 2 : 'Hlt1RandomDecision|Hlt1PhysicsDecision|Hlt1LumiDecision'
                                  , 3 : 'Hlt1RandomDecision'
                                  , 4 : 'Hlt1PhysicsDecision'
                                  , 5 : 'Hlt1LumiDecision'
                                  })


lumiDecision = 'Hlt1LumiDecision'
allExceptLumi = ' | '.join([ i for i in hlt1Decisions() if i != lumiDecision ])
lumiOnly = lumiDecision + ' & !( ' + allExceptLumi + ')'


rawbankLumiStripper = Sequence( 'LumiStripper'
                              , IgnoreFilterPassed = True
                              , Members = 
                              [ HltDecisionFilter('LumiOnlyFilter' , Accept = lumiOnly )
                              , bankKiller( BankTypes=[ "ODIN","HltLumiSummary"],  DefaultIsKill=True )
                              ]
                              )

Hlt  = Sequence('Hlt')
Hlt.Members = [ Hlt1
              , Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False
                        , Members = [ HltDecReportsWriter()
                                    , triggerBits
                                    , HltLumiWriter()
                                    #, rawbankLumiStripper
                                    ] )
              ]

print Hlt
