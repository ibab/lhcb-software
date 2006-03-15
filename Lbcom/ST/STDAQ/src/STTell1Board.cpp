// $Id: STTell1Board.cpp,v 1.2 2006-03-15 16:38:51 mneedham Exp $
#include "STTell1Board.h"


using namespace LHCb;

STTell1Board::STTell1Board(const STTell1ID aBoard, 
                           const unsigned int nStripsPerHybrid):
  m_boardID(aBoard),
  m_nStripsPerHybrid(nStripsPerHybrid)
{
  // constructer
  m_sectorsVector.reserve(8);
}

STTell1Board::~STTell1Board(){
  // destructer
}

void STTell1Board::addSector(STChannelID aOfflineChan, unsigned int orientation){
 // add sector to vector
 m_orientation.push_back(orientation);
 m_sectorsVector.push_back(aOfflineChan);
}


bool STTell1Board::isInside(const STChannelID aOfflineChan,
                            unsigned int& sectorIndex) const{
  // look-up the board
  unsigned int iSector = 0u;

  while ((iSector<m_sectorsVector.size())&&(aOfflineChan.uniqueSector() != m_sectorsVector[iSector].uniqueSector())){
    ++iSector;
  } // iSector
  sectorIndex = iSector;

  return (iSector != m_sectorsVector.size() ? true : false); 
}


STChannelID STTell1Board::DAQToOffline(const unsigned int aDAQChan) const{

  // convert a DAQ channel to offline !
  unsigned int index = aDAQChan/m_nStripsPerHybrid;
  unsigned int strip =  aDAQChan - (index*m_nStripsPerHybrid);
  if (m_orientation[index] == 0){
    strip = m_nStripsPerHybrid - strip;
  }
  else {
    ++strip;
  }

  return STChannelID(m_sectorsVector[index].type(),
                     m_sectorsVector[index].station(), 
                     m_sectorsVector[index].layer(),
                     m_sectorsVector[index].detRegion(),
                     m_sectorsVector[index].sector(),
                     strip);
}

unsigned int STTell1Board::offlineToDAQ(const STChannelID aOfflineChan,
                                        const unsigned int waferIndex) const{

  // convert an offline channel to DAQ channel
  unsigned int strip = aOfflineChan.strip();
  if (m_orientation[0] == 0){
    strip = m_nStripsPerHybrid - strip;
  }
  else {
    --strip;
  }
  return (waferIndex*m_nStripsPerHybrid)+ strip;
}

std::ostream& STTell1Board::fillStream( std::ostream& os ) const{

  // print function
  os << " Board: " << m_boardID.region() << " " <<  m_boardID.subID() << std::endl;
  std::vector<STChannelID>::const_iterator iterW = m_sectorsVector.begin();
  for (; iterW !=  m_sectorsVector.end() ;++iterW){
    os << (*iterW).station() << " " << (*iterW).layer() 
       << " " << (*iterW).detRegion() << " " << (*iterW).sector() << std::endl;
  }   // iW 
  os << " -----------" << std::endl; 

  return os;

}

