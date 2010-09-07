from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MultiSelectionSequence

pions = SelectionSequence('SeqPions',
                          TopSelection = AutomaticData('Bob',Location = 'Phys/StdTightPions') )
muons = SelectionSequence('SeqMuons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightMuons') )
kaons =  SelectionSequence('SeqKaons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightKaons') )

jpsi =  SelectionSequence('SeqJpsi',
                          TopSelection = AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu'))

#multiSeq = MultiSelectionSequence('Cocktail', Sequences = [pions, muons, kaons, jpsi])
multiSeqA = MultiSelectionSequence('CocktailA', Sequences = [kaons, pions])
multiSeqB = MultiSelectionSequence('CocktailB', Sequences = [muons, pions])
multiSeqC = MultiSelectionSequence('CocktailB', Sequences = [jpsi, pions])

conf = MicroDSTWriter("MicroDST0")
conf.OutputFileSuffix = "TestCrossover"
conf.CopyProtoParticles = False
#conf.SelectionSequences = [multiSeqA, multiSeqB]
conf.SelectionSequences = [pions, pions.clone('SeqPions2'), pions.clone('SeqPions3')]
#conf.SelectionSequences = [pions, muons, kaons, jpsi]
conf.CopyL0DUReport = False
conf.CopyHltDecReports = False
conf.CopyMCTruth = False
conf.CopyBTags = False
microDST0Seq = conf.sequence()

#

dv = DaVinci()
dv.DataType = 'MC09'
dv.Simulation = True
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
