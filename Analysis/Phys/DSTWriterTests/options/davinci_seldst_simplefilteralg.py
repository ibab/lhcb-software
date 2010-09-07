class EventSelection(object) :
    def __init__(self, algorithm) :
        self._alg = algorithm
        self.requiredSelections = list()
    def name(self) :
        return algorithm.name()
    def algorithm(self) :
        return self._alg
    def outputLocation(self) :
        return ''

from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter



from PhysSelPython.Wrappers import SelectionSequence
from Configurables import CheckPV
checkPV = EventSelection(CheckPV('TwoPV', MinPVs=1))
seqCheckPV =  SelectionSequence('SeqPV', checkPV)

conf = SelDSTWriter("SelDST")
conf.OutputFileSuffix = "TestSimpleAlg"
conf.SelectionSequences = [seqCheckPV]
conf.ExtraItems = ['/Event/DAQ/RawEvent#1']
selDST0Seq = conf.sequence()

#

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.UserAlgorithms = [selDST0Seq]
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
