from Gaudi.Configuration import *
from Configurables import DaVinci, MicroDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MergedSelection, MultiSelectionSequence
from Configurables import TESCheck

pions = AutomaticData(Location = 'Phys/StdTightPions')
muons = AutomaticData(Location = 'Phys/StdTightMuons')
kaons = AutomaticData(Location = 'Phys/StdTightKaons')
jpsis = AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu')

allSelection = MergedSelection('MergeAll', RequiredSelections = [pions, kaons, muons, jpsis] )

pionSeq = SelectionSequence('SeqPions', pions )
muonSeq = SelectionSequence('SeqMuons', muons )

kaonSeq =  SelectionSequence('SeqKaons', kaons)
jpsiSeq =  SelectionSequence('SeqJpsi', jpsis )

mergeSeq = SelectionSequence('SeqMerge', TopSelection = allSelection )

multiSeq = MultiSelectionSequence('CocktailB', Sequences = [kaonSeq, pionSeq, muonSeq, jpsiSeq, mergeSeq])

conf = MicroDSTWriter("MicroDST0", 
                      OutputFileSuffix = "Test",
                      SelectionSequences = [multiSeq],
                      CopyFlavourTags = False)

selDST0Seq = conf.sequence()

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.UserAlgorithms = [selDST0Seq]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
