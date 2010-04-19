from Gaudi import Configuration
from Configurables import PatForwardTool, PatSeedingTool
from HltLine.HltLine import Hlt1Tool   as Tool

def ConfiguredPR( tool ):
    if tool is "Forward" :
        # to come: if Hlttrackin.earlyData = true or similar
        return Tool( PatForwardTool
                     , MinXPlanes = 4
                     , MinPlanes = 8
                     , MaxSpreadX = 1.5
                     , MaxSpreadY = 3.0
                     , MaxChi2 = 40
                     , MaxChi2Track = 40
                     , MinHits = 12
                     , MinOTHits = 14 )
    elif tool is "PatSeeding":
        return Tool( PatSeedingTool,
                     OTNHitsLowThresh=12,
                     MinTotalPlanes = 7,
                     MaxMisses = 2,
                     MaxTrackChi2LowMult=10,
                     MaxFinalTrackChi2=20,
                     MaxFinalChi2=30,
                     MaxTrackChi2=40,
                     MaxChi2HitIT=10,
                     MaxChi2HitOT=30
                     ) 
    else :
        raise KeyError('unknown tool %s requested' % tool)


    
            ## else if tool is "TsaSeeding":
##                 if  TrackSys().earlyData():
##                     tsaSeed.addTool(Tf__Tsa__Likelihood(), name = "likelihood")
##                     tsaSeed.likelihood.LikCut = -40
##                     Tf__Tsa__SeedTrackCnv("TsaSeedTrackCnv").LikCut = -40
##                 return Tool( TsaSeeding

