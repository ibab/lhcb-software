from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, MultiSelectionSequence
from Configurables import TESCheck

pions = SelectionSequence('SeqPions',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightPions') )
muons = SelectionSequence('SeqMuons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightMuons') )

kaons =  SelectionSequence('SeqKaons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightKaons'))
jpsi =  SelectionSequence('SeqJpsi',
                          TopSelection = AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu') )



multiSeqA = MultiSelectionSequence('CocktailA', Sequences = [kaons, pions, muons])
multiSeqB = MultiSelectionSequence('CocktailB', Sequences = [pions, muons, kaons])
multiSeqC = MultiSelectionSequence('CocktailC', Sequences = [muons, pions, kaons])

conf = SelDSTWriter("MicroDST0")
conf.OutputFileSuffix = "TestAll"
conf.SelectionSequences = [multiSeqA, multiSeqB, multiSeqC]
#conf.SelectionSequences = [pions]
#conf.SelectionSequences = [kaons, pions, muons]
#conf.SelectionSequences = [multiSeqA]
conf.ExtraItems = ['/Event/DAQ/RawEvent#1']
selDST0Seq = conf.sequence()

#

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
#dv.UserAlgorithms = [multiSeqA.sequence(), multiSeqB.sequence(), selDST0Seq]
dv.UserAlgorithms = [conf.sequence()]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
