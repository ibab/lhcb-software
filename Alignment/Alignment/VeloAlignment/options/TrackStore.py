from Gaudi.Configuration import *
from Configurables import (TrackStore)

#
# Job options defining track selection cuts
# (see TrackStore.cpp for meaning)
#

TrackStore("TrackStoreTool").ACDC          = True
TrackStore("TrackStoreTool").NonzerMin     = 4   # 4 for Closed Velo, 3 for Open
TrackStore("TrackStoreTool").NonzerOverlap = 1
TrackStore("TrackStoreTool").XOverlapCut	 = 15. 
TrackStore("TrackStoreTool").MissedHits    = True
TrackStore("TrackStoreTool").TakeOverlap   = True
TrackStore("TrackStoreTool").OutputLevel   = 3
