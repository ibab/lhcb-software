##--------------------------------------------------------------
from Gaudi.Configuration import *
DataOnDemandSvc().Nodes += [ "DATA='/Event/MC' TYPE='DataObject'" ]
DataOnDemandSvc().AlgMap["MC/Particles"] = "UnpackMCParticle"
DataOnDemandSvc().AlgMap["MC/Vertices"] = "UnpackMCVertex"

