# A module to hold the hardcoded names of tracks/protoparticles in the Hlt
# and rules for deriving containers from them  
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
########################################################################
# Globals
########################################################################
# These "globals" define that Tracks go into .../Track/... and
# protoparticles into .../ProtoP/...
HltGlobalTrackLocation 			= "Track"
HltGlobalProtoPLocation			= "ProtoP"
#
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
# The recognised track types for the Hlt2 Tracking
Hlt2TrackingRecognizedTrackTypes = [Hlt2ForwardTracksName, Hlt2LongTracksName, Hlt2DownstreamTracksName] 
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
# These rules apply to HLT2 TODO: add rule for HLT1 usable from Hlt1Units
########################################################################
# For tracks, the format is e.g. Hlt2/Track/Unfitted/Forward 
#
# First of all we have the "base" track location for the VeloRZ,
# Velo, Forward, Seeding, SeedTT, and Match tracks. These track
# locations are never directly an input to a protoparticle
# or particle maker and by definition have no "fast-fit" run
#
def _baseTrackLocation(prefix,tracks) :
    return prefix + "/" + HltGlobalTrackLocation + "/" + tracks 
#
# Next the "short" track location for the derived tracks
# This function checks that the track asked for by the Hlt2Tracking instance
# is in the recognised track types, and returns "Unknown" or the correct
# suffix based on the configuration of the Hlt2Tracking. 
#
def _shortTrackLocation(trackingType) :
    if (trackingType.getProp("Hlt2Tracks") not in Hlt2TrackingRecognizedTrackTypes) :
        return "Unknown"
    elif (trackingType.getProp("Hlt2Tracks") == Hlt2LongTracksName) :
        if trackingType.getProp("DoSeeding") : 
            return Hlt2LongTracksName
        else : 
            return Hlt2ForwardTracksName
    elif (trackingType.getProp("Hlt2Tracks") == Hlt2ForwardTracksName) :
        if trackingType.getProp("DoSeeding") :
            return Hlt2LongTracksName
        else :
            return Hlt2ForwardTracksName
    elif (trackingType.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName) :
        return Hlt2DownstreamTracksName
#
# Now the "long" track location, for the tracks which will be used to
# make particles, protoparticles, etc.  
# 
def _trackLocation(trackingType):
    if (trackingType.getProp("FastFitType") == "") : return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + _shortTrackLocation(trackingType)
    else : return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + trackingType.getProp("FastFitType") + "/" + _shortTrackLocation(trackingType)
#
# For protos, the format is e.g. Hlt2/ProtoP/Unfitted/Charged/Forward 
#   
def _protosLocation(trackingType,protosType):
    if (trackingType.getProp("FastFitType") == "") : return trackingType.getProp("Prefix") + "/" + HltGlobalProtoPLocation + "/" + protosType +  _shortTrackLocation(trackingType)
    else : return trackingType.getProp("Prefix") + "/" + HltGlobalProtoPLocation + "/" + trackingType.getProp("FastFitType") + "/" + protosType + _shortTrackLocation(trackingType)
#
# The trackified Muon ID location
#
def _trackifiedMuonIDLocation(trackingType) :
    if (trackingType.getProp("FastFitType") == "") : 
        return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + _shortTrackLocation(trackingType) + "/"+ HltMuonTracksName
    else : 
        return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + trackingType.getProp("FastFitType") + "/" + _shortTrackLocation(trackingType) + "/"+ HltMuonTracksName
#
# The trackified AllMuon ID location
# 
def _trackifiedAllMuonIDLocation(trackingType) :
    if (trackingType.getProp("FastFitType") == "") : 
        return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + _shortTrackLocation(trackingType) + "/"+ HltAllMuonTracksName
    else : 
        return trackingType.getProp("Prefix") + "/" + HltGlobalTrackLocation + "/" + trackingType.getProp("FastFitType") + "/" + _shortTrackLocation(trackingType) + "/"+ HltAllMuonTracksName
#
# The MuonID objects themselves
#
def _muonIDLocation(trackingType) :
    if (trackingType.getProp("FastFitType") == "")  : 
        return trackingType.getProp("Prefix") + "/" + HltSharedMuonIDPrefix + "/" + _shortTrackLocation(trackingType) + "/" + HltSharedMuonIDSuffix
    else : 
        return trackingType.getProp("Prefix") + "/" + HltSharedMuonIDPrefix + "/" + trackingType.getProp("FastFitType") + "/" + _shortTrackLocation(trackingType) + "/" + HltSharedMuonIDSuffix
#
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
		_baseTrackLocation,
		_shortTrackLocation,
		_trackLocation,
		_protosLocation,
		_trackifiedMuonIDLocation,
		_trackifiedAllMuonIDLocation,
		_muonIDLocation
	  ) 
