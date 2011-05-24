# Test to read a POOL mdst
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

ioh = IOHelper('POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:POOL-Reco09.mdst'])
