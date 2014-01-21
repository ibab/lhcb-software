# Test to copy a POOL dst file to a ROOT mdst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/data/2010/CHARM.MDST/00008397/0000/00008397_00000939_1.charm.mdst'])
ioh.outStream('PFN:ROOT-Reco08.mdst',"InputCopyStream")
