#ifndef MUONALGS_MUONHITORIGIN_H 
#define MUONALGS_MUONHITORIGIN_H 1

// Include files
  
#include "MuonHitOriginPack.h" 
#include "Event/MuonBXFlag.h"
#include "Event/MuonOriginFlag.h"

class MuonHitOrigin{
public:

bool  getGeantNature();
bool  getBackgroundNature();
bool  getFlatSpilloverNature();
bool  getChamberNoiseNature();
bool  getXTalkNature();
bool  getElectronicNoiseNature();

unsigned int  getBX();
unsigned int getNature();
  
void setHitNature(unsigned int value);
void setBX(unsigned int value);

private :

unsigned int m_hitOrigin ;
};

inline unsigned int MuonHitOrigin::getNature(){
return
MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin);
}


inline bool MuonHitOrigin::getGeantNature(){
return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)==LHCb::MuonOriginFlag::GEANT);
}
inline bool MuonHitOrigin::getBackgroundNature(){
 return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)== 
LHCb::MuonOriginFlag::BACKGROUND);
}
inline bool MuonHitOrigin::getFlatSpilloverNature(){
return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)== 
LHCb::MuonOriginFlag::FLATSPILLOVER);
 }
inline bool MuonHitOrigin::getChamberNoiseNature(){
return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)==  
LHCb::MuonOriginFlag::CHAMBERNOISE);
 }
inline bool MuonHitOrigin::getXTalkNature(){
return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)== 
LHCb::MuonOriginFlag::XTALK);
 }

inline bool MuonHitOrigin::getElectronicNoiseNature(){
return
(MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin)== 
LHCb::MuonOriginFlag::ELECTRONICNOISE);
 }


inline unsigned int MuonHitOrigin::getBX(){
return MuonHitOriginPack::getBit(m_hitOrigin,MuonHitOriginPack::shiftBX,MuonHitOriginPack::maskBX);
}

inline void MuonHitOrigin::setHitNature(unsigned int value){
MuonHitOriginPack::setBit(m_hitOrigin,MuonHitOriginPack::shiftHitOrigin,MuonHitOriginPack::maskHitOrigin,value);
}

inline void MuonHitOrigin::setBX(unsigned int value){
MuonHitOriginPack::setBit(m_hitOrigin,MuonHitOriginPack::shiftBX,MuonHitOriginPack::maskBX,value);
} 

#endif
