# Test to read a POOL dst
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataType = 'MC09' )

ioh = IOHelper('POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL.dst'])
