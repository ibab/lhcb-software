from Gaudi.Configuration import *
from Configurables import InputCopyStream
# Options to read POOL .dst file and make a copy
EventSelector().Input   = [
  "DATAFILE='PFN:rfio:/castor/cern.ch/user/c/cattanem/Brunel/v34r5/00001820_00000001.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]

InputCopyStream().Output = "DATAFILE='PFN:PoolDst.root' TYP='POOL_ROOTTREE' OPT='REC'"
