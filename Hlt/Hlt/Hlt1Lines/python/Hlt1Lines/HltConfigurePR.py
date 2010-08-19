from Gaudi import Configuration
from Configurables import PatForwardTool, PatSeedingTool
from HltLine.HltLine import Hlt1Tool   as Tool

#TODO: pick up from HltTracking
EarlyDataTracking = False

#defaults: fwd: p>1000, pt > 80; seeding p > 500


def ConfiguredPR( tool, minPtFwd = 500., minPFwd = 5000., minPSeed = 3000):
    # Add the option to define a minimum PT/P 
    # for the tracking to consider
    # Only relevant for the forward upgrade
    if tool is "Forward" :
        if EarlyDataTracking: 
            return Tool( PatForwardTool
                         , MinXPlanes = 4
                         , MinPlanes = 8
                         , MaxSpreadX = 1.5
                         , MaxSpreadY = 3.0
                         , MaxChi2 = 40
                         , MaxChi2Track = 40
                         , MinHits = 12
                         , MinOTHits = 14 )
        else : return Tool( PatForwardTool
                         , SecondLoop = True
                         , MaxChi2 = 40
                         , MaxChi2Track = 40
                         , MinHits = 12
                         , MinOTHits = 14
                         , MinPt = minPtFwd
                         , MinMomentum = minPFwd )
        
    elif tool is "PatSeeding":
        if EarlyDataTracking:
            return Tool( PatSeedingTool,
                         OTNHitsLowThresh=12,
                         MinTotalPlanes = 7,
                         MaxMisses = 2,
                         MaxTrackChi2LowMult=10,
                         MaxFinalTrackChi2=20,
                         MaxFinalChi2=30,
                         MaxTrackChi2=40,
                         MaxChi2HitIT=10,
                         MaxChi2HitOT=30,
                         MinMomentum = minPSeed
                         )
        else : return Tool(PatSeedingTool,MinMomentum = minPSeed)
        
    else :
        raise KeyError('unknown tool %s requested' % tool)


    
            ## else if tool is "TsaSeeding":
##                 if  TrackSys().earlyData():
##                     tsaSeed.addTool(Tf__Tsa__Likelihood(), name = "likelihood")
##                     tsaSeed.likelihood.LikCut = -40
##                     Tf__Tsa__SeedTrackCnv("TsaSeedTrackCnv").LikCut = -40
##                 return Tool( TsaSeeding

