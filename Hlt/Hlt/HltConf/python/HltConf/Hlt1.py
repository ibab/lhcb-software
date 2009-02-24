#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.5 2009-02-24 14:54:58 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Configurables       import GaudiSequencer as Sequence
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


class Hlt1Conf(LHCbConfigurableUser):
   __slots__ = { "LumiBankKillerAcceptFraction" : 0 # fraction of lumi-only events where raw event is stripped down; online: 0.9999
               }

   def __apply_configuration__(self):
        # add a few thing to our printout
        addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                    , 'Code'
                    , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                    , 'OutputSelection','Context' ])

        importOptions('$HLTCONFROOT/options/HltInit.py')
        importOptions('$HLTCONFROOT/options/HltLumiInit.py')

        ## set triggerbits
        #  0-31: reserved for L0  // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        ### NOTE: any change in the meaning of any of the following needs to be 
        ###       communicated with online, to insure the events are still properly
        ###       routed!!!
        routingBits = { 32 : "HLT_PASS('Hlt1Global')"
                      , 33 : "HLT_PASS('Hlt1LumiDecision')"
                      , 34 : "HLT_PASS('Hlt1IgnoringLumiDecision')"
                      , 35 : "HLT_PASS('Hlt1VeloClosingDecision')"
                      , 36 : "HLT_PASS('Hlt1ExpressDecision')"
                      }

        ## and record the settings in the ANN service
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )


        ## finally, define the Hlt1 sequence!!
        Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )

        Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False , Members = 
                    [ HltGlobalMonitor()
                    , HltDecReportsMaker()
                    , HltDecReportsWriter()
                    , HltSelReportsMaker()
                    , HltSelReportsWriter()
                    , HltVertexReportsMaker() # we set this in a postconfig action in HltConf's Configuration.py
                    , HltVertexReportsWriter()
                    , HltRoutingBitsWriter( RoutingBits = routingBits )
                    , HltLumiWriter()
                    , Sequence( 'LumiStripper' , Members = 
                          [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS('Hlt1LumiDecision') & ~HLT_PASS('Hlt1IgnoringLumiDecision') " ) 
                          , Prescale('LumiStripperPrescaler',AcceptFraction=self.getProp('LumiBankKillerAcceptFraction')) 
                          , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary','DAQ'],  DefaultIsKill=True )
                          ])
                    ] )
