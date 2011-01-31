## 
##  Example on how to run only the generator phase but saving the
##  output in MCParticle format, the output will have .xgen extension
##

from Gauss.Configuration import *

Gauss().Phases = ['Generator', 'GenToMCTree']

