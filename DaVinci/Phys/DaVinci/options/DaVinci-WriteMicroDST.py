from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter

importOptions("$STDOPTS/PreloadUnits.opts")

from MicroDSTExample.Selections import SeqBs2Jpsi2MuMuPhi2KK
selSequence = SeqBs2Jpsi2MuMuPhi2KK.SeqBs2Jpsi2MuMuPhi2KK

conf = MicroDSTWriter("MicroDST0")
conf.OutputFileSuffix = "Test"
conf.CopyProtoParticles = False
conf.SelectionSequences = [selSequence]
conf.CopyL0DUReport = False
conf.CopyHltDecReports = False
conf.CopyMCTruth = True
conf.CopyBTags = True
microDST0Seq = conf.sequence()

#

dv = DaVinci()
dv.DataType = 'MC09'
dv.EvtMax = 100
dv.UserAlgorithms = [microDST0Seq]
dv.Input =  [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000002_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000003_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000004_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000005_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000006_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000007_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000008_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000009_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000010_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000011_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000012_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000013_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004879/0000/00004879_00000014_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
