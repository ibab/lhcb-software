# The file in which we define all known tracking
# configurations for HLT2
#
__author__  =  "V. Gligorov vladimir.gligorov@cern.ch"
from Hlt2Tracking  import Hlt2Tracking
#from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import HltDefaultFitSuffix
#from HltTrackNames import HltUniDirectionalKalmanFitSuffix
#
# Define all the instances of Hlt2Tracking
#
# With seeding and track fitting
# note : In 2011, this ran only the forward tracking, hence the name.
#        For 2012, it has been modified to do the seeding and clone killing,
#        but the name has been kept to simplify all the lines which depend on it.
#

#def Hlt2LongTracks():
#	return Hlt2Tracking("Hlt2LongTracks",
#			    FastFitType            = HltDefaultFitSuffix,
#			    Hlt2Tracks             = 'Long',
#			    DoSeeding              = True,
#			    DoCloneKilling         = True
#			    )
#
#def Hlt2BiKalmanFittedForwardTracking() :
# 	print "Please switch to Hlt2LongTracks"
#	print "Please switch to Hlt2LongTracks"
#	print "Please switch to Hlt2LongTracks"
#	print "Please switch to Hlt2LongTracks"
#	return Hlt2LongTracks()
#
def Hlt2BiKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2LongTracking",
			    FastFitType            = HltDefaultFitSuffix,
			    Hlt2Tracks             = 'Long',
			    DoSeeding              = True,
			    CreateBestTracks       = True # Set it to the same in both tracking configurations
			    )

##
# Now the Downstream tracks with Kalman fit.
#
def Hlt2BiKalmanFittedDownstreamTracking() :
	return Hlt2Tracking("Hlt2DownstreamTracking",
			    FastFitType            = HltDefaultFitSuffix,
			    Hlt2Tracks             = 'Downstream',
			    DoSeeding              = True,
			    CreateBestTracks       = True # Set it to the same in both tracking configurations
			    #TrackCuts              = {"Chi2Cut" : [0.,4.] }
			    )

def setDataTypeForTracking(trackingInstance, dataType):
	trackingInstance.setProp("DataType", dataType)
