// $Id: STTell1Board.cpp,v 1.4 2008-07-15 11:21:04 mneedham Exp $
#include "Kernel/STTell1Board.h"
#include "Kernel/STDAQDefinitions.h"

#include "Event/STCluster.h"

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


STTell1Board::chanPair STTell1Board::DAQToOffline(const unsigned int aDAQChan,
                                       const unsigned int fracStrip,
                                       const int version) const{

  // convert a DAQ channel to offline !
  const unsigned int index = aDAQChan/m_nStripsPerHybrid;
  unsigned int strip =  aDAQChan - (index*m_nStripsPerHybrid);

  int interstrip = fracStrip;

  if (m_orientation[index] == 0) {
    // reverse direction of strip numbering
    strip = m_nStripsPerHybrid - strip;
    // shift channel by one, because interstrip fraction cannot be negative
    if (fracStrip != 0 && version >= STDAQ::v4 ) {
      interstrip = 4 - fracStrip;
      --strip;
    }
  } else { // Add one because offline strips start at one.
    ++strip;
  }
  
  return std::make_pair(STChannelID(m_sectorsVector[index].type(),
                     m_sectorsVector[index].station(), 
                     m_sectorsVector[index].layer(),
                     m_sectorsVector[index].detRegion(),
                     m_sectorsVector[index].sector(),
                     strip), interstrip);
}


void STTell1Board::ADCToOffline(const unsigned int aDAQChan,
                                STCluster::ADCVector& adcs,
                                const int version,
                                const unsigned int offset,
                                const double interStripPos) const
{
  const unsigned int index = aDAQChan/m_nStripsPerHybrid;
  const unsigned int size = adcs.size();
  int newoffset = 0;

  // flip sequence of adc vector
  if (m_orientation[index] == 0 && version >= STDAQ::v4){
    // calculate the new offset
    newoffset = size - 1 - offset;
    // Correct for interstrip fraction when not equal to zero
    if( interStripPos > 0.01 && interStripPos < 0.99 ) {
      --newoffset;
    }
    if (newoffset < 0) newoffset = 0;

    // Do the actual flipping
    STCluster::ADCVector adcsflip = adcs;
    for(unsigned int i = 0u; i < size; ++i) {
      adcsflip[size -i -1] = adcs[i];
      adcsflip[size -i -1].first = size -i -1 -newoffset;
    }    
    
    adcs = adcsflip;
  }
}


unsigned int STTell1Board::offlineToDAQ(const STChannelID aOfflineChan,
                                        const unsigned int waferIndex,
                                        double isf) const
{
  // convert an offline channel to DAQ channel
  unsigned int strip = aOfflineChan.strip();
  if (m_orientation[waferIndex] == 0){
    // change the direction of numbering
    strip = m_nStripsPerHybrid - strip;
    // shift channel by one, because interstrip fraction cannot be negative
    if (isf > 0.01) --strip;
  }
  else {
    // subtract one because offline strips start at one.
    --strip;
  }

  return (waferIndex*m_nStripsPerHybrid) + strip;
}

std::ostream& STTell1Board::fillStream( std::ostream& os ) const{

  // print function
  os << " Board: " << m_boardID.region() << " " <<  m_boardID.subID() << std::endl;
  os << "# readout sectors " << m_sectorsVector.size() << std::endl;
  std::vector<STChannelID>::const_iterator iterW = m_sectorsVector.begin();
  for (; iterW !=  m_sectorsVector.end() ;++iterW){
    os << "Station: " << (*iterW).station() << "Layer: " << (*iterW).layer() 
       << "Region: " << (*iterW).detRegion() <<"Sector: " << (*iterW).sector() << std::endl;
  }   // iW 
  os << " -----------" << std::endl; 

  return os;

}

