# Test to ensure a Reco14-Stripping20 dst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco14-Stripping20.dst',"InputCopyStream")

TestFileDB.test_file_db["S20_stripped_test"].run()
