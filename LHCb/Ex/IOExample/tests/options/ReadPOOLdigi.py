# Test to read a POOL digi file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "DIGI", WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL.digi']
        )
