from Configurables import HltGenConfig, ConfigTarFileAccessSvc

confsvc = ConfigTarFileAccessSvc( File = 'config.tar', Mode = 'ReadWrite' )

gen = HltGenConfig( ConfigTop = [ "StrippingGlobal" ]
                  , ConfigSvc = [ "ToolSvc" ]
                  , ConfigAccessSvc = confsvc.getName()
                  , HltType = 'Stripping24'
                  , MooreRelease = 'DAVINCI_v38r1p1'
                  , Label = '2015 Full restripping')

from Configurables import DaVinci
DaVinci().EventPreFilters = [ gen ]

DaVinci().EvtMax = 10000
