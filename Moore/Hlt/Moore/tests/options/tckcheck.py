TCK1=0x00ed0046
TCK2=0x00ee0046
#TCK1=0x00990046
TCK3=0x00ef0046
configdest="../../../../InstallArea/$CMTCONFIG"

from TCKUtils.utils import *
from Configurables import ConfigTarFileAccessSvc as ctas
ctas(File=os.sep.join([configdest,"config.tar"]))
print "============= diffs zero->1 event ==============="
diff(TCK1,TCK2)
print "============= diffs 1->100 events ==============="
diff(TCK2,TCK3)

