#ifndef MuonAlgs_MuonPhChID_H
#define MuonAlgs_MuonPhChID_H  1

// Include files
#include "Kernel/MuonTileID.h"
#include "MuonPhChIDPack.h"
#include "GaudiKernel/MsgStream.h" 

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
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftStation,MuonPhChIDPack::maskStation);
}

inline unsigned int MuonPhChID::getRegion() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftRegion,MuonPhChIDPack::maskRegion);
}
inline unsigned int MuonPhChID::getQuadrant() const{
return 
MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftQuadrant,MuonPhChIDPack::maskQuadrant);
}
inline unsigned int MuonPhChID::getChamber() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftChamber,MuonPhChIDPack::maskChamber);
}
inline unsigned int MuonPhChID::getPhChIDX() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftPhChIDX,MuonPhChIDPack::maskPhChIDX);
}
inline unsigned int MuonPhChID::getPhChIDY() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftPhChIDY,MuonPhChIDPack::maskPhChIDY);
}
inline unsigned int MuonPhChID::getFrontEnd() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftFrontEnd,MuonPhChIDPack::maskFrontEnd);
}
inline unsigned int MuonPhChID::getReadout() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftReadout,MuonPhChIDPack::maskReadout);
}
inline unsigned int MuonPhChID::getGap() const{
return MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftGap,MuonPhChIDPack::maskGap);
}

inline void MuonPhChID::setStation(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftStation,MuonPhChIDPack::maskStation,value);
}

inline void MuonPhChID::setRegion(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftRegion,MuonPhChIDPack::maskRegion,value);
}

inline void MuonPhChID::setQuadrant(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftQuadrant,MuonPhChIDPack::maskQuadrant,value);
}

inline void MuonPhChID::setChamber(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftChamber,MuonPhChIDPack::maskChamber,value);
}

inline void MuonPhChID::setPhChIDX(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftPhChIDX,MuonPhChIDPack::maskPhChIDX,value);
}

inline void MuonPhChID::setPhChIDY(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftPhChIDY,MuonPhChIDPack::maskPhChIDY,value);
}

inline void MuonPhChID::setFrontEnd(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftFrontEnd,MuonPhChIDPack::maskFrontEnd,value);
}

inline void MuonPhChID::setReadout(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftReadout,MuonPhChIDPack::maskReadout,value);
}

inline void MuonPhChID::setGap(unsigned int value){
MuonPhChIDPack::setBit(m_channelID,MuonPhChIDPack::shiftGap,MuonPhChIDPack::maskGap,value);
}

inline unsigned int MuonPhChID::getFETile() const{
//return 1;
return m_channelID-MuonPhChIDPack::getBit(m_channelID,MuonPhChIDPack::shiftGap,MuonPhChIDPack::maskGap);
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
