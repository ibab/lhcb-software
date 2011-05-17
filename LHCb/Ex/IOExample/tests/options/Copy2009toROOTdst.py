# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'ROOT',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2009/DST/00006290/0000/00006290_00000001_1.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:ROOT-2009.dst'
        )
