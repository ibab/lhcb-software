##############################################################################
#
# Brunel options to run the default VELO monitoring
#
##############################################################################

from Configurables import GaudiSequencer

from Configurables import Velo__VeloClusterMonitor
from Configurables import Velo__VeloTrackMonitor

GaudiSequencer( 'MoniVELOSeq' ).Members +=\
                [   Velo__VeloClusterMonitor( 'VeloClusterMonitor' )
                  , Velo__VeloTrackMonitor  ( 'VeloTrackMonitor'   )
                  ]

Velo__VeloClusterMonitor( 'VeloClusterMonitor' ).PropertiesPrint = False
Velo__VeloTrackMonitor  ( 'VeloTrackMonitor'   ).PropertiesPrint = False

#TrackUser = Velo__VeloTrackMonitor( 'VeloTrackMonitor' )
#TrackUser.TrackLocation     = "Rec/Track/Best"
#TrackUser.TrackLocation     = "Rec/Track/Forward"
#TrackUser.OutputLevel       = 2
#TrackUser.HitMapHistos      = True
#TrackUser.ProfileResetEvent = 1000
#TrackUser.UnbiasedResiduals = True
#TrackUser.BiasedResiduals   = True
#TrackUser.ExtraPlots        = True
#TrackUser.EventClusterInfo  = True
#TrackUser.AlignMoniBasic    = True
#TrackUser.ACDCGeometry      = True

#To get unbiased residuals change the TrackLocation to "Rec/Track/Best"
# or "Rec/Track/Forward" 
#For biased/unbiased residuals vs momentum change TrackLocation to
# "Rec/Track/Forward"
##############################################################################
