# Test to read a ROOT digi file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "DIGI", WithMC = True, LoadAll = True )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT.digi']
        )
