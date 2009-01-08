#
# contains the keys and values for the lumi counters
#
from Configurables import ApplicationMgr, LumiANNSvc 

ApplicationMgr().ExtSvc.append( "LumiANNSvc"  )

LumiANNSvc().LumiCounters = {
    "PUMult"     : 0   ,
    "RZVelo"     : 1   ,
    "RZVeloBW"   : 2   , 
    "Velo"       : 3   , 
    "Muon"       : 4   , 
    "BKMuon"     : 5   , 
    "SPDMult"    : 6   , 
    "CaloEt"     : 7   , 
    "PV2D"       : 10  , 
    "PV3D"       : 11  , 
    "PU"         : 13  ,
};
