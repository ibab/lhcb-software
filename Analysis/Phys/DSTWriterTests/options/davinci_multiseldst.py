from Gaudi.Configuration import *
from Configurables import DaVinci#, SelDSTWriter

from PhysSelPython.Wrappers import AutomaticData, SelectionSequence

from Configurables import TESCheck

pions = SelectionSequence('SeqPions',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightPions') )
muons = SelectionSequence('SeqMuons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightMuons') )

kaons =  SelectionSequence('SeqKaons',
                          TopSelection = AutomaticData(Location = 'Phys/StdTightKaons'))
jpsi =  SelectionSequence('SeqJpsi',
                          TopSelection = AutomaticData(Location = 'Phys/StdLooseJpsi2MuMu') )

from DSTWriters.__dev__.Configuration import SelDSTWriter

conf = SelDSTWriter("TestDST")
conf.OutputFileSuffix = "TestFSR"
conf.SelectionSequences = [pions, muons]
#conf.ExtraItems = ['/Event/DAQ/RawEvent#1']
selDSTSeq = conf.sequence()

#

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = 100
dv.UserAlgorithms = [selDSTSeq]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']

#from Configurables import CopyParticles
#CopyParticles('CopyParticles_SeqPions').OutputLevel = 1
from Configurables import InputCopyStream
InputCopyStream('SeqPions_OStream').OutputLevel=1
