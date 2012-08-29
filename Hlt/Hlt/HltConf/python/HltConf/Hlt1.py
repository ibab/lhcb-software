#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.44 2010-08-22 22:50:46 gligorov Exp $
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.44 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *
from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
from Hlt1Lines.Hlt1LumiLines       import Hlt1LumiLinesConf
from Hlt1Lines.Hlt1BeamGasLines    import Hlt1BeamGasLinesConf
from Hlt1Lines.Hlt1L0Lines         import Hlt1L0LinesConf
from Hlt1Lines.Hlt1CosmicLines     import Hlt1CosmicLinesConf
from Hlt1Lines.Hlt1MBLines         import Hlt1MBLinesConf
from Hlt1Lines.Hlt1TrackLines      import Hlt1TrackLinesConf
from Hlt1Lines.Hlt1MuonLines       import Hlt1MuonLinesConf
from Hlt1Lines.Hlt1ProtonLines     import Hlt1ProtonLinesConf
from Hlt1Lines.Hlt1ElectronLines   import Hlt1ElectronLinesConf
from Hlt1Lines.Hlt1HighPtJetsSinglePVLines   import Hlt1HighPtJetsSinglePVLinesConf
from Hlt1Lines.Hlt1DisplVertexLines import Hlt1DisplVertexLinesConf
from Hlt1Lines.Hlt1pALines         import Hlt1pALinesConf

class Hlt1Conf(LHCbConfigurableUser):
   __used_configurables__ = [ Hlt1CommissioningLinesConf
                            , Hlt1LumiLinesConf
                            , Hlt1BeamGasLinesConf
                            , Hlt1L0LinesConf
                            , Hlt1MuonLinesConf
                            , Hlt1TrackLinesConf
                            , Hlt1CosmicLinesConf
                            , Hlt1MBLinesConf
                            , Hlt1ProtonLinesConf  
                            , Hlt1ElectronLinesConf
                            , Hlt1HighPtJetsSinglePVLinesConf                
                            , Hlt1DisplVertexLinesConf
                            , Hlt1pALinesConf  
                            ]

   __slots__ = { "ThresholdSettings"            : {} # dictionary decoded in HltThresholdSettings
               }

##################################################################################
#
#      
   def __apply_configuration__(self):
      ## New event model
      import HltTracking.Hlt1TrackUpgradeConf
      from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
      factory = CoreFactory( "Hlt1Factory" )
      for m in [ "LoKiCore.decorators"    ,
                 "LoKiTracks.decorators"  ,
                 "LoKiTrigger.decorators" ,
                 "LoKiNumbers.decorators" ,
                 "LoKiCore.functions"     ,
                 "LoKiCore.math"          ,
                 "LoKiHlt.algorithms"     ,
                 "LoKiPhys.decorators"    ] :
         if not m in factory.Modules : factory.Modules.append ( m )
      factory.Lines += [
         "from GaudiKernel.SystemOfUnits import GeV, MeV, mm" ,
         "import HltTracking.Hlt1StreamerConf"
         ]

      ## Existing stuff
      from Configurables import GaudiSequencer as Sequence
      from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
      ThresholdSettings = self.getProp("ThresholdSettings")
      
      for i in  self.__used_configurables__  :
         if not issubclass(i,HltLinesConfigurableUser) : continue
         from ThresholdUtils import setThresholds
         setThresholds(ThresholdSettings,i)
         
      # add a few thing to our printout
      from HltLine.HltLine     import addHlt1Prop
      addHlt1Prop([ 'RoutingBits', 'Accept', 'FilterDescriptor'
                  , 'Code', 'Preambulo', 'InputLocations', 'Input','Inputs', 'Output','OutputProtoParticleLocation','InputTrackLocation'
                  , 'DaughtersCuts', 'CombinationCut', 'MotherCut', 'DecayDescriptor'
                  , 'OutputSelection','Context' ])
      
      
      ## finally, define the Hlt1 sequence!!
      Sequence('Hlt1',  ModeOR = True, ShortCircuit = False )

      from HltLine.HltLine     import Hlt1Line
      Hlt1Line( 'Global', HLT= "HLT_PASS_SUBSTR('Hlt1') ", priority = 255 ) 
