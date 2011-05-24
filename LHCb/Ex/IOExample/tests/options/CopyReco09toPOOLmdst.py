# Test to copy a POOL dst file to a POOL mdst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataContent = 'MDST' )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/LHCb/Collision11/LEPTONIC.MDST/00010195/0000/00010195_00000443_1.leptonic.mdst?svcClass=lhcbdisk'])
ioh.outStream('PFN:POOL-Reco09.mdst',"InputCopyStream")
