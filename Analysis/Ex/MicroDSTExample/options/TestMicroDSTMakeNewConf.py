from Gaudi.Configuration import *
from Configurables import DaVinci, PhysMicroDST

importOptions("$STDOPTS/PreloadUnits.opts")
importOptions( "$CCBARROOT/options/DoDC06SelBs2Jpsi2MuMuPhi2KK_lifetime_unbiased.opts")
MySelection = GaudiSequencer("SeqDC06selBs2JpsiPhi")

conf = PhysMicroDST()
conf.OutputPrefix = "MicroDST"
conf.MicroDSTFile = "MyTestMDST_MC_newConf.dst"
conf.MicroDSTSelectionAlg = "DC06selBs2JpsiPhi_unbiased"
conf.CopyL0DUReport = False
conf.CopyHltDecReports = False
conf.CopyMCTruth = True
# to use the PV re-fitting, need to re-fit PVs, relate them, and pass the relations table location to the MicroDST maker. Could we put this On-Demand?
from Configurables import PVReFitterAlg, PVRelatorAlg
mainLocation = "Phys/"+"DC06selBs2JpsiPhi_unbiased"
PVReFitter = PVReFitterAlg("PVReFitterAlg")
PVReFitter.ParticleInputLocation = mainLocation+"/Particles"
PVReFitter.VertexOutputLocation = mainLocation+"/RefittedVertices"
p2ReFitPVRelationsLoc = mainLocation+"/Particle2ReFittedVertexRelations"
PVReFitter.P2VRelationsOutputLocation = p2ReFitPVRelationsLoc
PVReFitter.OutputLevel=4
PVRelator = PVRelatorAlg()
PVRelator.OutputLevel=4
PVRelator.P2PVRelationsInputLocation = p2ReFitPVRelationsLoc
p2pvSortedRelationsLoc = mainLocation + "/P2ReFitPVSortedRelations"
PVRelator.P2PVRelationsOutputLocation = p2pvSortedRelationsLoc
MySelection.Members += [PVReFitter, PVRelator]
conf.CopyRelatedPVs = True
conf.P2PVRelationsSuffix = "P2ReFitPVSortedRelations"
#
conf.CopyBTags = True
dv = DaVinci()
dv.EvtMax = 500
#dv.HistogramFile = "MyTestMDST_MC_newConf.dst"
dv.UserAlgorithms = [MySelection]
dv.Input =  [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000007_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000009_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000011_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v4-lumi2/00002146/DST/0000/00002146_00000013_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
