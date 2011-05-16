# Test to copy a POOL sim file to a POOL sim file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/g/gcorti/Gauss/2009/v37r0/30000000-100ev-20090407-MC09.sim?svcClass=default'],
        OutputFile        = 'PFN:POOL.sim'
        )
