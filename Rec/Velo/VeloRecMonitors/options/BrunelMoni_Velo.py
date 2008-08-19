##############################################################################
#
# Brunel options to run the default VELO monitoring
#
##############################################################################

from Configurables import ProcessPhase, GaudiSequencer

from Configurables import Velo__VeloClusterMonitor

ProcessPhase( 'Moni' ).DetectorList += [ 'VELO' ]

GaudiSequencer( 'MoniVELOSeq' ).Members +=\
                [ Velo__VeloClusterMonitor( 'VeloClusterMonitor' ) ]

##############################################################################
