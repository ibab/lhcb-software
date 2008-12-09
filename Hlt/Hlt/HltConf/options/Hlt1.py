#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.25 2008-12-09 15:49:58 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.25 $"
# =============================================================================

from Gaudi.Configuration import * 
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSelectionFilter, HltSelectionToTES
from Configurables       import HltDecisionFilter
from Configurables       import HltIncidentFilter
from Configurables       import HltANNSvc
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

importOptions('$HLTCONFROOT/options/HltInit.py')
importOptions('$HLTCONFROOT/options/HltLumiInit.opts')

## add the 'Incident' line...
## Line('Incident', algos = [ HltIncidentFilter() ])

## add a line for 'lumi only'
# note: NPASS = 1: Hlt1LumiDecision, as 'Global' not yet defined here ...
# otherwise, would be NPASS==2!
Line('LumiExclusive', HLT = "HLT_PASS('Hlt1LumiDecision') & ( HLT_NPASS==1 )" ) 

## add the Hlt1Global line...
Line('Global', HLT = 'HLT_DECISION' )

# grab the names of the velo vertices...
veloVertices = [ i for i in hlt1Selections()['All'] if i.startswith('Hlt1Velo')  and i.endswith('Decision') ]

## set triggerbits
#  0-31: reserved for L0  // need to add L0DU support to routing bit writer
# 32-63: reserved for Hlt1
# 64-91: reserved for Hlt2

#### TODO: Push this into ANN service...
#### TODO: check that the used lines are actually in use!!!
### non-existant strings always evaluate to false, and are not an error (yet)
### empty strings always evaluate to false, and are not an error

routingBits = { 32 : 'Hlt1Global'
              , 33 : 'Hlt1LumiDecision'
              , 34 : 'Hlt1LumiExclusiveDecision'
              , 35 : '|'.join(veloVertices)
              , 36 : 'Hlt1RandomDecision'
              , 37 : 'Hlt1PhysicsDecision'
              }

## and record the settings in the ANN service
HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )


## finally, define the Hlt1 sequence!!
#### TODO: add a setup where only one line is run, so that we can verify the independence
####       of the lines!!!
Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
        , Members = [ i.sequencer() for i in  hlt1Lines() ] # note : should verify order (?) -- global should be last hlt1line! 
        )
Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False , Members = 
            [ HltGlobalMonitor( Hlt1Decisions = list( hlt1Decisions() ) )
            , HltDecReportsMaker()
            , HltDecReportsWriter()
            , HltSelReportsMaker()
            , HltSelReportsWriter()
            , HltVertexReportsMaker( VertexSelections = veloVertices  )
            , HltVertexReportsWriter()
            , HltRoutingBitsWriter( routingBitDefinitions = routingBits )
            , HltLumiWriter()
            , Sequence( 'LumiStripper' , Members = 
                  [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS('Hlt1LumiExclusiveDecision') " ) 
                  , Prescale('LumiStripperPrescaler',AcceptFraction=0.999) # don't strip on 1/1000 random triggers
                  , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                  ])
            ] )
