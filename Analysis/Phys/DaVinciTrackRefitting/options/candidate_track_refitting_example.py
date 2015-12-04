from Gaudi.Configuration import *

MessageSvc().Format = "% F%90W%S%7W%R%T %0W%M"
##### Stream CONFIGURATION 
MyStream = "Dimuon"
MDST = False
readingStripping23 = True
fitAsInEM2015 = False
fitAsIn2015 = True
fitAsInReco14 = False

##### DAVINCI CONFIGURATION

from Configurables import DaVinci, RefitParticleTracks
DaVinci().EvtMax = 400
DaVinci().PrintFreq = 400
DaVinci().DataType = "2015"
if MDST:
   DaVinci().InputType = "MDST"
   DaVinci().RootInTES = MyStream
else:
   DaVinci().InputType = "DST"
DaVinci().Simulation = False
DaVinci().SkipEvents = 0

##### FIT CONFIGURATION

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


###### TRACKING IN DAVINCI

from STTools import STOfflineConf
STOfflineConf.DefaultConf().configureTools()
from Configurables import OTRawBankDecoder, PatSeedFit, Tf__OTHitCreator, FastVeloHitManager, Tf__DefaultVeloPhiHitManager, Tf__DefaultVeloRHitManager
ToolSvc().addTool(OTRawBankDecoder())
ToolSvc().addTool(PatSeedFit())
ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
ToolSvc().addTool(FastVeloHitManager("FastVeloHitManager"))
ToolSvc().addTool(Tf__DefaultVeloPhiHitManager("DefaultVeloPhiHitManager"))
ToolSvc().addTool(Tf__DefaultVeloRHitManager("DefaultVeloRHitManager"))
if "MDST" == DaVinci().InputType or readingStripping23:
   ToolSvc().OTRawBankDecoder.RootInTES = '/Event/' + MyStream
   ToolSvc().PatSeedFit.RootInTES = '/Event/' + MyStream
   ToolSvc().OTHitCreator.RootInTES = '/Event/' + MyStream
   ToolSvc().FastVeloHitManager.RootInTES = '/Event/' + MyStream
   ToolSvc().DefaultVeloPhiHitManager.RootInTES = '/Event/' + MyStream
   ToolSvc().DefaultVeloRHitManager.RootInTES = '/Event/' + MyStream


#### the actual algorithm
from Configurables import RefitParticleTracks
refitter = RefitParticleTracks()
refitter.Inputs = ['Phys/StdLooseKsLL/Particles','Phys/StdLooseKsDD/Particles']
giveitafit(refitter)
DaVinci().UserAlgorithms = [
RefitParticleTracks()
] 



###### INPUTDATA


from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/DIMUON.DST/00048460/0000/00048460_00000415_1.dimuon.dst'
], clear=True)



