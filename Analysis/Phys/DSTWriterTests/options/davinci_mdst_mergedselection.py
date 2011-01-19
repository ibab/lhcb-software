from Gaudi.Configuration import *
from Configurables import DaVinci #, MicroDSTWriter
from DSTWriters.__dev__.Configuration import MicroDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MergedSelection, MultiSelectionSequence
from Configurables import TESCheck

pions = AutomaticData(Location = 'Phys/StdTightPions')
muons = AutomaticData(Location = 'Phys/StdTightMuons')
kaons = AutomaticData(Location = 'Phys/StdTightKaons')
jpsis = AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu')

allSelection = MergedSelection('MergeAll', RequiredSelections = [pions, kaons, muons, jpsis] )

pionSeq = SelectionSequence('SeqPions', TopSelection = pions )
muonSeq = SelectionSequence('SeqMuons', TopSelection = muons )

kaonSeq =  SelectionSequence('SeqKaons', TopSelection = kaons)
jpsiSeq =  SelectionSequence('SeqJpsi', TopSelection = jpsis )

mergeSeq = SelectionSequence('SeqMerge', TopSelection = allSelection )

#mergeSeq.algorithm().OutputLevel=1

mergeAlg = getConfigurable("MergeAll")
mergeAlg.OutputLevel=1
mergeAlg.ForceP2PVBuild = True

multiSeq = MultiSelectionSequence('CocktailB', Sequences = [kaonSeq, pionSeq, muonSeq, jpsiSeq, mergeSeq])

conf = MicroDSTWriter("MicroDST0", 
                      OutputFileSuffix = "Test4",
                      SelectionSequences = [multiSeq])

#selDST0Seq = conf.sequence()

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.UserAlgorithms = [conf.sequence()]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
