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

#TrackUser = Velo__VeloTrackMonitor( 'VeloTrackMonitor' )
#TrackUser.TrackLocation     = "Rec/Track/Best"
#TrackUser.OutputLevel       = 2
#TrackUser.HitMapHistos      = True
#TrackUser.ProfileResetEvent = 1000
#TrackUser.UnbiasedResiduals = True
#TrackUser.BiasedResiduals   = True
#TrackUser.ExtraPlots        = True
#TrackUser.EventClusterInfo  = True
#TrackUser.ACDCGeometry      = True
#To get unbiased residuals only, change the TrackLocation to "Rec/Track/Best"

##############################################################################
