# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2010/BHADRON.DST/00008399/0000/00008399_00001052_1.bhadron.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:POOL-Reco08.dst'
        )
