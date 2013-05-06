####################
from Gaudi.Configuration import *
from Configurables import EventIndexer

ei = EventIndexer()
ei.Stripping = "Stripping20"  # This is the name of the stripping to put in the index
ei.OutputFile = "event_index.root"  # Output file name for the indexer data.

ApplicationMgr().TopAlg = [ei]

# set up the input data
from GaudiConf.IOHelper import IOHelper
ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0000/00020198_00000763_1.dimuon.dst'])
####################


