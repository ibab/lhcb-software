# Test to copy a POOL dst file to a POOL mdst file
from Configurables import IOTest, IOConf

IOTest( EvtMax = 5, DataContent = 'MDST' )

IOConf( InputPersistency  = 'POOL',
        OutputPersistency = 'POOL',
        InputFiles        = ['PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid//lhcb/LHCb/Collision11/LEPTONIC.MDST/00010195/0000/00010195_00000443_1.leptonic.mdst?svcClass=lhcbdisk' ],
        OutputFile        = 'PFN:POOL-Reco09.mdst'
        )
