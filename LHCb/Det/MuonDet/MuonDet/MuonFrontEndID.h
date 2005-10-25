#ifndef MUONDET_MUONFRONTENDID_H
#define MUONDET_MUONFRONTENDID_H 1


// Include files

#include "MuonDet/MuonFrontEndIDPack.h"


using namespace MuonFrontEndIDPack ;
class MuonFrontEndID{
public:

MuonFrontEndID():m_channelID(0){};
MuonFrontEndID(unsigned int ID ):m_channelID(ID){};
~MuonFrontEndID(){};

void setReadout(unsigned int value);
void setLayer(unsigned int value);
void setFEGridX(unsigned int value);
void setFEGridY(unsigned int value);
void setFEIDX(unsigned int value);
void setFEIDY(unsigned int value);


unsigned int getID() const;

unsigned int getReadout() const;
unsigned int getLayer() const;
unsigned int getFEGridX() const;
unsigned int getFEGridY() const;
unsigned int getFEIDX() const;
unsigned int getFEIDY() const;

/// printout to std::ostream 
inline std::ostream& printOut ( std::ostream& ) const ;
/// printout to MsgStream
inline MsgStream&    printOut ( MsgStream&    ) const ;
  


private:
unsigned int m_channelID;

};

inline unsigned int MuonFrontEndID::getID() const{
return m_channelID;
}


inline unsigned int MuonFrontEndID::getReadout() const{
return getBit(m_channelID,shiftReadout,maskReadout);
}
inline unsigned int MuonFrontEndID::getLayer() const{
return getBit(m_channelID,shiftLayer,maskLayer);
}
inline unsigned int MuonFrontEndID::getFEGridX() const{
return getBit(m_channelID,shiftFEGridX,maskFEGridX);
}
inline unsigned int MuonFrontEndID::getFEGridY() const{
return getBit(m_channelID,shiftFEGridY,maskFEGridY);
}
inline unsigned int MuonFrontEndID::getFEIDX() const{
return getBit(m_channelID,shiftFEIDX,maskFEIDX);
}
inline unsigned int MuonFrontEndID::getFEIDY() const{
return getBit(m_channelID,shiftFEIDY,maskFEIDY);
}

inline void MuonFrontEndID::setReadout(unsigned int value){
setBit(m_channelID,shiftReadout,maskReadout,value);
}
inline void MuonFrontEndID::setLayer (unsigned int value){
setBit(m_channelID,shiftLayer ,maskLayer ,value);
}
inline void MuonFrontEndID::setFEGridX(unsigned int value){
setBit(m_channelID,shiftFEGridX,maskFEGridX,value);
}
inline void MuonFrontEndID::setFEGridY(unsigned int value){
setBit(m_channelID,shiftFEGridY,maskFEGridY,value);
}
inline void MuonFrontEndID::setFEIDX(unsigned int value){
setBit(m_channelID,shiftFEIDX,maskFEIDX,value);
}
inline void MuonFrontEndID::setFEIDY(unsigned int value){
setBit(m_channelID,shiftFEIDY,maskFEIDY,value);
}


// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonFrontEndID& id ) {
  return id.printOut( os );  
}

inline MsgStream& operator<<( MsgStream&    os , const MuonFrontEndID& id ) {
  return id.printOut( os ); 
}

inline MsgStream& MuonFrontEndID::printOut( MsgStream& os ) const { 
  os << "[" ;
  return 
    os <<  getReadout() << "," 
       <<  getLayer() << "," 
       <<  getFEGridX() << "," 
       <<  getFEGridY() << "," 
       <<  getFEIDX() << "," 
       <<  getFEIDY() << "]" ;   
}

inline std::ostream& MuonFrontEndID::printOut( std::ostream& os ) const { 
  os << "[" ;
  return 
    os <<  getReadout() << "," 
       <<  getLayer() << "," 
       <<  getFEGridX() << "," 
       <<  getFEGridY() << "," 
       <<  getFEIDX() << "," 
       <<  getFEIDY() << "]" ;
 }


#endif 

