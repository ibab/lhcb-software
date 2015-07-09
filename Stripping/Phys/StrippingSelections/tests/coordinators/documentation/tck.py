from Configurables import HltGenConfig, ConfigTarFileAccessSvc

confsvc = ConfigTarFileAccessSvc( File = 'config.tar', Mode = 'ReadWrite' )

gen = HltGenConfig( ConfigTop = [ "StrippingGlobal" ]
                  , ConfigSvc = [ "ToolSvc" ]
                  , ConfigAccessSvc = confsvc.getName()
                  , HltType = 'Stripping20r1p1'
                  , MooreRelease = 'DAVINCI_v32r2p4'
                  , Label = '2011, 1st incremental restripping')

from Configurables import DaVinci
DaVinci().EventPreFilters = [ gen ]

DaVinci().EvtMax = 10000
