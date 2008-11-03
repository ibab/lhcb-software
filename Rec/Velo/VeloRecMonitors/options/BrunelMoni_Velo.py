##############################################################################
#
# Brunel options to run the default VELO monitoring
#
##############################################################################

from Configurables import ProcessPhase, GaudiSequencer

from Configurables import Velo__VeloClusterMonitor
from Configurables import Velo__VeloTrackMonitor

GaudiSequencer( 'MoniVELOSeq' ).Members +=\
                [   Velo__VeloClusterMonitor( 'VeloClusterMonitor' )
                  , Velo__VeloTrackMonitor  ( 'VeloTrackMonitor'   )
                  ]

##############################################################################
