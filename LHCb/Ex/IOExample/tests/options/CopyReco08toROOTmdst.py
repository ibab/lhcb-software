# Test to copy a POOL dst file to a ROOT mdst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:ROOT-Reco08.mdst',"InputCopyStream")

TestFileDB.test_file_db["Reco08-charm.mdst"].run()
