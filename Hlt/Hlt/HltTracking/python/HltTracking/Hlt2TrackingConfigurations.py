# The file in which we define all known tracking
# configurations for HLT2
#
__author__  =  "V. Gligorov vladimir.gligorov@cern.ch"
from Hlt2Tracking  import Hlt2Tracking
from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import HltUniDirectionalKalmanFitSuffix 
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
			    FastFitType            = HltBiDirectionalKalmanFitSuffix,
			    Hlt2Tracks             = 'Long',
			    DoSeeding              = True,
			    DoCloneKilling         = True
			    )

#Hlt2BiKalmanFittedForwardTracking = Hlt2Tracks
def Hlt2BiKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedForwardTracking",        
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = 'Long',
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = True
                                                                        )
#
# With seeding and track fitting
#
def Hlt2BiKalmanFittedLongTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedLongTracking",  
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = 'Long',
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = True
                                                                        )
#
# Now the Downstream tracks with the bidirectional Kalman fit.
#
def Hlt2BiKalmanFittedDownstreamTracking() :
    return Hlt2Tracking("Hlt2BiKalmanFittedDownstreamTracking", 
			FastFitType            = HltBiDirectionalKalmanFitSuffix,
			Hlt2Tracks             = 'Downstream',
			DoSeeding              = True,
			DoCloneKilling         = False,
			#TrackCuts              = {"Chi2Cut" : [0.,4.] }    
			)
#
# With track fitting and RICH PID for 3 hypotheses (kaon-proton-pion), but no seeding
#
def Hlt2BiKalmanFittedRichForProtonsForwardTracking() :
     return Hlt2Tracking("Hlt2BiKalmanFittedRichForProtonsForwardTracking", 
                         FastFitType            = HltBiDirectionalKalmanFitSuffix,
                         Hlt2Tracks             = 'Long',
                         DoSeeding              = True,
                         DoCloneKilling         = True,
                         RichHypos              = ["pion","proton","kaon"],
                         ExtraRichName          = "highPT",
                         RichTrackCuts          = {"Forward":{ "Chi2Cut" : [0,3], "PCut" : [2,100], "PtCut" : [1.9,100], "Likelihood" : [-999999,999999] } , "Match":{ "Chi2Cut" : [0,3], "PCut" : [2,100], "PtCut" : [1.9,100], "Likelihood" : [-999999,999999] } }
                         )

#
# With track fitting and RICH PID for 2 hypotheses (proton-pion), but no seeding
#
def Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking() :
     return Hlt2Tracking("Hlt2BiKalmanFittedRichForLowPTParticlesForwardTracking", 
                         FastFitType            = HltBiDirectionalKalmanFitSuffix,
                         Hlt2Tracks             = 'Long',
                         DoSeeding              = True,
                         DoCloneKilling         = True,
                         RichHypos              = ["pion","proton","kaon"],
                         ExtraRichName          = "lowPT",
                         RichTrackCuts          = {"Forward":{ "Chi2Cut" : [0,3], "PCut" : [2,100], "PtCut" : [0.3,100], "Likelihood" : [-999999,999999] }, "Match" : { "Chi2Cut" : [0,3], "PCut" : [2,100], "PtCut" : [0.3,100], "Likelihood" : [-999999,999999] }  }
                         )

#
def setDataTypeForTracking(trackingInstance, dataType):

    trackingInstance.setProp("DataType", dataType)
