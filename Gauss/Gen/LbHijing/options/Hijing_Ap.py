from Gaudi.Configuration import *
from Configurables import Generation, MinimumBias, HijingProduction
from Configurables import ToolSvc, EvtGenDecay

Generation().addTool( MinimumBias )
Generation().MinimumBias.addTool( HijingProduction )
# Configuration for pA collisions
Generation().MinimumBias.HijingProduction.Commands += [
  "hijinginit frame CMS",
  "hijinginit proj A",
  "hijinginit targ P",
  "hijinginit beam1",
  "hijinginit iap 208", 
  "hijinginit izp 82",
  "hijinginit iat 1",
  "hijinginit izt 1"
]
