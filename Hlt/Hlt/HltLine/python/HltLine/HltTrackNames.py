# A module to hold the hardcoded names of tracks in the Hlt
# and rules for deriving containers from them  

# Tracks

HltSharedTracksPrefix 			= "Hlt"

HltSharedRZVeloTracksName 		= "RZVelo"

Hlt1TracksPrefix 			= "Hlt1"

Hlt2TracksPrefix 			= "Hlt2"

Hlt2VeloTracksName 			= "Velo"

Hlt2ForwardTracksName 			= "Forward"

Hlt2MatchTracksName 			= "Match"

Hlt2LongTracksName 			= "Long"

Hlt2SeedingTracksName 			= "Seeding"

Hlt2DownstreamTracksName 		= "SeedTT"

HltBiDirectionalKalmanFitSuffix 	= "BiKalmanFitted"

HltUniDirectionalKalmanFitSuffix 	= "UniKalmanFitted"  

# ProtoParticles
# These share "prefixes" with the tracks

Hlt2SharedProtoParticleSuffix 		= "ProtoP"

Hlt2ChargedProtoParticleSuffix 		= "Charged"

Hlt2NeutralProtoParticleSuffix 		= "Neutral" 


__all__ = (	HltSharedTracksPrefix, 
           	Hlt1TracksPrefix, 
           	Hlt2TracksPrefix, 
		Hlt2ForwardTracksName, 
		Hlt2MatchTracksName, 
		Hlt2LongTracksName, 
		Hlt2SeedingTracksName, 
		Hlt2DownstreamTracksName, 
		HltBiDirectionalKalmanFitSuffix, 
		HltUniDirectionalKalmanFitSuffix, 
		Hlt2SharedProtoParticleSuffix, 
		Hlt2ChargedProtoParticleSuffix, 
		Hlt2NeutralProtoParticleSuffix
	  ) 
