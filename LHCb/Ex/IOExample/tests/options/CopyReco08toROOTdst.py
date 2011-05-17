# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'ROOT',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2010/BHADRON.DST/00008399/0000/00008399_00001052_1.bhadron.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:ROOT-Reco08.dst'
        )
