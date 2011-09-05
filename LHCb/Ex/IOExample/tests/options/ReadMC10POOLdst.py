# Test to read a POOL MC10 dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, WithMC = True, LoadAll = True )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL-MC10.dst'])
