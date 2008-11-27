"""
 GAUDI data cards generated on 7/18/07 2:01 PM
 For Event Type = 10000000 / Data type = DST 1
     Configuration = DC06 - phys-v3-lumi2
     DST 1 datasets produced by Brunel - v30r17
     From FETC 1 datasets produced by DaVinci - v19r1
     From RDST 1 datasets produced by Brunel - v30r15
     Database version = v30r14
     Cards content = physical-physical
     
 Datasets replicated at CERN
 4 dataset(s) - NbEvents = 2626
 
"""

from Gaudi.Configuration import EventSelector

EventSelector().Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v3-lumi2/00001883/DST/0000/00001883_00002264_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v3-lumi2/00001883/DST/0000/00001883_00002382_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v3-lumi2/00001883/DST/0000/00001883_00002466_2.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v3-lumi2/00001883/DST/0000/00001883_00002627_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    ]
