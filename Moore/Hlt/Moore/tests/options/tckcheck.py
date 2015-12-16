import os
from TCKUtils.utils import *

TCK1=0x00ed0046
TCK2=0x00ee0046
TCK3=0x00ef0046
configdest="../../../../InstallArea/$CMTCONFIG"

ConfigAccessSvc().File = os.path.join(configdest, "config.cdb")
print "============= diffs zero->1 event ==============="
diff(TCK1,TCK2)
print "============= diffs 1->100 events ==============="
diff(TCK2,TCK3)
