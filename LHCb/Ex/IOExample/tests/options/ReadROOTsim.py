# Test to read a ROOT sim file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "SIM", WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT.sim']
        )
