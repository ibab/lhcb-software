# Test to read a POOL MC10 dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL-MC10.dst']
        )
