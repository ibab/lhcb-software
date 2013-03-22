# Test to ensure a Reco14-Stripping20 dst can be copied
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/DIMUON.DST/00020198/0000/00020198_00000763_1.dimuon.dst'])
ioh.outStream('PFN:Reco14-Stripping20.dst',"InputCopyStream")
