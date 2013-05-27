#ifndef DICT_MUONINTERFACESDICT_H 
#define DICT_MUONINTERFACESDICT_H 1

#include "MuonInterfaces/IMuonClusterTool.h"     
#include "MuonInterfaces/IMuonTimeCor.h"     
#include "MuonInterfaces/IMuonFastHWTool.h"     
#include "MuonInterfaces/IMuonHitDecode.h"
#include "MuonInterfaces/IMuonPadRec.h"
#include "MuonInterfaces/IMuonClusterRec.h"
#include "MuonInterfaces/IMuonTrackRec.h"
#include "MuonInterfaces/IMuonTrackMomRec.h"
#include "MuonInterfaces/MuonTrack.h"
#include "MuonInterfaces/MuonHit.h"
#include "MuonInterfaces/MuonNeuron.h"
#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"

MuonLogHit l1 ; 
MuonLogPad x1 ; 
MuonHit    m1 ; 
MuonTrack  t1 ; 
MuonNeuron  n1 ; 
std::vector<MuonLogHit*> k1 ; 
std::vector<MuonLogPad*> z1 ; 
std::vector<MuonHit*>    v1 ; 
std::vector<MuonTrack*>  h1 ;
std::vector<MuonNeuron*>  w1 ;

#endif //DICT_MUONINTERFACESDICT_H 
