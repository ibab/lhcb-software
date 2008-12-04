# Options to initialize MCParticle to MCHit associators

from Configurables import MCParticle2MCHitAlg, TrackerMCParticle2MCHitAlg, DataOnDemandSvc

# Define the algorithms
veloAssoc = MCParticle2MCHitAlg( "MCP2VeloMCHitAlg", MCHitPath = "MC/Velo/Hits", OutputData = "/Event/MC/Particles2MCVeloHits" )
ttAssoc   = MCParticle2MCHitAlg( "MCP2TTMCHitAlg",   MCHitPath = "MC/TT/Hits",   OutputData = "/Event/MC/Particles2MCTTHits" )
itAssoc   = MCParticle2MCHitAlg( "MCP2ITMCHitAlg",   MCHitPath = "MC/IT/Hits",   OutputData = "/Event/MC/Particles2MCITHits" )
otAssoc   = MCParticle2MCHitAlg( "MCP2OTMCHitAlg",   MCHitPath = "MC/OT/Hits",   OutputData = "/Event/MC/Particles2MCOTHits" )
muonAssoc = MCParticle2MCHitAlg( "MCP2MuonMCHitAlg", MCHitPath = "MC/Muon/Hits", OutputData = "/Event/MC/Particles2MCMuonHits" )

trackerAssoc = TrackerMCParticle2MCHitAlg( "MCP2TrackerHitAlg", OutputData = "/Event/MC/Particles2MCTrackerHits" )

# tell the Data On Demand Service about them
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCVeloHits" ]    = veloAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCTTHits" ]      = ttAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCITHits" ]      = itAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCOTHits" ]      = otAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCMuonHits" ]    = muonAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCTrackerHits" ] = trackerAssoc

DataOnDemandSvc().NodeMap[ "/Event/Link" ]    = "DataObject"
DataOnDemandSvc().NodeMap[ "/Event/Link/MC" ] = "DataObject"




