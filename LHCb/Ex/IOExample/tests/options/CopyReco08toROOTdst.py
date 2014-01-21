# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, LoadAll = True )

ioh = IOHelper()
ioh.setupServices()
ioh.inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/data/2010/BHADRON.DST/00008399/0000/00008399_00001052_1.bhadron.dst'])
ioh.outStream('PFN:ROOT-Reco08.dst',"InputCopyStream")
