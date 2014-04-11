# Test to ensure a Reco12-Stripping17 dst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco12-Stripping17.dst',"InputCopyStream")

TestFileDB.test_file_db["R12S17-bhadron.dst"].run()
