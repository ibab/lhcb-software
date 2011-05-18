# Test to copy a POOL dst file to a POOL mdst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = 'MDST' )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/data/2010/CHARM.MDST/00008397/0000/00008397_00000939_1.charm.mdst?svcClass=lhcbdisk' ],
        OutputFile        = 'PFN:POOL-Reco08.mdst'
        )
