#ifndef MuonAlgs_MuonPhChID_H
#define MuonAlgs_MuonPhChID_H  1

// Include files
#include <vector>
#include <algorithm>
#include "Kernel/CLHEPStreams.h"
#include "Event/MuonEventBase.h"
#include "MuonKernel/MuonTileID.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "MuonAlgs/MuonPhChIDPack.h"

using namespace MuonPhChIDPack ;
class MuonPhChID{
public:

MuonPhChID():m_channelID(0){};
MuonPhChID(unsigned int ID ):m_channelID(ID){};
~MuonPhChID(){};
void setStation(unsigned int value);
void setRegion(unsigned int value);
void setQuadrant(unsigned int value);
void setChamber(unsigned int value);
void setPhChIDX(unsigned int value);
void setPhChIDY(unsigned int value);
void setFrontEnd(unsigned int value);
void setReadout(unsigned int value);
void setGap(unsigned int value);

unsigned int getID() const;
unsigned int getStation();
unsigned int getRegion();
unsigned int getQuadrant();
unsigned int getChamber();
unsigned int getPhChIDX();
unsigned int getPhChIDY();
unsigned int getFrontEnd();
unsigned int getReadout();
unsigned int getGap();
unsigned int getFETile();

private:
unsigned int m_channelID;

};

inline unsigned int MuonPhChID::getID() const{
return m_channelID;
}


inline unsigned int MuonPhChID::getStation(){
return getBit(m_channelID,shiftStation,maskStation);
}

inline unsigned int MuonPhChID::getRegion(){
return getBit(m_channelID,shiftRegion,maskRegion);
}
inline unsigned int MuonPhChID::getQuadrant(){
return getBit(m_channelID,shiftQuadrant,maskQuadrant);
}
inline unsigned int MuonPhChID::getChamber(){
return getBit(m_channelID,MuonPhChIDPack::shiftChamber,maskChamber);
}
inline unsigned int MuonPhChID::getPhChIDX(){
return getBit(m_channelID,shiftPhChIDX,maskPhChIDX);
}
inline unsigned int MuonPhChID::getPhChIDY(){
return getBit(m_channelID,shiftPhChIDY,maskPhChIDY);
}
inline unsigned int MuonPhChID::getFrontEnd(){
return getBit(m_channelID,shiftFrontEnd,maskFrontEnd);
}
inline unsigned int MuonPhChID::getReadout(){
return getBit(m_channelID,shiftReadout,maskReadout);
}
inline unsigned int MuonPhChID::getGap(){
return getBit(m_channelID,shiftGap,maskGap);
}

inline void MuonPhChID::setStation(unsigned int value){
setBit(m_channelID,shiftStation,maskStation,value);
}

inline void MuonPhChID::setRegion(unsigned int value){
setBit(m_channelID,shiftRegion,maskRegion,value);
}

inline void MuonPhChID::setQuadrant(unsigned int value){
setBit(m_channelID,shiftQuadrant,maskQuadrant,value);
}

inline void MuonPhChID::setChamber(unsigned int value){
setBit(m_channelID,shiftChamber,maskChamber,value);
}

inline void MuonPhChID::setPhChIDX(unsigned int value){
setBit(m_channelID,shiftPhChIDX,maskPhChIDX,value);
}

inline void MuonPhChID::setPhChIDY(unsigned int value){
setBit(m_channelID,shiftPhChIDY,maskPhChIDY,value);
}

inline void MuonPhChID::setFrontEnd(unsigned int value){
setBit(m_channelID,shiftFrontEnd,maskFrontEnd,value);
}

inline void MuonPhChID::setReadout(unsigned int value){
setBit(m_channelID,shiftReadout,maskReadout,value);
}

inline void MuonPhChID::setGap(unsigned int value){
setBit(m_channelID,shiftGap,maskGap,value);
}

inline unsigned int MuonPhChID::getFETile(){
//return 1;
return m_channelID-getBit(m_channelID,shiftGap,maskGap);
}

#endif
