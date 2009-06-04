#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.14 2009-06-04 13:36:05 graven Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.14 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSelReportsMaker, HltSelReportsWriter
from Configurables       import HltDecReportsWriter
from Configurables       import HltVertexReportsMaker, HltVertexReportsWriter
from Configurables       import HltRoutingBitsWriter
from Configurables       import HltLumiWriter
from Configurables       import HltGlobalMonitor
from Configurables       import bankKiller
from Configurables       import LoKi__HDRFilter   as HltFilter
from Configurables       import DeterministicPrescaler as Prescale
from HltLine.HltLine     import addHlt1Prop


class Hlt1Conf(LHCbConfigurableUser):
   __slots__ = { "LumiBankKillerAcceptFraction" : 0.0 # fraction of lumi-only events where raw event is stripped down; online: 0.9999
               , "EnableHltGlobalMonitor"       : True
               , "EnableHltDecReports"          : True
               , "EnableHltSelReports"          : True
               , "EnableHltVtxReports"          : True
               , "EnableHltRoutingBits"         : True
               , "EnableLumiEventWriting"       : True
               }

   def __apply_configuration__(self):
        # add a few thing to our printout
        addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                    , 'Code', 'InputLocations'
                    , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                    , 'OutputSelection','Context' ])

        importOptions('$HLTCONFROOT/options/HltInit.py')

        ## finally, define the Hlt1 sequence!!
        Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )


        # note: the following is a list and not a dict, as we depend on the order of iterating through it!!!
        _list = ( ( "EnableHltGlobalMonitor" , HltGlobalMonitor  )
                , ( "EnableHltDecReports"    , HltDecReportsWriter )
                , ( "EnableHltSelReports"    , [HltSelReportsMaker, HltSelReportsWriter ] )
                , ( "EnableHltVtxReports"    , [HltVertexReportsMaker, HltVertexReportsWriter ] )
                , ( "EnableHltRoutingBits"   , HltRoutingBitsWriter )
                )

        End = Sequence( 'HltEndSequence', IgnoreFilterPassed = True )
        EndMembers = End.Members
        for i in [ i for (k,i) in _list if self.getProp(k) ] :
            if type(i) is not list : i = [ i ]
            # make sure we only instantiate if we actually use it...
            EndMembers += [ j() for j in i ]
        if (self.getProp("EnableLumiEventWriting")) :
            EndMembers += [ HltLumiWriter()
                          , Sequence( 'LumiStripper' , Members = 
                                [ HltFilter('LumiStripperFilter' , Code = "HLT_PASS('Hlt1LumiDecision') & ~HLT_PASS('Hlt1IgnoringLumiDecision') " ) 
                                , Prescale('LumiStripperPrescaler',AcceptFraction=self.getProp('LumiBankKillerAcceptFraction')) 
                                , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary','HltRoutingBits','DAQ' ],  DefaultIsKill=True )
                                ])
                          ] 
