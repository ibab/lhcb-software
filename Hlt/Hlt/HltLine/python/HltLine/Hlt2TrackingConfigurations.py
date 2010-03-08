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
        #__used_configurables__ = []
        __slots__ = []
#
class Hlt2BiKalmanFittedRICHForwardTracking(Hlt2Tracking) :
	#__used_configurables__ = []
	__slots__ = []
#
class Hlt2UnfittedDownstreamTracking(Hlt2Tracking) :
	#__used_configurables__ = []
	__slots__ = []
#
class Hlt2BiKalmanFittedDownstreamTracking(Hlt2Tracking) :
	#__used_configurables__ = []
	__slots__ = []
#
# The "vanilla" configuration, no track fitting, no RICH
# PID, no seeding, no CALO PID for charged hadrons 
#
def setHlt2UnfittedForwardTracking(prefix,dataType,Hlt2UnfittedForwardTracking) :
        
	Hlt2UnfittedForwardTracking.setProp("DataType",		dataType		)
        Hlt2UnfittedForwardTracking.setProp("Prefix",		prefix			)
        Hlt2UnfittedForwardTracking.setProp("FastFitType",	HltUnfittedTracksSuffix )
        Hlt2UnfittedForwardTracking.setProp("Hlt2Tracks",	Hlt2ForwardTracksName	)
        Hlt2UnfittedForwardTracking.setProp("DoFastFit",	False			)
        Hlt2UnfittedForwardTracking.setProp("UseRICH",		False			)
        Hlt2UnfittedForwardTracking.setProp("UseCALO",		False			)
        Hlt2UnfittedForwardTracking.setProp("DoSeeding",	False			)
        Hlt2UnfittedForwardTracking.setProp("DoCloneKilling",	False			)
#
# With track fitting and RICH PID, but no seeding
# and no CALO PID for charged hadrons
#
def setHlt2BiKalmanFittedRICHForwardTracking(prefix,dataType,Hlt2BiKalmanFittedRICHForwardTracking) :

	Hlt2BiKalmanFittedRICHForwardTracking.setProp("Prefix",		prefix				)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("FastFitType",	HltBiDirectionalKalmanFitSuffix	)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("Hlt2Tracks",	Hlt2ForwardTracksName		)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("DoFastFit",	True				)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("UseRICH",	True				)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("UseCALO",	False				)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("DataType",	dataType			)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("DoSeeding",	False				)
	Hlt2BiKalmanFittedRICHForwardTracking.setProp("DoCloneKilling",	False				)
#
# Now for the Downstream tracks, but no track fit. RICH and CALO PID
# information off, makes downstream tracks ONLY
#
def setHlt2UnfittedDownstreamTracking(prefix,dataType,Hlt2UnfittedDownstreamTracking) :

	Hlt2UnfittedDownstreamTracking.setProp("Prefix",	prefix			)
	Hlt2UnfittedDownstreamTracking.setProp("FastFitType",	HltUnfittedTracksSuffix	)
	Hlt2UnfittedDownstreamTracking.setProp("Hlt2Tracks",	Hlt2DownstreamTracksName)
	Hlt2UnfittedDownstreamTracking.setProp("DoFastFit",	False			)
	Hlt2UnfittedDownstreamTracking.setProp("UseRICH",	False			)
	Hlt2UnfittedDownstreamTracking.setProp("UseCALO",	False			)
	Hlt2UnfittedDownstreamTracking.setProp("DataType",	dataType		)
	Hlt2UnfittedDownstreamTracking.setProp("DoSeeding",	True			)
	Hlt2UnfittedDownstreamTracking.setProp("DoCloneKilling",False			)
#
# Now the Downstream tracks with the bidirectional Kalman fit.
# RICH and CALO PID information off, makes downstream tracks ONLY
#
def setHlt2BiKalmanFittedDownstreamTracking(prefix,dataType,Hlt2BiKalmanFittedDownstreamTracking) :

	Hlt2BiKalmanFittedDownstreamTracking.setProp("Prefix",		prefix				)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("FastFitType",	HltBiDirectionalKalmanFitSuffix	)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("Hlt2Tracks",	Hlt2DownstreamTracksName	)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("DoFastFit",	True				)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("UseRICH",		False				)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("UseCALO",		False				)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("DataType",	dataType			)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("DoSeeding",	True				)
        Hlt2BiKalmanFittedDownstreamTracking.setProp("DoCloneKilling",	False				)
