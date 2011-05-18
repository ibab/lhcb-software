# Test to copy a POOL dst file to a ROOT dst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5 )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'ROOT',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/LHCb/Collision11/DIMUON.DST/00010348/0000/00010348_00000579_1.dimuon.dst?svcClass=lhcbdisk'],
        OutputFile        = 'PFN:ROOT-Reco09.dst'
        )
