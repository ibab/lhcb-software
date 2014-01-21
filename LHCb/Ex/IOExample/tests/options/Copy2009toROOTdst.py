# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:ROOT-2009.dst',"InputCopyStream")

TestFileDB.test_file_db["2009_dst_read_test"].run()
