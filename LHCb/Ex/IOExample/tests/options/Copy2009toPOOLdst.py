# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2009/DST/00006290/0000/00006290_00000001_1.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:POOL-2009.dst'
        )
