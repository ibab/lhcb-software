from Configurables import Moore, HltConf
## Configure the tarfile access
from Configurables import ConfigCDBAccessSvc
ConfigCDBAccessSvc().File='config.cdb'

Moore().TCKData = '$MOOREROOT/tests/options/TCKData/'
Moore().InitialTCK = '0x50000001'
Moore().UseTCK = True
Moore().CheckOdin=False
