# Options to initialize MCParticle to MCHit associators for the upgrade

from Configurables import MCParticle2MCHitAlg, DataOnDemandSvc

# Define the algorithms
vpAssoc = MCParticle2MCHitAlg( "MCP2VPMCHitAlg", MCHitPath = "MC/VP/Hits", OutputData = "/Event/MC/Particles2MCVPHits" )
utAssoc   = MCParticle2MCHitAlg( "MCP2UTMCHitAlg",   MCHitPath = "MC/UT/Hits",   OutputData = "/Event/MC/Particles2MCUTHits" )
ftAssoc   = MCParticle2MCHitAlg( "MCP2FTMCHitAlg",   MCHitPath = "MC/FT/Hits",   OutputData = "/Event/MC/Particles2MCFTHits" )
muonAssoc = MCParticle2MCHitAlg( "MCP2MuonMCHitAlg", MCHitPath = "MC/Muon/Hits", OutputData = "/Event/MC/Particles2MCMuonHits" )

# tell the Data On Demand Service about them
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCVPHits" ]    = vpAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCUTHits" ]      = utAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCFTHits" ]      = ftAssoc
DataOnDemandSvc().AlgMap[ "/Event/Link/MC/Particles2MCMuonHits" ]    = muonAssoc

DataOnDemandSvc().NodeMap[ "/Event/Link" ]    = "DataObject"
DataOnDemandSvc().NodeMap[ "/Event/Link/MC" ] = "DataObject"




