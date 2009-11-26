#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.34 2009-11-26 10:50:01 panmanj Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.34 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
from Hlt1Lines.Hlt1LumiLines     import Hlt1LumiLinesConf
from Hlt1Lines.Hlt1BeamGasLines  import Hlt1BeamGasLinesConf
from Hlt1Lines.Hlt1L0Lines       import Hlt1L0LinesConf
from Hlt1Lines.Hlt1VeloLines     import Hlt1VeloLinesConf
from Hlt1Lines.Hlt1ExpressLines  import Hlt1ExpressLinesConf
from Hlt1Lines.Hlt1MuonLines     import Hlt1MuonLinesConf
from Hlt1Lines.Hlt1HadronLines   import Hlt1HadronLinesConf
from Hlt1Lines.Hlt1PhotonLines   import Hlt1PhotonLinesConf
from Hlt1Lines.Hlt1ElectronLines import Hlt1ElectronLinesConf
from Hlt1Lines.Hlt1CosmicLines   import Hlt1CosmicLinesConf
from Hlt1Lines.Hlt1MiniBiasLines import Hlt1MiniBiasLinesConf
from Hlt1Lines.Hlt1NoBiasLines import Hlt1NoBiasLinesConf



class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = [ Hlt1CommissioningLinesConf
                            , Hlt1LumiLinesConf
                            , Hlt1BeamGasLinesConf
                            , Hlt1L0LinesConf
                            , Hlt1VeloLinesConf
                            , Hlt1ExpressLinesConf
                            , Hlt1MuonLinesConf
                            , Hlt1HadronLinesConf
                            , Hlt1PhotonLinesConf
                            , Hlt1ElectronLinesConf
                            , Hlt1CosmicLinesConf
                            , Hlt1MiniBiasLinesConf
                            , Hlt1NoBiasLinesConf
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
