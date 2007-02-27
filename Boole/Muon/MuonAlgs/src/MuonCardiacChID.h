#ifndef MuonAlgs_MuonCardiacChID_H
#define MuonAlgs_MuonCardiacChID_H  1

// Include files
#include "Kernel/MuonTileID.h"
#include "MuonCardiacIDPack.h"
#include "GaudiKernel/MsgStream.h" 

class MuonCardiacChID{
public:

MuonCardiacChID():m_channelID(0){};
MuonCardiacChID(unsigned int ID ):m_channelID(ID){};
~MuonCardiacChID(){};
void setStation(unsigned int value);
void setRegion(unsigned int value);
void setQuadrant(unsigned int value);
void setChamber(unsigned int value);
void setChIDX(unsigned int value);
void setChIDY(unsigned int value);
void setReadout(unsigned int value);

unsigned int getID() const;
unsigned int getStation() const;
unsigned int getRegion() const;
unsigned int getQuadrant() const;
unsigned int getChamber() const;
unsigned int getChIDX() const;
unsigned int getChIDY() const;
unsigned int getReadout() const;

/// printout to std::ostream 
inline std::ostream& printOut ( std::ostream& ) const ;
/// printout to MsgStream
inline MsgStream&    printOut ( MsgStream&    ) const ;
  
 bool operator==(const MuonCardiacChID& id) const;


private:
unsigned int m_channelID;

};

inline unsigned int MuonCardiacChID::getID() const{
return m_channelID;
}


inline unsigned int MuonCardiacChID::getStation() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftStation,MuonCardiacIDPack::maskStation);
}

inline unsigned int MuonCardiacChID::getRegion() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftRegion,MuonCardiacIDPack::maskRegion);
}
inline unsigned int MuonCardiacChID::getQuadrant() const{
return 
MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftQuadrant,MuonCardiacIDPack::maskQuadrant);
}
inline unsigned int MuonCardiacChID::getChamber() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftChamber,MuonCardiacIDPack::maskChamber);
}
inline unsigned int MuonCardiacChID::getChIDX() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftPhChIDX,MuonCardiacIDPack::maskPhChIDX);
}
inline unsigned int MuonCardiacChID::getChIDY() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftPhChIDY,MuonCardiacIDPack::maskPhChIDY);
}

inline unsigned int MuonCardiacChID::getReadout() const{
return MuonCardiacIDPack::getBit(m_channelID,MuonCardiacIDPack::shiftReadout,MuonCardiacIDPack::maskReadout);
}


inline void MuonCardiacChID::setStation(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftStation,MuonCardiacIDPack::maskStation,value);
}

inline void MuonCardiacChID::setRegion(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftRegion,MuonCardiacIDPack::maskRegion,value);
}

inline void MuonCardiacChID::setQuadrant(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftQuadrant,MuonCardiacIDPack::maskQuadrant,value);
}

inline void MuonCardiacChID::setChamber(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftChamber,MuonCardiacIDPack::maskChamber,value);
}

inline void MuonCardiacChID::setChIDX(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftPhChIDX,MuonCardiacIDPack::maskPhChIDX,value);
}

inline void MuonCardiacChID::setChIDY(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftPhChIDY,MuonCardiacIDPack::maskPhChIDY,value);
}


inline void MuonCardiacChID::setReadout(unsigned int value){
MuonCardiacIDPack::setBit(m_channelID,MuonCardiacIDPack::shiftReadout,MuonCardiacIDPack::maskReadout,value);
}




// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonCardiacChID& id ) {
  return id.printOut( os );  
}

inline MsgStream& operator<<( MsgStream&    os , const MuonCardiacChID& id ) {
  return id.printOut( os ); 
}

inline MsgStream& MuonCardiacChID::printOut( MsgStream& os ) const { 
  os << "[" ;
  return 
    os <<  getStation() << ","
        << getRegion() << "," 
       <<  getQuadrant() << "," 
       <<  getChamber() << "," 
       <<  getChIDX() << "," 
       <<  getChIDY() << "," 
       <<  getReadout() << "]" ;   
}

inline std::ostream& MuonCardiacChID::printOut( std::ostream& os ) const { 
  os << "[" ;
  return 
    os <<  getStation() << ","
        << getRegion() << "," 
       <<  getQuadrant() << "," 
       <<  getChamber() << "," 
       <<  getChIDX() << "," 
       <<  getChIDY() << "," 
       <<  getReadout()  << "]" ;   
 }

inline bool MuonCardiacChID::operator==(const MuonCardiacChID& id) const
{
 return m_channelID ==  id.m_channelID ;
}



#endif
