from Gaudi.Configuration import *

from Configurables import HltGenConfig, ConfigTarFileAccessSvc

confsvc = ConfigTarFileAccessSvc( File = 'config.tar', Mode = 'ReadWrite' )

gen = HltGenConfig( ConfigTop = [ "StrippingGlobal" ]
                  , ConfigSvc = [ "ToolSvc" ]
                  , ConfigAccessSvc = confsvc.getName()
                  , HltType = 'Stripping'
                  , MooreRelease = 'v28r5'
                  , Label = 'Stripping16')


from Configurables import DaVinci 

DaVinci().EventPreFilters = [ gen ]

EventSelector().Input=[
  "DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2010/CHARM.DST/00008157/0000/00008157_00001000_1.charm.dst' TYP='POOL_ROOTTREE' OPT='READ'",
]
