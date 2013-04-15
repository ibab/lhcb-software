##### LOTS OF IMPORTS... PROBABLY MANY NOT NECESSARY

from Configurables import 	TrackAddNNGhostId   ,RefitParticleTracks, TrackStateInitAlg
from Configurables import RawBankToSTClusterAlg, UnpackTrack, TrackSelector , DecodeVeloRawBuffer, RawBankToSTLiteClusterAlg, TrackContainerCleaner, STOfflinePosition, TrackMonitor
from TrackFitter.ConfiguredFitters import ConfiguredFit
from Configurables import GaudiSequencer
from Configurables import MeasurementProviderT_MeasurementProviderTypes__IT_			#These are the conf. for the MeasurementProvider algorithms
from Configurables import	MeasurementProviderT_MeasurementProviderTypes__TT_			#in charge for looking for cluster information in mDST
from Configurables import MeasurementProviderT_MeasurementProviderTypes__VeloR_ 
from Configurables import	MeasurementProviderT_MeasurementProviderTypes__VeloPhi_
from Gaudi.Configuration import *
from Configurables import StoreExplorerAlg
from TrackFitter.ConfiguredFitters import ConfiguredEventFitter
from Configurables import MeasurementProvider 
from Configurables import RefitParticleTracks
from Configurables import TrackInitFit, TrackMasterFitter, TrackStateInitTool,TrackNNGhostId
from Configurables import ToolSvc, TrackEventFitter


##### MDST CONFIGURATION
MyStream = "PID"
from PhysConf.MicroDST import uDstConf
uDstConf ('/Event/'+MyStream ) 


##### DAVINCI CONFIGURATION

from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1
DaVinci().DataType = "2012"
DaVinci().InputType = "MDST"
DaVinci().Simulation = False
DaVinci().SkipEvents = 0
DaVinci().UserAlgorithms = [
GaudiSequencer("RefitSeq")
] 


###### INPUTDATA



from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/PID.MDST/00020198/0000/00020198_00008986_1.pid.mdst'])




###### TRACKING IN DAVINCI

from Configurables import Tf__PatVeloFitLHCbIDs, Tf__PatVeloPhiHitManager, Tf__PatVeloRHitManager, Tf__DefaultVeloPhiHitManager, Tf__DefaultVeloRHitManager
from Configurables import OTRawBankDecoder, PatSeedFit, Tf__OTHitCreator

from STTools import STOfflineConf
STOfflineConf.DefaultConf().configureTools()



##### TRACKFITTING ENVIRONMENT ON MDST

ToolSvc().addTool(OTRawBankDecoder())
if "MDST" == DaVinci().InputType:
   ToolSvc().OTRawBankDecoder.RootInTES = '/Event/' + MyStream
ToolSvc().addTool(PatSeedFit())
if "MDST" == DaVinci().InputType:
   ToolSvc().PatSeedFit.RootInTES = '/Event/' + MyStream
ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
if "MDST" == DaVinci().InputType:
   ToolSvc().OTHitCreator.RootInTES = '/Event/' + MyStream


ToolSvc().addTool(Tf__DefaultVeloPhiHitManager("DefaultVeloPhiHitManager"))
if "MDST" == DaVinci().InputType:
   ToolSvc().DefaultVeloPhiHitManager.RootInTES = '/Event/' + MyStream

ToolSvc().addTool(Tf__DefaultVeloRHitManager("DefaultVeloRHitManager"))
if "MDST" == DaVinci().InputType:
   ToolSvc().DefaultVeloRHitManager.RootInTES = '/Event/' + MyStream



##### THE ACTUAL ALGORITHM TO DO THE WORK

from Configurables import TracksFromParticles

RefitParticleTracks().Inputs=['Phys/MuIDCalib_JpsiKFromBNoPIDNoMip/Particles']

### with state initialisation
RefitParticleTracks().addTool(TrackInitFit())
RefitParticleTracks().TrackInitFit.OutputLevel=0
RefitParticleTracks().TrackInitFit.addTool(TrackMasterFitter("Fit"))
RefitParticleTracks().TrackInitFit.Fit.OutputLevel=0
RefitParticleTracks().Fitter = "TrackInitFit"


### without state initialisation
#RefitParticleTracks().addTool(TrackMasterFitter())
#RefitParticleTracks().TrackMasterFitter.OutputLevel = 0
#RefitParticleTracks().Fitter = "TrackMasterFitter"


RefitParticleTracks().OutputLevel = 0 

### copy tracks to TES
TracksFromParticles().Inputs = ['/Event/PID/Phys/MuIDCalib_JpsiKFromBNoPIDNoMip/Particles']
TracksFromParticles().inputLocation = "Rec/Track/Zeug"
TracksFromParticles().OutputLevel = 0 

GaudiSequencer("RefitSeq").Members = [
RefitParticleTracks(),
TracksFromParticles() ]
if "MDST" == DaVinci().InputType:
   GaudiSequencer("RefitSeq").RootInTES = '/Event/' + MyStream


