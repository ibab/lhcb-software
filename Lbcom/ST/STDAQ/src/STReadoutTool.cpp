// $Id: STReadoutTool.cpp,v 1.8 2008-07-01 10:10:06 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"


// STDAQ
#include "STReadoutTool.h"

#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STDAQDefinitions.h"
#include "Event/STCluster.h"

// IT
#include "Kernel/STChannelID.h"

#include "STDet/DeSTDetector.h"

using namespace LHCb;

STReadoutTool::STReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent):
  GaudiTool( type, name, parent ),
  m_nBoard(0)
{
  // constructer
  declareProperty("printMapping", m_printMapping = false );

  m_boards.reserve(100); // about correct
}

STReadoutTool::~STReadoutTool() {
  /// destructer
  clear();
}

void STReadoutTool::clear() {
 
  // clear the boards
  if (m_boards.size() != 0) {
     std::vector<STTell1Board*>::iterator iterBoard = m_boards.begin();
     while (iterBoard != m_boards.end()) {
       STTell1Board* aBoard = *iterBoard;
       delete aBoard;
       m_boards.erase(iterBoard);
     } // while
  } // if

  m_nBoard = 0;
}

StatusCode STReadoutTool::initialize() {

  // initialization phase...
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // tracker
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  return StatusCode::SUCCESS;
}


unsigned int STReadoutTool::nBoard() const {
  // number of boards
  return m_nBoard;
}

std::vector<STTell1ID> STReadoutTool::boardIDs() const{
  std::vector<STTell1Board*>::const_iterator iterBoard = m_boards.begin();
  std::vector<STTell1ID> ids; ids.reserve(m_boards.size());
  for (;iterBoard != m_boards.end(); ++iterBoard){
    ids.push_back((*iterBoard)->boardID());
  } //iterBoard
  return ids;
}

STDAQ::chanPair STReadoutTool::offlineChanToDAQ(const STChannelID aOfflineChan,
                                                double isf) const
{
  // look up region start.....
  unsigned int iBoard = m_firstBoardInRegion[region(aOfflineChan)];
  bool isFound = false;
  unsigned int waferIndex = 999u; 

  while ((iBoard != m_nBoard)&&(isFound == false)){
     if (m_boards[iBoard]->isInside(aOfflineChan,waferIndex)) {
      isFound = true;
    }
    else {
      ++iBoard;
    }
  } // iBoard

  if (isFound == false){
    return(std::make_pair(STTell1ID(STTell1ID::nullBoard),0));
  }
  else {
    return (std::make_pair(m_boards[iBoard]->boardID(),
                           m_boards[iBoard]->offlineToDAQ(aOfflineChan,
                                                          waferIndex,isf))); 
  }
}


double STReadoutTool::interStripToDAQ(const STChannelID aOfflineChan,
                                      const STTell1ID aBoardID,
                                      const double isf) const
{
  unsigned int waferIndex = 999u;

  STTell1Board* aBoard = this->findByBoardID(aBoardID);
  double newisf = 0;
  
  if(aBoard->isInside(aOfflineChan,waferIndex)){
    unsigned int orientation = aBoard->orientation()[waferIndex];
    if(orientation == 0 && isf > 0.01){
      newisf = 1 - isf;
    } else {
      newisf = isf;
    }
  } else { // Can not find board!
    newisf = -1;
  }  
  
  return newisf;
}


bool STReadoutTool::ADCOfflineToDAQ(const STChannelID aOfflineChan,
                                    const STTell1ID aBoardID,
                                    STCluster::ADCVector& adcs) const
{
  unsigned int waferIndex = 999u;
  STTell1Board* aBoard = this->findByBoardID(aBoardID);
    
  if( aBoard->isInside(aOfflineChan,waferIndex) ) {
    unsigned int orientation = aBoard->orientation()[waferIndex];
    if( orientation == 0 ){
      STCluster::ADCVector adcsflip = adcs;
      for (unsigned int i = 0; i < adcs.size(); ++i) {
        adcsflip[adcs.size()-1-i] = adcs[i];
      }
      adcs = adcsflip;
    }
  }
  else{ // can not find board!
    return false;
  }  
  
  return true;
}


STTell1Board* STReadoutTool::findByBoardID(const STTell1ID aBoardID) const{
  // find by board id
  return findByOrder( m_firstBoardInRegion[aBoardID.region()] + aBoardID.subID());
}

STTell1Board* STReadoutTool::findByOrder(const unsigned int aValue) const{
  // find by order
  return ( aValue< m_nBoard ? m_boards[aValue] : 0);
}

void STReadoutTool::printMapping() const{

  // dump out the readout mapping
  std::cout << "print mapping for: " << name() << " tool" << std::endl;  
  std::cout << " Number of boards " << m_nBoard << std::endl;
  std::vector<STTell1Board*>::const_iterator iterBoard = m_boards.begin();  
  for (; iterBoard != m_boards.end() ;++iterBoard){
    std::cout << **iterBoard << std::endl;
  } // print loop
  std::cout << " ----------------" << std::endl; 
 
}


StatusCode STReadoutTool::validate() const{

  // validate the map - every sector must go somewhere !
  bool valid = true;
  const DeSTDetector::Sectors& dSectors = m_tracker->sectors(); 
  DeSTDetector::Sectors::const_iterator iter = dSectors.begin();
  for (; iter != dSectors.end() && valid; ++iter) {
    STChannelID chan = (*iter)->elementID();
    STDAQ::chanPair chanPair = offlineChanToDAQ(chan,0.0);
    if (chanPair.first == STTell1ID(STTell1ID::nullBoard)) valid = false; 
  } // iter 
  
  return valid;
}
