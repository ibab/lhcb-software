# Test to read a POOL dst
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL-Reco09.dst'])
