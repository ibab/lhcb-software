# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, WithMC = True, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/MC/MC10/ALLSTREAMS.DST/00008898/0000/00008898_00000002_1.allstreams.dst'])
ioh.outStream('PFN:ROOT-MC10.dst',"InputCopyStream")
