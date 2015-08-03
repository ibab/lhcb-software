// $Id: STReadoutTool.cpp,v 1.15 2009-10-30 12:54:19 mtobin Exp $

// STDAQ
#include "STReadoutTool.h"

#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STDAQDefinitions.h"
#include "Event/STCluster.h"

// IT
#include "Kernel/STChannelID.h"
#include "STDet/DeSTDetector.h"


// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// STKernel
#include "Kernel/STXMLUtils.h"

/** @file DeSTSector.cpp
*
*  Implementation of class :  DeSTSector
*
*    @author Matthew Needham
*/

using namespace boost::lambda;
using namespace LHCb;


STReadoutTool::STReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent):
  GaudiTool( type, name, parent ),
  m_nBoard(0),
  m_tracker(NULL)
{
  // constructer
  declareProperty("printMapping", m_printMapping = false );
  declareProperty("writeMappingToXML", m_writeXML = false );
  declareProperty("footer", m_footer = "</DDDB>");
  declareProperty("startTag", m_startTag = "<condition");
  declareProperty("outputFile",m_outputFileName = "ReadoutMap.xml");
  declareProperty("depths", m_depth = 3u );
  declareProperty("precision", m_precision = 16u);
  declareProperty("removeCondb", m_removeCondb = false);
  declareProperty("author", m_author = "Joe Bloggs");
  declareProperty("tag", m_tag = "None");
  declareProperty("description", m_desc = "BlahBlahBlah"); 

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

StatusCode STReadoutTool::finalize()  {

  if (m_writeXML){
    writeMappingToXML();
  }

  return GaudiTool::finalize();
}

StatusCode STReadoutTool::writeMappingToXML() const {

  // load conditions
  Condition* rInfo = getDet<Condition>(m_conditionLocation);

  std::ofstream outputFile(m_outputFileName.c_str());
  if (outputFile.fail() ){
    return Warning("Failed to open output file",StatusCode::FAILURE);
  }

  // write the xml headers 
  outputFile << header(rInfo->toXml("", true, m_precision))<< std::endl;

  // add comments
  std::ostringstream comment;
  ST::XMLUtils::fullComment(comment, m_author, m_tag, m_desc);
  outputFile << comment.str() << std::endl; 

  std::string temp = strip(rInfo->toXml("", false, m_precision));
  outputFile << temp << "\n"  << std::endl; 

  // footer 
  outputFile << footer() << std::endl;

  return StatusCode::SUCCESS;
}

unsigned int STReadoutTool::nBoard() const {
  // number of boards
  return m_nBoard;
}

unsigned int STReadoutTool::nServiceBox() const{
  return m_serviceBoxes.size();
}

std::string STReadoutTool::serviceBox(const LHCb::STChannelID& aChan) const{

  // find the board

  static const std::string InValidBox = "Unknown";
  bool isFound = false;
  unsigned int waferIndex = 999u; 
  unsigned int iBoard = m_firstBoardInRegion[region(aChan)];
  while ((iBoard != m_nBoard)&&(isFound == false)){
     if (m_boards[iBoard]->isInside(aChan,waferIndex)) {
      isFound = true;
    }
    else {
      ++iBoard;
    }
  } // iBoard
  return(  isFound ? m_boards[iBoard]->serviceBoxes()[waferIndex] : InValidBox);
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
    return(std::make_pair(STTell1ID(STTell1ID::nullBoard, false),0));
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
  std::vector<STTell1Board*>::const_iterator iter = 
    std::find_if( m_boards.begin(), m_boards.end(), 
    bind(&STTell1Board::sameID, _1, aBoardID));
  return (iter != m_boards.end() ? *iter: 0);
  //  return findByOrder( m_firstBoardInRegion[aBoardID.region()] + aBoardID.subID());
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
}

/// Add the mapping of source ID to TELL1 board number
unsigned int STReadoutTool::SourceIDToTELLNumber(unsigned int sourceID) const {
  return (this->SourceIDToTELLNumberMap().find(sourceID))->second;
}

/// Add the mapping of TELL1 board number to source ID
unsigned int STReadoutTool::TELLNumberToSourceID(unsigned int TELL) const {
  return (this->TELLNumberToSourceIDMap().find(TELL))->second;
}

StatusCode STReadoutTool::validate() const{

  // validate the map - every sector must go somewhere !
  bool valid = true;
  const DeSTDetector::Sectors& dSectors = m_tracker->sectors(); 
  DeSTDetector::Sectors::const_iterator iter = dSectors.begin();
  for (; iter != dSectors.end() && valid; ++iter) {
    STChannelID chan = (*iter)->elementID();
    STDAQ::chanPair chanPair = offlineChanToDAQ(chan,0.0);
    if (chanPair.first == STTell1ID(STTell1ID::nullBoard, false)) valid = false; 
  } // iter 
  
  return valid;
}

std::vector<LHCb::STChannelID> STReadoutTool::sectorIDs(const STTell1ID board) const{

  std::vector<LHCb::STChannelID> sectors; sectors.reserve(8);
  STTell1Board* theBoard = findByBoardID(board);  
  if (theBoard){
    sectors.insert(sectors.begin(), theBoard->sectorIDs().begin(), theBoard->sectorIDs().end());
  }
  else {
    Error("Failed to find Board", StatusCode::SUCCESS, 100);
  }
  return sectors;
}
  
std::vector<DeSTSector*> STReadoutTool::sectors(const STTell1ID board) const{ 

  std::vector<LHCb::STChannelID> sectors = sectorIDs(board);
  return m_tracker->findSectors(sectors);
}

std::vector<DeSTSector*> STReadoutTool::sectorsOnServiceBox(const std::string& serviceBox) const{ 

  std::vector<LHCb::STChannelID> sectors = sectorIDsOnServiceBox(serviceBox);
  return m_tracker->findSectors(sectors);
}

std::vector<LHCb::STChannelID> STReadoutTool::sectorIDsOnServiceBox(const std::string& serviceBox) const{
  // loop over all boards
  std::vector<LHCb::STChannelID> sectors; sectors.reserve(16);
  std::vector<STTell1Board*>::const_iterator iterB = m_boards.begin();
  for (; iterB != m_boards.end(); ++iterB){
    const STTell1Board* board = *iterB;
    const std::vector<LHCb::STChannelID>& sectorVec = board->sectorIDs();
    const std::vector<std::string>& sBoxes = board->serviceBoxes();
    for (unsigned int iS = 0u ; iS < board->nSectors(); ++iS){
      if (sBoxes[iS] == serviceBox) sectors.push_back(sectorVec[iS]);
    } // iS
  } // iterB  
  return sectors;
}

const std::vector<std::string>& STReadoutTool::serviceBoxes() const{
  return m_serviceBoxes;
}

std::string STReadoutTool::footer() const
{
  std::string temp = m_footer;
  temp.insert(0, "</catalog>" );
  return temp;
} 

std::string STReadoutTool::header(const std::string& conString) const
{
  // get the header
  std::string::size_type startpos = conString.find(m_startTag);
  std::string temp = conString.substr(0,startpos);
  temp.insert(startpos, "<catalog name=\"ReadoutSectors\">" );

  // correct the location of the DTD
  if( m_removeCondb ) {
    ST::XMLUtils::replace(temp,"conddb:", "");
    std::string location;
    for (unsigned int i = 0;  i< m_depth; ++i) location += "../";
    std::string::size_type pos = temp.find("/DTD/");
    temp.insert(pos,location);
    ST::XMLUtils::replace(temp,"//", "/");
  }

  return temp;
} 

std::string STReadoutTool::strip(const std::string& conString) const
{
  std::string::size_type startpos = conString.find(m_startTag);
  std::string::size_type endpos = conString.find(m_footer);
  return conString.substr(startpos, endpos - startpos);
} 





