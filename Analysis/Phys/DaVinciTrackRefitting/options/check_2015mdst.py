from Configurables import DaVinci
##### Stream CONFIGURATION 
MyStream = "Leptonic"
is2015 = True
fitasin = "2015" # "2015","EM2015","Reco14"
DaVinci().InputType = "MDST"
#inputs = ['Phys/B2XMuMu_Line/Particles']
#inputs = ['Phys/DstarPromptWithD02HHHHLine/Particles']
#inputs = ['Phys/PiforB2DMuNuX/Particles',
#inputs = ['Semileptonic/Phys/B2XuMuNuBs2KLine/Particles']
#inputs = ['Phys/BetaSJpsi2MuMuLine/Particles']
inputs = ['Phys/B2XMuMu_Line/Particles']
#DaVinci().DDDBtag   = "dddb-20120831"
#DaVinci().CondDBtag = "cond-20120831"

DaVinci().DDDBtag    = "dddb-20150724" 
DaVinci().CondDBtag  = "cond-20150828" 




##### DAVINCI CONFIGURATION

from Configurables import DaVinci, RefitParticleTracks
DaVinci().EvtMax = 10000#-1
DaVinci().PrintFreq = 1
if is2015:
   DaVinci().DataType = "2015"
else:
   DaVinci().DataType = "2012"
if DaVinci().InputType == "MDST":
   DaVinci().RootInTES = MyStream
DaVinci().Simulation = False
DaVinci().SkipEvents = 0

#### the actual algorithm
from DaVinciTrackRefitting import TrackRefitting
from Configurables import GaudiSequencer
refitter = TrackRefitting.TrackRefitting("refittingalg",
                                          fitAsIn=fitasin,
                                          Stripping23=is2015,
                                          RootInTES = MyStream, # Also needed on FullDST!
                                          Inputs = inputs,
                                          ValidationMode = True,
                                          UpdateProbability = True,
                                          DoProbability = True,
                                          Manipulator = "Run2GhostId"
                                          ).fitter()
DaVinci().UserAlgorithms += [refitter]


###### INPUTDATA

from Gaudi.Configuration import appendPostConfigAction
from Configurables import Tf__OTHitCreator,ToolSvc,OTRawBankDecoder
#def fix():
#  ToolSvc().addTool(OTRawBankDecoder())
#  ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
#  ToolSvc().OTRawBankDecoder.RootInTES = MyStream
#  ToolSvc().OTHitCreator.RootInTES = MyStream
#appendPostConfigAction(fix)

  ##          from Configurables import OTRawBankDecoder, PatSeedFit, Tf__OTHitCreator, FastVeloHitManager, Tf__DefaultVeloPhiHitManager, Tf__DefaultVeloRHitManager
  ##          ToolSvc().addTool(PatSeedFit())
  ##          ToolSvc().addTool(Tf__OTHitCreator("OTHitCreator") )
  ##          ToolSvc().addTool(FastVeloHitManager("FastVeloHitManager"))
  ##          ToolSvc().addTool(Tf__DefaultVeloPhiHitManager("DefaultVeloPhiHitManager"))
  ##          ToolSvc().addTool(Tf__DefaultVeloRHitManager("DefaultVeloRHitManager"))
  ##          ToolSvc().OTRawBankDecoder.RootInTES = rit
  ##          ToolSvc().PatSeedFit.RootInTES = rit
  ##          ToolSvc().OTHitCreator.RootInTES = rit
  ##          ToolSvc().FastVeloHitManager.RootInTES = rit
  ##          ToolSvc().DefaultVeloPhiHitManager.RootInTES = rit
  ##          ToolSvc().DefaultVeloRHitManager.RootInTES = rit
from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/DIMUON.DST/00048460/0000/00048460_00000415_1.dimuon.dst'                 ## 'Dimuon/Phys/BetaSJpsi2MuMuLine/Particles'
'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/LEPTONIC.MDST/00048279/0000/00048279_00001457_1.leptonic.mdst'          ## 'Phys/B2XMuMu_Line/Particles'
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision12/SEMILEPTONIC.DST/00041834/0000/00041834_00000332_1.semileptonic.dst'     ## 'Phys/PiforB2DMuNuX/Particles','Semileptonic/Phys/B2XuMuNuBs2KLine/Particles'
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision12/CHARM.MDST/00041834/0000/00041834_00000774_1.charm.mdst'                ## 'Phys/DstarPromptWithD02HHHHLine/Particles'
# BenderTools.GetDBtags     INFO           DDDB : dddb-20120831
# BenderTools.GetDBtags     INFO       LHCBCOND : cond-20120831

], clear=True)



