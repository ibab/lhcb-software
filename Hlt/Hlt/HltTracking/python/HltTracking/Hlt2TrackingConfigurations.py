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

def Hlt2Tracks():
	return Hlt2Tracking("Hlt2Tracks",        
			    FastFitType            = HltDefaultFitSuffix,
			    Hlt2Tracks             = 'Long',
			    DoSeeding              = True,
			    DoCloneKilling         = True
			    )

#Hlt2BiKalmanFittedForwardTracking = Hlt2Tracks
def Hlt2BiKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedForwardTracking",        
			    FastFitType            = HltDefaultFitSuffix,
			    Hlt2Tracks             = 'Long',
			    DoSeeding              = True,
			    DoCloneKilling         = True
			    )
##
# Now the Downstream tracks with Kalman fit.
#
def Hlt2BiKalmanFittedDownstreamTracking() :
    return Hlt2Tracking("Hlt2BiKalmanFittedDownstreamTracking", 
			FastFitType            = HltDefaultFitSuffix,
			Hlt2Tracks             = 'Downstream',
			DoSeeding              = True,
			DoCloneKilling         = False,
			#TrackCuts              = {"Chi2Cut" : [0.,4.] }    
			)
def setDataTypeForTracking(trackingInstance, dataType):

    trackingInstance.setProp("DataType", dataType)
