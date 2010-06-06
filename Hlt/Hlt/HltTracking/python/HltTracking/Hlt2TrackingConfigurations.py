# The file in which we define all known tracking
# configurations for HLT2
#
__author__  =  "V. Gligorov vladimir.gligorov@cern.ch"
from Configurables import Hlt2Tracking
from HltTrackNames import Hlt2TracksPrefix, HltUnfittedTracksSuffix, Hlt2ForwardTracksName
from HltTrackNames import HltBiDirectionalKalmanFitSuffix,Hlt2DownstreamTracksName 
from HltTrackNames import Hlt2LongTracksName, HltUniDirectionalKalmanFitSuffix 
#
# Define all the instances of Hlt2Tracking
#
# The  vanilla  configuration, no track fitting or seeding
#
def Hlt2UnfittedForwardTracking() :
     return Hlt2Tracking("Hlt2UnfittedForwardTracking",                          EarlyDataTracking       = True,
                                                                                 Prefix                  = Hlt2TracksPrefix, 
                                                                                 FastFitType             = HltUnfittedTracksSuffix,
                                                                                 Hlt2Tracks              = Hlt2ForwardTracksName,
                                                                                 DoFastFit               = False,
                                                                                 DoSeeding               = False
                                    )
#
# With unidirectional track fitting, no seeding
#
def Hlt2UniKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2UniKalmanFittedForwardTracking",                EarlyDataTracking      = True,
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUniDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2ForwardTracksName,
                                                                                 DoFastFit              = True,
                                                                                 DoSeeding              = False
                                                                        )
#
# With bidirectional track fitting, no seeding
#
def Hlt2BiKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedForwardTracking",                 EarlyDataTracking      = True,
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2ForwardTracksName,
                                                                                 DoFastFit              = True,
                                                                                 DoSeeding              = False
                                                                        )
#
# With seeding, no track fitting
#
def Hlt2UnfittedLongTracking() :
        return Hlt2Tracking("Hlt2UnfittedLongTracking",                          EarlyDataTracking      = True, 
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUnfittedTracksSuffix,
                                                                                 Hlt2Tracks             = Hlt2LongTracksName,
                                                                                 DoFastFit              = False,
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = True
                                                                        )
#
# With seeding and track fitting
#
def Hlt2BiKalmanFittedLongTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedLongTracking",                    EarlyDataTracking      = True, 
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2LongTracksName,
                                                                                 DoFastFit              = True,
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = True
                                                                        )
#
# Now for the Downstream tracks, but no track fit. 
#
def Hlt2UnfittedDownstreamTracking() :
    return Hlt2Tracking("Hlt2UnfittedDownstreamTracking",                        EarlyDataTracking      = True, 
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUnfittedTracksSuffix,
                                                                                 Hlt2Tracks             = Hlt2DownstreamTracksName,
                                                                                 DoFastFit              = False,
                                                                                 DoSeeding              = True
                                                                        )
#
# Now the Downstream tracks with the bidirectional Kalman fit.
#
def Hlt2BiKalmanFittedDownstreamTracking() :
    return Hlt2Tracking("Hlt2BiKalmanFittedDownstreamTracking",                  EarlyDataTracking      = True, 
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2DownstreamTracksName,
                                                                                 DoFastFit              = True,
                                                                                 DoSeeding              = True
                                                                        )
#
# With track fitting and RICH PID for 3 hypotheses (kaon-proton-pion), but no seeding
#
def Hlt2BiKalmanFittedRichForProtonsForwardTracking() :
        return Hlt2Tracking("Hlt2BiKalmanFittedRichForProtonsForwardTracking",   EarlyDataTracking      = True, 
                                                                                 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2ForwardTracksName,
                                                                                 DoFastFit              = True,
                                                                                 DoSeeding              = False,
                                                                                 DoCloneKilling         = False,
                                                                                 MakeNeutrals           = False,
                                                                                 RichHypos              = ["pion","kaon","proton"]
                                                                        )
#
def setDataTypeForTracking(trackingInstance, dataType):

    trackingInstance.setProp("DataType", dataType)
