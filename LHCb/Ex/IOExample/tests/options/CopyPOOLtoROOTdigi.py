# Test to copy a POOL digi file to a ROOT digi file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = "DIGI", WithMC = True )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'ROOT',
        InputFiles        = ['PFN:castor:/castor/cern.ch/user/c/cattanem/Boole/v19r2/30000000-100ev-20090407-MC09.digi'],
        OutputFile        = 'PFN:ROOT.digi'
        )
