# Test to read a ROOT mdst
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

IOConf( InputPersistency  = 'ROOT',
        InputFiles        = ['PFN:ROOT-Reco08.mdst']
        )
