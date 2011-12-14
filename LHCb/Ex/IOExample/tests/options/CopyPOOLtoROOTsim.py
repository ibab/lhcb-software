# Test to copy a POOL sim file to a ROOT sim file
from Configurables import IOTest
from GaudiConf.IOHelper import IOHelper

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True, LoadAll = True )

ioh = IOHelper('POOL','ROOT')
ioh.setupServices()
ioh.inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/g/gcorti/Gauss/2009/v37r0/30000000-100ev-20090407-MC09.sim?svcClass=default'])
ioh.outStream('PFN:ROOT.sim',"InputCopyStream")
