# Test to copy a POOL sim file to a ROOT sim file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper
from PRConfig import TestFileDB

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
#ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/g/gcorti/Gauss/2009/v37r0/30000000-100ev-20090407-MC09.sim?svcClass=default'])
ioh.outStream('PFN:ROOT.sim',"InputCopyStream")

TestFileDB.test_file_db["MC09-pool-sim"].run()
