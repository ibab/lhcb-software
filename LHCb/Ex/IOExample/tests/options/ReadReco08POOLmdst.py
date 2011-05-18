# Test to read a POOL mdst
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, LoadAll = True, DataContent = 'MDST' )

IOConf( InputPersistency  = 'POOL',
        InputFiles        = ['PFN:POOL-Reco08.mdst']
        )
