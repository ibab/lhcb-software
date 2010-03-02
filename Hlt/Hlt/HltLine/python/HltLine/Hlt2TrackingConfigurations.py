# The file in which we define all known tracking
# configurations for HLT2
#
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
from Hlt2Tracking import Hlt2Tracking
from HltTrackNames import Hlt2TracksPrefix, HltUnfittedTracksSuffix, Hlt2ForwardTracksName
from HltTrackNames import HltBiDirectionalKalmanFitSuffix,Hlt2DownstreamTracksName 
#
# First we have to declare all the classes which will inherit from
# Hlt2Tracking
#
class Hlt2UnfittedForwardTracking(Hlt2Tracking) :
        __used_configurables__ = []
        __slots__ = []
#
class Hlt2BiKalmanFittedRICHForwardTracking(Hlt2Tracking) :
	__used_configurables__ = []
	__slots__ = []
#
class Hlt2UnfittedDownstreamTracking(Hlt2Tracking) :
	__used_configurables__ = []
	__slots__ = []
#
class Hlt2BiKalmanFittedDownstreamTracking(Hlt2Tracking) :
    __used_configurables__ = []
    __slots__ = []
#
# The "vanilla" configuration, no track fitting, no RICH
# PID, no seeding, no CALO PID for charged hadrons 
#
def setHlt2UnfittedForwardTracking(prefix,dataType,Hlt2UnfittedForwardTracking) :
        
	Hlt2UnfittedForwardTracking.DataType = dataType
        Hlt2UnfittedForwardTracking.Prefix = prefix
        Hlt2UnfittedForwardTracking.FastFitType = HltUnfittedTracksSuffix #track fit is off by default
        Hlt2UnfittedForwardTracking.Hlt2Tracks = Hlt2ForwardTracksName
        Hlt2UnfittedForwardTracking.DoFastFit = False
        Hlt2UnfittedForwardTracking.UseRICH = False
        Hlt2UnfittedForwardTracking.UseCALO = False
        Hlt2UnfittedForwardTracking.DoSeeding = False
        Hlt2UnfittedForwardTracking.DoCloneKilling = False
#
# With track fitting and RICH PID, but no seeding
# and no CALO PID for charged hadrons
#
def setHlt2BiKalmanFittedRICHForwardTracking(prefix,dataType,Hlt2BiKalmanFittedRICHForwardTracking) :

	Hlt2BiKalmanFittedRICHForwardTracking.Prefix = prefix
	Hlt2BiKalmanFittedRICHForwardTracking.FastFitType = HltBiDirectionalKalmanFitSuffix
	Hlt2BiKalmanFittedRICHForwardTracking.Hlt2Tracks = Hlt2ForwardTracksName
	Hlt2BiKalmanFittedRICHForwardTracking.DoFastFit = True
	Hlt2BiKalmanFittedRICHForwardTracking.UseRICH = True
	Hlt2BiKalmanFittedRICHForwardTracking.UseCALO = False
	Hlt2BiKalmanFittedRICHForwardTracking.DataType = dataType
	Hlt2BiKalmanFittedRICHForwardTracking.DoSeeding = False
	Hlt2BiKalmanFittedRICHForwardTracking.DoCloneKilling = False
#
# Now for the Downstream tracks, but no track fit. RICH and CALO PID
# information off, makes downstream tracks ONLY
#
def setHlt2UnfittedDownstreamTracking(prefix,dataType,Hlt2UnfittedDownstreamTracking) :

	Hlt2UnfittedDownstreamTracking.Prefix = prefix
	Hlt2UnfittedDownstreamTracking.FastFitType = HltUnfittedTracksSuffix
	Hlt2UnfittedDownstreamTracking.Hlt2Tracks = Hlt2DownstreamTracksName
	Hlt2UnfittedDownstreamTracking.DoFastFit = False
	Hlt2UnfittedDownstreamTracking.UseRICH = False
	Hlt2UnfittedDownstreamTracking.UseCALO = False
	Hlt2UnfittedDownstreamTracking.DataType = dataType
	Hlt2UnfittedDownstreamTracking.DoSeeding = True
	Hlt2UnfittedDownstreamTracking.DoCloneKilling = False
#
# Now the Downstream tracks with the bidirectional Kalman fit.
# RICH and CALO PID information off, makes downstream tracks ONLY
#
def setHlt2BiKalmanFittedDownstreamTracking(prefix,dataType,Hlt2BiKalmanFittedDownstreamTracking) :

	Hlt2BiKalmanFittedDownstreamTracking.Prefix = prefix
        Hlt2BiKalmanFittedDownstreamTracking.FastFitType = HltBiDirectionalKalmanFitSuffix
        Hlt2BiKalmanFittedDownstreamTracking.Hlt2Tracks = Hlt2DownstreamTracksName
        Hlt2BiKalmanFittedDownstreamTracking.DoFastFit = True
        Hlt2BiKalmanFittedDownstreamTracking.UseRICH = False
        Hlt2BiKalmanFittedDownstreamTracking.UseCALO = False
        Hlt2BiKalmanFittedDownstreamTracking.DataType = dataType
        Hlt2BiKalmanFittedDownstreamTracking.DoSeeding = True
        Hlt2BiKalmanFittedDownstreamTracking.DoCloneKilling = False
