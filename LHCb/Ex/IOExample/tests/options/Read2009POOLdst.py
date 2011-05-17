# Test to read a POOL dst
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL-2009.dst']
        )
