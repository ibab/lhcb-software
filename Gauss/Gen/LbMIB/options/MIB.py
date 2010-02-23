##
## Steer Gauss not to use standard physics generation algorihtm but the
## one for MIB: 'MIBackground'
##
from Gauss.Configuration import *
Gauss().Production = 'MIB'
