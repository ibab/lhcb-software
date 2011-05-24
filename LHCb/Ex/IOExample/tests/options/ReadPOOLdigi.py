# Test to read a POOL digi file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataContent = "DIGI", WithMC = True, LoadAll = True )

ioh = IOHelper('POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL.digi'])
