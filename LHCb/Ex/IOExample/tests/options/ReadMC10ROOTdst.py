# Test to read a ROOT MC10 dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT-MC10.dst']
        )
