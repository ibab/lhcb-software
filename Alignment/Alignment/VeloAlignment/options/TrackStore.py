from Gaudi.Configuration import *
from Configurables import (TrackStore)

#
# Job options defining track selection cuts
# (see TrackStore.cpp for meaning)
#

TrackStore("TrackStore").ACDC          = True
TrackStore("TrackStore").NonzerMin     = 4   # 4 for Closed Velo, 3 for Open
TrackStore("TrackStore").NonzerOverlap = 1
TrackStore("TrackStore").XOverlapCut   = 15. 
TrackStore("TrackStore").MissedHits    = True
TrackStore("TrackStore").TakeOverlap   = True
TrackStore("TrackStore").OutputLevel   = 3
