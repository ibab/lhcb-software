// $Id: STTell1Board.cpp,v 1.15 2010-04-14 11:14:17 mneedham Exp $
#include "Kernel/STTell1Board.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/LHCbConstants.h"

#include "Event/STCluster.h"
#include "Kernel/ITNames.h"
#include "Kernel/TTNames.h"
#include "Kernel/UTNames.h"

using namespace LHCb;

STTell1Board::STTell1Board(const STTell1ID aBoard, 
                           const unsigned int nStripsPerHybrid,
                           const std::string& detType):
  m_boardID(aBoard),
  m_nStripsPerHybrid(nStripsPerHybrid),
  m_detType(detType)
{
  // constructer
  m_sectorsVector.reserve(8);
}

STTell1Board::~STTell1Board(){
  // destructer
}

void STTell1Board::addSector(STChannelID aOfflineChan, unsigned int orientation, 
                             const std::string& serviceBox){
 // add sector to vector
 m_orientation.push_back(orientation);
 m_sectorsVector.push_back(aOfflineChan);
 m_serviceBoxVector.push_back(serviceBox);
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


STTell1Board::chanPair STTell1Board::DAQToOffline(const unsigned int fracStrip,
                                                  const STDAQ::version& version,
                               const STDAQ::StripRepresentation aDAQChan) const{

  // convert a DAQ channel to offline !
  const unsigned int index = aDAQChan.value()/m_nStripsPerHybrid;
  unsigned int strip =  aDAQChan.value() - (index*m_nStripsPerHybrid);

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

  
  // hack for TED run 
  /*
  STChannelID chan = STChannelID(STChannelID::typeIT, ITNames::IT3, 
                                 ITNames::X1, ITNames::Top, 1, 0);
  
  if (m_sectorsVector[index] == chan){
    if (strip <= 128){
      strip = strip + 128;
    }
    else if(strip >128 && strip <= 256){
      strip = strip - 128;
    } 
    else {
      // nothing
    } 
  }  
  
  */
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
                                const unsigned int fracStrip) const
{
  const unsigned int index = aDAQChan/m_nStripsPerHybrid;
  const unsigned int size = adcs.size();
  int newoffset = 0;

  // flip sequence of adc vector
  if (m_orientation[index] == 0 && version >= STDAQ::v4){
    // calculate the new offset
    newoffset = size - 1 - offset;
    // Correct for interstrip fraction when not equal to zero
    if( fracStrip != 0u ) {
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


std::string STTell1Board::serviceBox(const STDAQ::StripRepresentation& tell1Chan) const{

  const unsigned int index = tell1Chan.value()/m_nStripsPerHybrid;
  return serviceBox(index);
}


std::ostream& STTell1Board::fillStream( std::ostream& os ) const{

  // print function
  os << m_detType + "Tell1: " << flatTell1Number() << std::endl;
  os << " Source ID Board: "  <<m_boardID.id() << " region " << m_boardID.region() 
     << " subID" <<  m_boardID.subID() << std::endl;
  os << "# readout sectors " << m_sectorsVector.size() << std::endl;
  std::vector<STChannelID>::const_iterator iterW = m_sectorsVector.begin();
  unsigned int wafer = 0u;
  for (; iterW !=  m_sectorsVector.end() ;++iterW, ++wafer){
   
    if (*iterW == LHCb::STChannelID(0) ){
      // link is not loaded
      os << "Unloaded Link" << std::endl;
      continue;
    }

    if (m_detType == "IT"){    
      os  << ITNames().UniqueSectorToString(*iterW)  << " "   << serviceBox(wafer) << std::endl;
    }
    else if (m_detType == "TT") {
      os << TTNames().UniqueSectorToString(*iterW) << " " << serviceBox(wafer) << std::endl;   
    }
    else {
      os << UTNames().UniqueSectorToString(*iterW) << " " << serviceBox(wafer) << std::endl;   
    }
  }   // iW 
  os << " -----------" << std::endl; 

  return os;

}

#include "Kernel/STBoardMapping.h"

unsigned int STTell1Board::flatTell1Number() const{
  if ( m_detType == "TT" )
    return STBoardMapping::find(boardID().id(), STBoardMapping::TTSourceIDToNumberMap());   
  else if ( m_detType == "IT" )
    return STBoardMapping::find(boardID().id(), STBoardMapping::ITSourceIDToNumberMap());
  else
    return STBoardMapping::find(boardID().id(), STBoardMapping::UTSourceIDToNumberMap());
}
