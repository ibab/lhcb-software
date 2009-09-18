## Switch on geometry upstream of Velo
from Gaudi.Configuration import *
from Configurables import GiGaInputStream

geo = GiGaInputStream('Geo')
geo.StreamItems += [ "/dd/Structure/LHCb/UpstreamRegion/BlockWallUpstr" ]
geo.StreamItems += [ "/dd/Structure/LHCb/BeforeMagnetRegion/PipeJunctionBeforeVelo" ]
geo.StreamItems += [ "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo" ]


