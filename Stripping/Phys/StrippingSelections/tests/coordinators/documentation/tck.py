from Configurables import HltGenConfig, ConfigTarFileAccessSvc

confsvc = ConfigTarFileAccessSvc( File = 'config.tar', Mode = 'ReadWrite' )

gen = HltGenConfig( ConfigTop = [ "StrippingGlobal" ]
                  , ConfigSvc = [ "ToolSvc" ]
                  , ConfigAccessSvc = confsvc.getName()
                  , HltType = 'Stripping21r1p1'
                  , MooreRelease = 'DAVINCI_v39r1'
                  , Label = '2011 Incremental stripping')

from Configurables import DaVinci
DaVinci().EventPreFilters = [ gen ]

DaVinci().EvtMax = 10000
