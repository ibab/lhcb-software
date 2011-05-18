# Test to copy a POOL dst file to a POOL dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/LHCb/Collision11/DIMUON.DST/00010348/0000/00010348_00000579_1.dimuon.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:POOL-Reco09.dst'
        )
