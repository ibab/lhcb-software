from Gaudi.Configuration import *
from Configurables import InputCopyStream
# Options to read POOL .dst file and make a copy
EventSelector().Input   = [
  "DATAFILE='PFN:rfio:/castor/cern.ch/user/c/cattanem/Brunel/v32r1/00001820_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]

InputCopyStream().Output = "DATAFILE='PFN:PoolDst.root' TYP='POOL_ROOTTREE' OPT='REC'"

# Option needed when reading a file containing HepMC 1.26 records (e.g. DC06)
ApplicationMgr().Dlls += [ "HepMCBack" ]
