from Gaudi.Configuration import *

MessageSvc().Format = "% F%90W%S%7W%R%T %0W%M"
##### Stream CONFIGURATION 
MyStream = "Dimuon"

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

#### the actual algorithm
from DaVinciTrackRefitting import TrackRefitting
refitter = TrackRefitting.TrackRefitting("refittingalg",
                                          fitAsIn="2015",
                                            # possible: "2015","EM2015","Reco14"
                                          Stripping23=True,
					  RootInTES = "Dimuon", # Also needed on FullDST!
					  Inputs = ['Phys/StdLooseKsLL/Particles','Phys/StdLooseKsDD/Particles'] 
                                          ).fitter()

DaVinci().UserAlgorithms += [ refitter ] 
# add your ntuple after the refitter


###### INPUTDATA


from Gaudi.Configuration import *
from GaudiConf import IOHelper
IOHelper('ROOT').inputFiles([
'root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/LHCb/Collision15/DIMUON.DST/00048460/0000/00048460_00000415_1.dimuon.dst'
], clear=True)



