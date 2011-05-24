# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5 )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2009/DST/00006290/0000/00006290_00000001_1.dst?svcClass=lhcbdisk'])
ioh.outStream('PFN:POOL-2009.dst',"InputCopyStream")
