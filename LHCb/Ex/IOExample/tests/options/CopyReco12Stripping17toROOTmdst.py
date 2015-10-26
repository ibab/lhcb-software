# Test to ensure a Reco12-Stripping17 mdst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST', KillDAQ = True )

ioh = IOHelper()
ioh.setupServices()
ioh.outStream('PFN:Reco12-Stripping17.mdst',"InputCopyStream")
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/swtest/lhcb/LHCb/Collision11/CHARM.MDST/00012547/0000/00012547_00000013_1.charm.mdst'])

#TestFileDB.test_file_db["R12S17-charm.mdst"].run()
