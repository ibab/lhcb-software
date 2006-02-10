// $Id: STReadoutTool.cpp,v 1.1 2006-02-10 08:59:31 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"


// STDAQ
#include "STReadoutTool.h"

#include "STTell1Board.h"
#include "STTell1ID.h"
#include "STDAQDefinitions.h"

// IT
#include "Kernel/STChannelID.h"

using namespace LHCb;

STReadoutTool::STReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent):
  GaudiTool( type, name, parent ),
  m_nBoard(0)
{
  // constructer
  declareProperty("printMapping", m_printMapping = false);

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

  return StatusCode::SUCCESS;
}


unsigned int STReadoutTool::nBoard() const {
  // number of boards
  return m_nBoard;
}

STDAQ::chanPair STReadoutTool::offlineChanToDAQ(const STChannelID aOfflineChan) const{

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
    return (std::make_pair(m_boards[iBoard]->boardID(),m_boards[iBoard]->offlineToDAQ(aOfflineChan,waferIndex))); 
  }
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
  std::cout << "print mapping for: " << name() << std::endl;  
  std::cout << " Number of boards " << m_nBoard << std::endl;
  std::vector<STTell1Board*>::const_iterator iterBoard = m_boards.begin();  
  for (; iterBoard != m_boards.end() ;++iterBoard){
    std::cout << **iterBoard << std::endl;
  } // print loop
  std::cout << " ----------------" << std::endl; 
 
}
