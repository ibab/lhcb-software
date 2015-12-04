##### Stream CONFIGURATION 
MyStream = "Leptonic"
is2015 = True
fitasin = "2015" # "2015","EM2015","Reco14"
DaVinci().InputType = "MDST"
inputs = ['Phys/B2XMuMu_Line/Particles']



##### DAVINCI CONFIGURATION

from Configurables import DaVinci, RefitParticleTracks
DaVinci().EvtMax = 600
DaVinci().PrintFreq = 200
if is2015:
   DaVinci().DataType = "2015"
else:
   DaVinci().DataType = "2012"
if DaVinci().InputType = "MDST":
   DaVinci().RootInTES = MyStream
DaVinci().Simulation = False
DaVinci().SkipEvents = 0

#### the actual algorithm
from DaVinciTrackRefitting import TrackRefitting
refitter = TrackRefitting.TrackRefitting("refittingalg",
                                          fitAsIn=fitasin,
                                          Stripping23=is2015,
					  RootInTES = MyStream, # Also needed on FullDST!
					  Inputs = inputs
                                          ).fitter()

DaVinci().UserAlgorithms += [ refitter ] 
# add your ntuple after the refitter


###### INPUTDATA


from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/DIMUON.DST/00048460/0000/00048460_00000415_1.dimuon.dst'
'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/LEPTONIC.MDST/00048279/0000/00048279_00001457_1.leptonic.mdst'          ## 'Phys/B2XMuMu_Line/Particles'
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision12/SEMILEPTONIC.DST/00041834/0000/00041834_00000332_1.semileptonic.dst'     ## 'Phys/PiforB2DMuNuX/Particles','Semileptonic/Phys/B2XuMuNuBs2KLine/Particles'
#'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision12/CHARM.MDST/00041834/0000/00041834_00000774_1.charm.mdst'                ## 'Phys/DstarPromptWithD02HHHHLine/Particles'
], clear=True)



