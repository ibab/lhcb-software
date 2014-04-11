# Test to ensure a Reco14-Stripping20 mdst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST', KillDAQ = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco14-Stripping20.mdst',"InputCopyStream")

TestFileDB.test_file_db["R14S20-bhadron.mdst"].run()
