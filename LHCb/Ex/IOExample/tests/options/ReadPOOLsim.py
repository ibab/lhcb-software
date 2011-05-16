# Test to read a POOL sim file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL.sim']
        )
