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
#include "GaudiKernel/MsgStream.h" 

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
unsigned int getStation() const;
unsigned int getRegion() const;
unsigned int getQuadrant() const;
unsigned int getChamber() const;
unsigned int getPhChIDX() const;
unsigned int getPhChIDY() const;
unsigned int getFrontEnd() const;
unsigned int getReadout() const;
unsigned int getGap() const;
unsigned int getFETile() const;
/// printout to std::ostream 
inline std::ostream& printOut ( std::ostream& ) const ;
/// printout to MsgStream
inline MsgStream&    printOut ( MsgStream&    ) const ;
  


private:
unsigned int m_channelID;

};

inline unsigned int MuonPhChID::getID() const{
return m_channelID;
}


inline unsigned int MuonPhChID::getStation() const{
return getBit(m_channelID,shiftStation,maskStation);
}

inline unsigned int MuonPhChID::getRegion() const{
return getBit(m_channelID,shiftRegion,maskRegion);
}
inline unsigned int MuonPhChID::getQuadrant() const{
return getBit(m_channelID,shiftQuadrant,maskQuadrant);
}
inline unsigned int MuonPhChID::getChamber() const{
return getBit(m_channelID,MuonPhChIDPack::shiftChamber,maskChamber);
}
inline unsigned int MuonPhChID::getPhChIDX() const{
return getBit(m_channelID,shiftPhChIDX,maskPhChIDX);
}
inline unsigned int MuonPhChID::getPhChIDY() const{
return getBit(m_channelID,shiftPhChIDY,maskPhChIDY);
}
inline unsigned int MuonPhChID::getFrontEnd() const{
return getBit(m_channelID,shiftFrontEnd,maskFrontEnd);
}
inline unsigned int MuonPhChID::getReadout() const{
return getBit(m_channelID,shiftReadout,maskReadout);
}
inline unsigned int MuonPhChID::getGap() const{
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

inline unsigned int MuonPhChID::getFETile() const{
//return 1;
return m_channelID-getBit(m_channelID,shiftGap,maskGap);
}

// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonPhChID& id ) {
  return id.printOut( os );  
}

inline MsgStream& operator<<( MsgStream&    os , const MuonPhChID& id ) {
  return id.printOut( os ); 
}

inline MsgStream& MuonPhChID::printOut( MsgStream& os ) const { 
  os << "[" ;
  return 
    os <<  getStation() << ","
        << getRegion() << "," 
       <<  getQuadrant() << "," 
       <<  getChamber() << "," 
       <<  getPhChIDX() << "," 
       <<  getPhChIDY() << "," 
       <<  getFrontEnd() << "," 
       <<  getReadout() << "," 
       <<  getGap() << "]" ;   
}

inline std::ostream& MuonPhChID::printOut( std::ostream& os ) const { 
  os << "[" ;
  return 
    os <<  getStation() << ","
        << getRegion() << "," 
       <<  getQuadrant() << "," 
       <<  getChamber() << "," 
       <<  getPhChIDX() << "," 
       <<  getPhChIDY() << "," 
       <<  getFrontEnd() << "," 
       <<  getReadout() << "," 
       <<  getGap()  << "]" ;   
 }



#endif
