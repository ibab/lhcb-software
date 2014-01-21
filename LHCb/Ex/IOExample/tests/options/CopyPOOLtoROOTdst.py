# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataType = 'MC09' )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:ROOT.dst',"InputCopyStream")

TestFileDB.test_file_db["MC09-pool-dst"].run()
