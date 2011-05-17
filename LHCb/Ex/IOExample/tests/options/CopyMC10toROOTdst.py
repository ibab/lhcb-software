# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, WithMC = True )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'ROOT',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008898/0000/00008898_00000002_1.allstreams.dst'],
        OutputFile        = 'PFN:ROOT-MC10.dst'
        )
