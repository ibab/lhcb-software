# Test to read a POOL sim file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True, LoadAll = True )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL.sim'])
