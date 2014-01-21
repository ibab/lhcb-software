# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataType = 'MC09' )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch//user/c/cattanem/Brunel/v34r5/00001820_00000001.dst?svcClass=default'])
ioh.outStream('PFN:ROOT.dst',"InputCopyStream")
