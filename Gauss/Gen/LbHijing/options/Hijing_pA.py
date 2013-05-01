from Gaudi.Configuration import *
from Configurables import Generation, MinimumBias, HijingProduction
from Configurables import ToolSvc, EvtGenDecay

Generation().addTool( MinimumBias )
Generation().MinimumBias.addTool( HijingProduction )
# Configuration for pA collisions
Generation().MinimumBias.HijingProduction.Commands += [
  "hijinginit frame CMS",
  "hijinginit proj P",
  "hijinginit targ A",
  "hijinginit beam1",
  "hijinginit iap 1", 
  "hijinginit izp 1",
  "hijinginit iat 208",
  "hijinginit izt 82"
]
