from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter
from StrippingConf.Configuration import StrippingConf

from StrippingSelections.Streams import StreamDimuon, StreamCalibration
sc = StrippingConf( Streams = [StreamDimuon.stream] )

dstWriter = SelDSTWriter("MyDSTWriter",
                         SelectionSequences = sc.activeStreams(),
                         OutputFileSuffix = 'TestDiMuon',
                         CopyPVRelations = {"Particle2VertexRelations":True}
                         )
dstWriter.ExtraItems += ['/Event/Strip/Phys/DecReports#1']
dv = DaVinci()
dv.DataType = 'MC09'
dv.Simulation = True
dv.EvtMax = 500
dv.appendToMainSequence( [ sc.sequence() ] )
dv.appendToMainSequence( [ dstWriter.sequence() ] )
#dv.UserAlgorithms = [dstWriter.sequence()]
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
