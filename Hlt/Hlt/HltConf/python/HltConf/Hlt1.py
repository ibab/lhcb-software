#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.32 2009-10-23 10:05:59 snies Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.32 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
from Hlt1Lines.HltLumiLines     import HltLumiLinesConf
from Hlt1Lines.HltBeamGasLines  import HltBeamGasLinesConf
from Hlt1Lines.HltL0Lines       import HltL0LinesConf
from Hlt1Lines.HltVeloLines     import HltVeloLinesConf
from Hlt1Lines.HltExpressLines  import HltExpressLinesConf
from Hlt1Lines.HltMuonLines     import HltMuonLinesConf
from Hlt1Lines.HltHadronLines   import HltHadronLinesConf
from Hlt1Lines.HltPhotonLines   import HltPhotonLinesConf
from Hlt1Lines.HltElectronLines import HltElectronLinesConf
from Hlt1Lines.HltCosmicLines   import HltCosmicLinesConf



class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = [ HltCommissioningLinesConf
                            , HltLumiLinesConf
                            , HltBeamGasLinesConf
                            , HltL0LinesConf
                            , HltVeloLinesConf
                            , HltExpressLinesConf
                            , HltMuonLinesConf
                            , HltHadronLinesConf
                            , HltPhotonLinesConf
                            , HltElectronLinesConf
                            , HltCosmicLinesConf
                            ]

   __slots__ = { "ThresholdSettings"            : {} # dictionary decoded in HltThresholdSettings
               }

##################################################################################
#
#      
   def __apply_configuration__(self):
      from Configurables       import GaudiSequencer as Sequence
      from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
      ThresholdSettings = self.getProp("ThresholdSettings")
      
      for i in  self.__used_configurables__  :
         if not issubclass(i,HltLinesConfigurableUser) : continue
         from ThresholdUtils import setThresholds
         setThresholds(ThresholdSettings,i)
         
      # add a few thing to our printout
      from HltLine.HltLine     import addHlt1Prop
      addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                  , 'Code', 'InputLocations'
                  , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                  , 'OutputSelection','Context' ])
      
      importOptions('$HLTCONFROOT/options/HltInit.py')
      
      ## finally, define the Hlt1 sequence!!
      Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )

      from HltLine.HltLine     import Hlt1Line
      Hlt1Line( 'Global', HLT= "HLT_PASS_SUBSTR('Hlt1') ", priority = 255 ) 
