# Test to copy a POOL dst file to a POOL mdst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataContent = 'MDST' )

ioh = IOHelper('POOL','POOL')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2010/CHARM.MDST/00008397/0000/00008397_00000939_1.charm.mdst?svcClass=lhcbdisk'])
ioh.outStream('PFN:POOL-Reco08.mdst',"InputCopyStream")
