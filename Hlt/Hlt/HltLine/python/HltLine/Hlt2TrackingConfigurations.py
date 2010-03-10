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
# The  vanilla  configuration, no track fitting, no RICH
# PID, no seeding, no CALO PID for charged hadrons 
#
def Hlt2UnfittedForwardTracking() :
	return Hlt2Tracking("Hlt2UnfittedForwardTracking", 			Prefix 			= Hlt2TracksPrefix, 
										FastFitType 		= HltUnfittedTracksSuffix,
										Hlt2Tracks		= Hlt2ForwardTracksName,
										DoFastFit		= False,
										UseRICHForHadrons	= False,
										UseCALOForHadrons	= False,
										DoSeeding		= False,
										DoCloneKilling		= False
									)
#
# With unidirectional track fitting, no RICH PID, no seeding,
# no CALO PID for charged hadrons 
#
def Hlt2UniKalmanFittedForwardTracking() :
        return Hlt2Tracking("Hlt2UniKalmanFittedForwardTracking",	 	 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUniDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2ForwardTracksName,
                                                                                 DoFastFit              = True,
                                                                                 UseRICHForHadrons      = False,
                                                                                 UseCALOForHadrons      = False,
                                                                                 DoSeeding              = False,
                                                                                 DoCloneKilling         = False
                                                                        )
#
# With seeding, no track fitting, no RICH
# PID, no CALO PID for charged hadrons 
#
def Hlt2UnfittedLongTracking() :
        return Hlt2Tracking("Hlt2UnfittedLongTracking",              	       	 Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUnfittedTracksSuffix,
                                                                                 Hlt2Tracks             = Hlt2LongTracksName,
                                                                                 DoFastFit              = False,
                                                                                 UseRICHForHadrons      = False,
                                                                                 UseCALOForHadrons      = False,
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = False
                                                                        )
#
# With track fitting and RICH PID, but no seeding
# and no CALO PID for charged hadrons
#
def Hlt2BiKalmanFittedRICHForwardTracking() :
	return Hlt2Tracking("Hlt2BiKalmanFittedRICHForwardTracking",    	 Prefix                 = Hlt2TracksPrefix,
                                                                        	 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                        	 Hlt2Tracks             = Hlt2ForwardTracksName,
                                                                        	 DoFastFit              = True,
                                                                        	 UseRICHForHadrons      = True,
                                                                        	 UseCALOForHadrons      = False,
                                                                        	 DoSeeding              = False,
                                                                        	 DoCloneKilling         = False
                                                                	)
#
# Now for the Downstream tracks, but no track fit. RICH and CALO PID
# information off, makes downstream tracks ONLY
#
def Hlt2UnfittedDownstreamTracking() :
	return Hlt2Tracking("Hlt2UnfittedDownstreamTracking",		         Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltUnfittedTracksSuffix,
                                                                                 Hlt2Tracks             = Hlt2DownstreamTracksName,
                                                                                 DoFastFit              = False,
                                                                                 UseRICHForHadrons      = False,
                                                                                 UseCALOForHadrons      = False,
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = False
                                                                        )
#
# Now the Downstream tracks with the bidirectional Kalman fit.
# RICH and CALO PID information off, makes downstream tracks ONLY
#
def Hlt2BiKalmanFittedDownstreamTracking() :
	return Hlt2Tracking("Hlt2BiKalmanFittedDownstreamTracking",	         Prefix                 = Hlt2TracksPrefix,
                                                                                 FastFitType            = HltBiDirectionalKalmanFitSuffix,
                                                                                 Hlt2Tracks             = Hlt2DownstreamTracksName,
                                                                                 DoFastFit              = True,
                                                                                 UseRICHForHadrons      = False,
                                                                                 UseCALOForHadrons      = False,
                                                                                 DoSeeding              = True,
                                                                                 DoCloneKilling         = False
                                                                        )
#
def setDataTypeForTracking(trackingInstance, dataType):

	trackingInstance.setProp("DataType", dataType)
