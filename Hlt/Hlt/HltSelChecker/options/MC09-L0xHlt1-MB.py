#-- GAUDI jobOptions generated on Mon Nov  9 10:57:12 2009
#-- Contains event types :
#--   L0xHlt1 stripped MC09 MB

from Gaudi.Configuration import *

EventSelector().Input   = [
    "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/L0HLT1.DST/00005589/0000/0005589_00000502_1.l0hlt1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/L0HLT1.DST/00005589/0000/0005589_00000503_1.l0hlt1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]
