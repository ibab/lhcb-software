# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:ROOT-Reco08.dst',"InputCopyStream")

TestFileDB.test_file_db["Reco08-bhadron.dst"].run()
