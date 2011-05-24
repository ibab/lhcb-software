# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataType = 'MC09' )

ioh = IOHelper('POOL','ROOT')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch//user/c/cattanem/Brunel/v32r1/00001820_00000001_5.dst?svcClass=default'])
ioh.outStream('PFN:ROOT.dst',"InputCopyStream")
