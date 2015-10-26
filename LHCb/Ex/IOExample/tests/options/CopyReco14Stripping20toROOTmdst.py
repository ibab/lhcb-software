# Test to ensure a Reco14-Stripping20 mdst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST', KillDAQ = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco14-Stripping20.mdst',"InputCopyStream")
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/LHCb/Collision12/BHADRON.MDST/00020198/0000/00020198_00000758_1.bhadron.mdst'])

#TestFileDB.test_file_db["R14S20-bhadron.mdst"].run()
