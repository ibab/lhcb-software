# A module to hold the hardcoded names of tracks/protoparticles in the Hlt
# and rules for deriving containers from them  

########################################################################
# Globals
########################################################################
# These "globals" define that Tracks go into .../Track/... and
# protoparticles into .../ProtoP/...
HltGlobalTrackLocation 			= "Track"
HltGlobalProtoPLocation			= "ProtoP"

########################################################################
# Tracks
########################################################################
# prefixes where to put the tracks (these go into the prefix field of 
# _trackLocation and _protosLocation)
#
HltSharedTracksPrefix 			= "Hlt"
Hlt1TracksPrefix 			= "Hlt1"
Hlt2TracksPrefix 			= "Hlt2"
#
# names of track types (these go into the tracks field of _trackLocation 
# and _protosLocation)
#
HltSharedRZVeloTracksName               = "RZVelo"
Hlt2VeloTracksName 			= "Velo"
Hlt2ForwardTracksName 			= "Forward"
Hlt2MatchTracksName 			= "Match"
Hlt2LongTracksName 			= "Long"
Hlt2SeedingTracksName 			= "Seeding"
Hlt2DownstreamTracksName 		= "SeedTT"
HltMuonTracksName			= "Muon"
HltAllMuonTracksName			= "AllMuon"
#
# types of track fit (including no fit!) (these go into the fastFitType 
# field of _trackLocation and _protosLocation)
#
HltUnfittedTracksSuffix			= "Unfitted"
HltBiDirectionalKalmanFitSuffix 	= "BiKalmanFitted"
HltUniDirectionalKalmanFitSuffix 	= "UniKalmanFitted"  
#
########################################################################
# ProtoParticles
########################################################################
# The suffix (this goes into the type field of _protosLocation)
#
Hlt2ChargedProtoParticleSuffix 		= "Charged"
Hlt2NeutralProtoParticleSuffix 		= "Neutral" 
#
########################################################################
# PID
########################################################################
# We want to generate PID containers for different track types the
# same way in which we generate tracks, meaning using the same rules,
# because we need different PID for different tracks; they should not 
# overwrite each other!
# 
# First for the Muon PID
# the prefix goes into the "type" field of _trackLocation
# the suffix goes into the "tracks" field
#
HltSharedMuonIDPrefix 			= "Muon"
HltSharedMuonIDSuffix			= "MuonPID"
#  
########################################################################
# The rules for generating track and proto particle containers
########################################################################
# For tracks, the format is e.g. Hlt2/Track/Unfitted/Forward 
#
def _trackLocation(prefix,type,fastFitType,tracks):
    if (fastFitType == "") : return prefix + "/" + type + "/" + tracks
    else : return prefix + "/" + type + "/" + fastFitType + "/" + tracks
#
# For protos, the format is e.g. Hlt2/ProtoP/Unfitted/Charged/Forward 
#   
def _protosLocation(prefix,type,fastFitType,tracks):
    if (fastFitType == "") : return prefix +"/" + HltGlobalProtoPLocation + "/" + type + "/" + tracks
    else :              return prefix +"/" + HltGlobalProtoPLocation + "/" + fastFitType + "/" + type + "/" + tracks

__all__ = (	
		#
		# The strings
		#
		HltGlobalTrackLocation,
		HltGlobalProtoPLocation,
		# 
		HltSharedTracksPrefix, 
           	Hlt1TracksPrefix, 
           	Hlt2TracksPrefix, 
		#
		HltSharedRZVeloTracksName,
		Hlt2ForwardTracksName, 
		Hlt2MatchTracksName, 
		Hlt2LongTracksName, 
		Hlt2SeedingTracksName, 
		Hlt2DownstreamTracksName,
		HltMuonTracksName,
		HltAllMuonTracksName,
		#
		HltUnfittedTracksSuffix, 
		HltBiDirectionalKalmanFitSuffix, 
		HltUniDirectionalKalmanFitSuffix, 
		#
		Hlt2ChargedProtoParticleSuffix, 
		Hlt2NeutralProtoParticleSuffix,
		#	
		HltSharedMuonIDPrefix,
		HltSharedMuonIDSuffix,
		#
		# The functions
		#
		_trackLocation,
		_protosLocation
	  ) 
