from Configurables import Moore, HltConf

HltConf().ThresholdSettings="Hlt2_TrackingOnly"
Moore().UseTCK = False

# HLT1 might have been run with a TCK so lets look into the local one
from Configurables import ConfigCDBAccessSvc
ConfigCDBAccessSvc().File='config.cdb'
Moore().TCKData = '$MOOREROOT/tests/options/TCKData/'
Moore().InitialTCK = '0x50000001'
Moore().CheckOdin=False
