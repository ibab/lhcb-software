# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5 )

ioh = IOHelper('POOL','ROOT')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2009/DST/00006290/0000/00006290_00000001_1.dst?svcClass=lhcbdisk'])
ioh.outStream('PFN:ROOT-2009.dst',"InputCopyStream")
