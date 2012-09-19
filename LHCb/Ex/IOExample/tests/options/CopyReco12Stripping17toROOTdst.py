# Test to ensure a Reco12-Stripping17 dst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/LHCb/Collision11/BHADRON.DST/00012545/0000/00012545_00000003_1.bhadron.dst?svcClass=lhcbdisk'])
ioh.outStream('PFN:Reco12-Stripping17.dst',"InputCopyStream")
