from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter
from StrippingConf.Configuration import StrippingConf
from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MultiSelectionSequence
from StrippingSelections.StreamDimuon import stream
streamLocations = [stream.name() + '/' + loc for loc in stream.outputLocations()]
print 'Will copy particles from'
selections = []
for loc in streamLocations :
    print loc
    sel = AutomaticData(Location = loc)
    selections.append(SelectionSequence('StripSeq'+ sel.name(), TopSelection = sel) )

multiSeq = MultiSelectionSequence('DiMuonSeq', Sequences = selections)



dstWriter = MicroDSTWriter("MyDSTWriter",
                           SelectionSequences = [multiSeq],
                           OutputPrefix = 'MicroDST',
                           OutputFileSuffix = 'TestDiMuon2',
                           CopyL0DUReport = False,
                           CopyHltDecReports = False,
                           CopyMCTruth = True,
                           CopyBTags = False
                           )

from Configurables import P2MCRelatorAlg, CopyParticle2MCRelations
P2MCRelatorAlg('P2MCRel_DiMuonSeq').OutputLevel = 1
CopyParticle2MCRelations('CopyP2MCRel_DiMuonSeq').OutputLevel = 1

dv = DaVinci()
dv.DataType = 'MC09'
dv.Simulation = True
dv.EvtMax = -1
dv.UserAlgorithms = [dstWriter.sequence()]
dv.Input =  [
    "   DATAFILE='TestDiMuon.Dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"
#"   DATAFILE='castor:/castor/cern.ch/user/j/jpalac/StripDST/2010/MC/DV_v25r5/TestDiMuon.Dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
