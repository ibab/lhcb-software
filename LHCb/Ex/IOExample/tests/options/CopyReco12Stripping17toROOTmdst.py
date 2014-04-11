# Test to ensure a Reco12-Stripping17 mdst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST', KillDAQ = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco12-Stripping17.mdst',"InputCopyStream")

TestFileDB.test_file_db["R12S17-charm.mdst"].run()
