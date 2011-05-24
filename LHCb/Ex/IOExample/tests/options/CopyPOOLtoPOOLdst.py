# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataType = 'MC09' )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch//user/c/cattanem/Brunel/v32r1/00001820_00000001_5.dst?svcClass=default'])
ioh.outStream('PFN:POOL.dst',"InputCopyStream")
