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


##### Stream CONFIGURATION 
MyStream = "Dimuon"
MDST = False
readingStripping23 = True
fitAsInEM2015 = False
fitAsIn2015 = True
fitAsInReco14 = False


##### DAVINCI CONFIGURATION

from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1
DaVinci().DataType = "2012"
if MDST:
   DaVinci().InputType = "MDST"
   DaVinci().RootInTES = MyStream
DaVinci().Simulation = False
DaVinci().SkipEvents = 0
DaVinci().UserAlgorithms = [
GaudiSequencer("RefitSeq")
] 


###### INPUTDATA


from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/validation/Collision15/DIMUON.DST/00047284/0000/00047284_00000180_1.dimuon.dst'
], clear=True)




###### TRACKING IN DAVINCI

from Configurables import Tf__PatVeloFitLHCbIDs, Tf__PatVeloPhiHitManager, Tf__PatVeloRHitManager, Tf__DefaultVeloPhiHitManager, Tf__DefaultVeloRHitManager
from Configurables import OTRawBankDecoder, PatSeedFit, Tf__OTHitCreator

from STTools import STOfflineConf
STOfflineConf.DefaultConf().configureTools()



##### TRACKFITTING ENVIRONMENT ON MDST

ToolSvc().addTool(OTRawBankDecoder())
ToolSvc().addTool(PatSeedFit())
ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
ToolSvc().addTool(Tf__DefaultVeloPhiHitManager("DefaultVeloPhiHitManager"))
ToolSvc().addTool(Tf__DefaultVeloRHitManager("DefaultVeloRHitManager"))
if "MDST" == DaVinci().InputType or readingStripping23:
   ToolSvc().OTRawBankDecoder.RootInTES = '/Event/' + MyStream
   ToolSvc().PatSeedFit.RootInTES = '/Event/' + MyStream
   ToolSvc().OTHitCreator.RootInTES = '/Event/' + MyStream
   ToolSvc().DefaultVeloPhiHitManager.RootInTES = '/Event/' + MyStream
   ToolSvc().DefaultVeloRHitManager.RootInTES = '/Event/' + MyStream



##### THE ACTUAL ALGORITHM TO DO THE WORK

from Configurables import TracksFromParticles

RefitParticleTracks().Inputs=['Phys/StdLooseKsLL/Particles']


from Configurables import TrackInitFit, TrackMasterFitter
from TrackFitter.ConfiguredFitters import ConfiguredMasterFitter
from Configurables import TrackBestTrackCreator, TrackMasterFitter, TrackStateInitTool, FastVeloFitLHCbIDs

def giveitafit(thething):
   thething.addTool(TrackInitFit,"TrackInitFit")
   thething.TrackInitFit.addTool( TrackMasterFitter, name = "Fit" )
   thething.TrackInitFit.addTool( TrackStateInitTool, name = "Init")
   thething.TrackInitFit.Init.UseFastMomentumEstimate = True

   if fitAsInEM2015:
      ConfiguredMasterFitter( getattr(thething.TrackInitFit, "Fit"), SimplifiedGeometry = True, LiteClusters = True, MSRossiAndGreisen = False )
   if fitAsIn2015:
      ConfiguredMasterFitter( getattr(thething.TrackInitFit, "Fit"), SimplifiedGeometry = True, LiteClusters = True, MSRossiAndGreisen = True )
   if fitAsInReco14:
      ConfiguredMasterFitter( getattr(thething.TrackInitFit, "Fit"), SimplifiedGeometry = False, LiteClusters = False, MSRossiAndGreisen = False )
   thething.TrackInitFit.Init.VeloFitterName = "FastVeloFitLHCbIDs"
   thething.TrackInitFit.Init.addTool(FastVeloFitLHCbIDs, name = "FastVeloFitLHCbIDs")
   if DaVinci().InputType == "MDST" or readingStripping23:
      thething.TrackInitFit.RootInTES = MyStream
      thething.TrackInitFit.Fit.RootInTES = MyStream
      thething.TrackInitFit.Init.RootInTES = MyStream
      thething.TrackInitFit.Init.FastVeloFitLHCbIDs.RootInTES = MyStream


giveitafit(RefitParticleTracks())


### copy tracks to TES
TracksFromParticles().Inputs = ['/Event/PID/Phys/MuIDCalib_JpsiKFromBNoPIDNoMip/Particles']
TracksFromParticles().inputLocation = "Rec/Track/Zeug"
TracksFromParticles().OutputLevel = 0 

GaudiSequencer("RefitSeq").Members = [
RefitParticleTracks(),
TracksFromParticles() ]


