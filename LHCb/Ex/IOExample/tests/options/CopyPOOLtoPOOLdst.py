# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch//user/c/cattanem/Brunel/v32r1/00001820_00000001_5.dst?svcClass=default'],
        OutputFile        = 'PFN:POOL.dst'
        )
