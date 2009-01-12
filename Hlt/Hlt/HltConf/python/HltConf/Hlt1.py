#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.1 2009-01-12 11:03:12 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
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
   __slots__ = { }

   def __apply_configuration__(self):
        # add a few thing to our printout
        addHlt1Prop([ 'routingBitDefinitions', 'Accept', 'FilterDescriptor'
                    , 'Code'
                    , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                    , 'OutputSelection','Context' ])

        importOptions('$HLTCONFROOT/options/HltInit.py')
        importOptions('$HLTCONFROOT/options/HltLumiInit.py')

        # Lines which are 'always' there...


        ## set triggerbits
        #  0-31: reserved for L0  // need to add L0DU support to routing bit writer
        # 32-63: reserved for Hlt1
        # 64-91: reserved for Hlt2

        #### TODO: check that the used lines are actually in use!!!
        ### non-existant strings always evaluate to false, and are not an error (yet)
        ### empty strings always evaluate to false, and are not an error

        routingBits = { 32 : 'Hlt1Global'
                      , 33 : 'Hlt1LumiDecision'
                      , 34 : 'Hlt1IgnoringLumiDecision' 
                      , 35 : 'Hlt1VeloClosingDecision'
                      , 36 : 'Hlt1ExpressDecision'
                      }

        ## and record the settings in the ANN service
        HltANNSvc().RoutingBits = dict( [ (v,k) for k,v in routingBits.iteritems() ] )


        ## finally, define the Hlt1 sequence!!
        #### TODO: add a setup where only one line is run, so that we can verify the independence
        ####       of the lines!!!
        Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )

        Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False , Members = 
                    [ HltGlobalMonitor()
                    , HltDecReportsMaker()
                    , HltDecReportsWriter()
                    , HltSelReportsMaker()
                    , HltSelReportsWriter()
                    , HltVertexReportsMaker() # we set this in a postconfig action in HltConf's Configuration.py
                    , HltVertexReportsWriter()
                    , HltRoutingBitsWriter( routingBitDefinitions = routingBits )
                    , HltLumiWriter()
                    , Sequence( 'LumiStripper' , Members = 
                          [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS('Hlt1LumiDecision') & ~HLT_PASS('Hlt1IgnoringLumiDecision') " ) 
                          , Prescale('LumiStripperPrescaler',AcceptFraction=0.9999) # don't strip on 1/10000 random triggers
                          , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                          ])
                    ] )
