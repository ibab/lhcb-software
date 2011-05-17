# Test to read a POOL dst
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True, DataType = 'MC09' )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL.dst']
        )
