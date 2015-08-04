# A module to hold the hardcoded names of tracks/protoparticles in the Hlt
# and rules for deriving containers from them  
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
########################################################################
# Globals
########################################################################
from HltTracking.Hlt1TrackNames import HltGlobalEventPrefix
########################################################################
# These "globals" define that Tracks go into .../Track/... and
# protoparticles into .../ProtoP/...
from HltTracking.Hlt1TrackNames import HltGlobalTrackLocation
HltGlobalProtoPLocation			= "ProtoP"
#
########################################################################
# The rules for generating track and proto particle containers
# These rules apply to HLT2 TODO: add rule for HLT1 usable from Hlt1Units
########################################################################
# For tracks, the format is e.g. Hlt2/Track/Unfitted/Forward 
#
# First of all we have the "base" track and protoparticle
# location; this just defines that tracks and protoparticles go into
# some_prefix_you_choose/Track/... and some_refix_you_choose/ProtoP/... 
#
from HltTracking.Hlt1TrackNames import _baseTrackLocation
#
def _baseProtoPLocation(prefix,protos) :
    return prefix + "/" + HltGlobalProtoPLocation + "/" + protos

########################################################################
# Tracks
########################################################################
# prefixes where to put the tracks (these go into the prefix field of 
# _trackLocation and _protosLocation)
#
HltSharedTracksPrefix 		= HltGlobalEventPrefix + "Hlt"
Hlt2TracksPrefix 			= HltGlobalEventPrefix + "Hlt2"

Hlt2TrackRoot               = Hlt2TracksPrefix + "/" + HltGlobalTrackLocation
HltSharedTrackRoot          = HltSharedTracksPrefix + "/" + HltGlobalTrackLocation

Hlt2TrackEffRoot  =  Hlt2TracksPrefix + "/TrackEff" 

#
# names of track types (these go into the tracks field of _trackLocation 
# and _protosLocation)
#
HltSharedRZVeloTracksName               = "RZVelo"

from HltTracking.Hlt1TrackNames import TrackName
# The prefix says where this track has been produced
Hlt2TrackLoc = { name : _baseTrackLocation(Hlt2TracksPrefix,loc) for name,loc in TrackName.iteritems() }
HltSharedTrackLoc = { name : _baseTrackLocation(HltSharedTracksPrefix,loc) for name,loc in TrackName.iteritems() }

# Names for tracking efficiency probe tracks
TrackEffNames   = { "MuonTT"            : "MuonTT"         # probe track for tracking efficiency from Muon and TT hits
                  , "VeloMuon"          : "VeloMuon"       # probe track for tracking efficiency from Velo and Muon hits
                  , "FullDownstream"    : "FullDownstream" # probe track for tracking efficiency, needs full downstream reco.
                  , "StandaloneMuon"    : "StandaloneMuon" # muon track for tracking efficiency, used by VeloMuon tracks
		  }

# Locations for tracking efficiency probe tracks
Hlt2TrackEffLoc = { name : _baseTrackLocation(Hlt2TrackEffRoot,loc) for name,loc in TrackEffNames.iteritems() }

# check if the Decoders are writing to the correct locations
from DAQSys.Decoders import DecoderDB
DecoderLocations = DecoderDB["HltTrackReportsDecoder"].listOutputs()
from HltTracking.Hlt1TrackNames import Hlt1TrackLoc
for loc in DecoderLocations :
    if not loc in Hlt1TrackLoc.values() + HltSharedTrackLoc.values()  : 
        print "TrackReports location: " + loc + " not found in track locations. Go synchronize HltTrackNames.py and HltDAQ."
        raise Exception("TrackReports location not found in TrackNames") 
    #else : print "Checked TrackReports location "+loc
    #endif#endloop    

#HltSharedVeloTracksName               = "Velo"
#HltSharedVeloTTTracksName               = "VeloTT"
#HltSharedForwardTracksName               = "Forward"
#
#HltSharedVeloLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedVeloTracksName)
#HltSharedVeloTTLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedVeloTTTracksName)
#HltSharedForwardLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedForwardTracksName)

#
#Hlt1SeedingTracksName                   = "Seeding"

#Hlt1ForwardPestimateTracksName          = "PestiForward"

#
#Hlt2VeloTracksName 			= "Velo"
#Hlt2ForwardTracksName 			= "Forward"
#Hlt2ForwardSecondLoopTracksName = "ForwardSecondLoop"
#Hlt2MatchTracksName 			= "Match"
#Hlt2LongTracksName 			= "Long"
#Hlt2SeedingTracksName 			= Hlt1SeedingTracksName
#Hlt2DownstreamTracksName 		= "SeedTT"
# The next two are "trackified" PID objects
HltMuonTracksName			= "MuonSegments"
HltAllMuonTracksName			= "AllMuonSegments"
#
# types of track fit (including no fit!) (these go into the fastFitType 
# field of _trackLocation and _protosLocation)
#
#HltUnfittedTracksSuffix			= "Unfitted"
#HltBiDirectionalKalmanFitSuffix 	= "BiKalmanFitted"
#HltUniDirectionalKalmanFitSuffix 	= "UniKalmanFitted"
HltDefaultFitSuffix 	                = "Fitted"  
#
# The recognised track types for the Hlt2 Tracking
#
Hlt2TrackingRecognizedTrackTypes 	= [ "Forward",
                                            "Long", 
                                            "Downstream",
                                            "Best"]
#
# The recognised fit types for the Hlt2 Tracking
# 
Hlt2TrackingRecognizedFitTypes		= [ HltDefaultFitSuffix
                                            #HltUnfittedTracksSuffix,
                                            #HltBiDirectionalKalmanFitSuffix,
                                            #HltUniDirectionalKalmanFitSuffix
					  ]
#
Hlt2TrackingRecognizedFitTypesForRichID = [
                                            HltDefaultFitSuffix
                                            ]	
#




########################################################################
# ProtoParticles
########################################################################
# The suffix (this goes into the type field of _protosLocation)
#
Hlt2ChargedProtoParticleSuffix 		= "Charged"
Hlt2NeutralProtoParticleSuffix 		= "Neutrals" 
#
########################################################################
# PID
########################################################################
# We want to generate PID containers for different track types the
# same way in which we generate tracks, meaning using the same rules,
# because we need different PID for different tracks; they should not 
# overwrite each other!
# 
HltSharedPIDPrefix			= "PID"
HltNoPIDSuffix				= "NoPID"
HltAllPIDsSuffix                        = "AllPIDs"
HltMuonIDSuffix				= "Muon"
HltRICHIDSuffix				= "RICH"
HltCALOIDSuffix				= "CALO"

# And the subdirectories. These are necessary so that different algorithms
# using e.g. different options for the RICH reco (radiators, hypotheses) 
# don't clash with each other 
#
HltAllPIDsProtosSuffix                  = "WithAllPIDs"
HltCaloProtosSuffix			= "WithCaloID"
HltMuonProtosSuffix			= "WithMuonID"
HltCaloAndMuonProtosSuffix		= "WithCaloAndMuonID"
HltRichProtosSuffix			= "WithRichID"

from HltTracking.Hlt1TrackNames import Hlt1TracksPrefix, Hlt1TrackRoot

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
#        HltSharedVeloLocation,
#        HltSharedVeloTTLocation,
#        HltSharedForwardLocation,
		#
        TrackName,
        Hlt1TrackRoot,
        Hlt2TrackRoot,
        HltSharedTrackRoot,
        Hlt1TrackLoc,
        Hlt2TrackLoc,
        HltSharedTrackLoc,
        #
		HltSharedRZVeloTracksName,
#Hlt2ForwardTracksName, 
#		Hlt2ForwardSecondLoopTracksName,
#        Hlt2MatchTracksName, 
#		Hlt2LongTracksName, 
#		Hlt2SeedingTracksName, 
#		Hlt2DownstreamTracksName,
		HltMuonTracksName,
		HltAllMuonTracksName,
		#
                HltDefaultFitSuffix,
		#HltUnfittedTracksSuffix, 
		#HltBiDirectionalKalmanFitSuffix, 
		#HltUniDirectionalKalmanFitSuffix,
		#
		Hlt2TrackingRecognizedTrackTypes,
		Hlt2TrackingRecognizedFitTypes, 
		Hlt2TrackingRecognizedFitTypesForRichID,
		#
		Hlt2ChargedProtoParticleSuffix, 
		Hlt2NeutralProtoParticleSuffix,
		#	
		HltSharedPIDPrefix,
		HltNoPIDSuffix,
                HltAllPIDsSuffix,
		HltMuonIDSuffix,   
		HltRICHIDSuffix,   
		HltCALOIDSuffix,
		#
		#HltRichDefaultHypos,
		#HltRichDefaultRadiators,
		#
                HltAllPIDsProtosSuffix,
		HltCaloProtosSuffix,
		HltMuonProtosSuffix,                     
                HltCaloAndMuonProtosSuffix,
		HltRichProtosSuffix,                   
		#
		# The functions
		#
		_baseTrackLocation,
		_baseProtoPLocation

	  ) 
