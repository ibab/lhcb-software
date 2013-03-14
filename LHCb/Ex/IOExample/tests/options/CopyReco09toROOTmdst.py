# Test to copy a POOL dst file to a ROOT mdst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

ioh = IOHelper('POOL','ROOT')
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision11/LEPTONIC.MDST/00010195/0000/00010195_00000443_1.leptonic.mdst'])
ioh.outStream('PFN:ROOT-Reco09.mdst',"InputCopyStream")
