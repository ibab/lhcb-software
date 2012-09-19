# Test to ensure a Reco14-Stripping20 mdst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST', KillDAQ = True )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/LHCb/Collision12/BHADRON.MDST/00020198/0000/00020198_00000758_1.bhadron.mdst?svcClass=lhcbdisk'])
ioh.outStream('PFN:Reco14-Stripping20.mdst',"InputCopyStream")
