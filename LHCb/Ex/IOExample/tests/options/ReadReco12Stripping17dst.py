from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper('ROOT')
ioh.setupServices()
ioh.inputFiles(['PFN:Reco12-Stripping17.dst'])
