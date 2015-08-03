// $Id: ITReadoutTool.cpp,v 1.15 2009-10-30 12:54:19 mtobin Exp $

// STDAQ
#include "ITReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STBoardMapping.h"

// IT
#include "Kernel/STChannelID.h"

// Det Desc
#include "DetDesc/Condition.h"

#include <algorithm>


using namespace LHCb;

DECLARE_TOOL_FACTORY( ITReadoutTool )

ITReadoutTool::ITReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : STReadoutTool( type, name, parent )
  , m_firstStation(512)
{
  // constructor

  declareProperty( "conditionLocation", 
                   m_conditionLocation  = "/dd/Conditions/ReadoutConf/IT/ReadoutMap" );

  // need a line here to get the interface correct !!!!
  declareInterface<ISTReadoutTool>(this);

  m_detType = "IT";

}

StatusCode ITReadoutTool::initialize() {

  // initialization phase...
  StatusCode sc = STReadoutTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  registerCondition(m_conditionLocation,
                    &ITReadoutTool::createBoards );
  sc = runUpdate();
  if (sc.isFailure()) return Error ( "Failed first UMS update for ITReadoutTool", sc );

  if (m_printMapping == true){
    printMapping();
  }
 
  return StatusCode::SUCCESS;
}

unsigned int ITReadoutTool::region(const STChannelID aChan) const{
  // convert channel to region
  return (aChan.station()-m_firstStation);
}

// Add the mapping of source ID to board number
const  std::map<unsigned int, unsigned int>& ITReadoutTool::SourceIDToTELLNumberMap() const {
  return STBoardMapping::ITSourceIDToNumberMap(); 
}

// Add the mapping of TELL1 board number to source ID
const  std::map<unsigned int, unsigned int>& ITReadoutTool::TELLNumberToSourceIDMap() const {
  return STBoardMapping::ITNumberToSourceIDMap();
}

StatusCode ITReadoutTool::createBoards() {

  clear();

  // load conditions
  Condition* rInfo = getDet<Condition>(m_conditionLocation);

  // vector of layer types
  const std::vector<std::string>& stations = 
    rInfo->param<std::vector<std::string> >("stations");

  m_hybridsPerBoard = rInfo->param<int>("hybridsPerBoard");
  unsigned int nBoardPerStation = rInfo->param<int>("nBoard");
  unsigned int nStripsPerHybrid =  STDAQ::nStripsPerBoard/m_hybridsPerBoard;
  m_nBoard = nBoardPerStation*stations.size();

  std::vector<std::string>::const_iterator iterS = stations.begin();
  unsigned int iReg = 0;

  for (; iterS != stations.end(); ++iterS, ++iReg){
   m_firstBoardInRegion.push_back(m_boards.size());
   
   const std::vector<int>& tMap = rInfo->param<std::vector<int> >(*iterS); 
   const std::vector<int>& orientation = rInfo->param<std::vector<int> >(*iterS+"HybridOrientation"); 
   const std::vector<std::string>& serviceBoxes = rInfo->param<std::vector<std::string> >(*iterS+"ServiceBox");

   unsigned int vecLoc = 0;
  
   if ( iterS == stations.begin()){
     STChannelID firstChan = STChannelID(tMap[0]);
     m_firstStation = firstChan.station() ;
   }
  
   for (unsigned int iBoard = 0; iBoard < nBoardPerStation; ++iBoard){
   
     // make new board
     STTell1ID anID = STTell1ID(iReg,iBoard);
     STTell1Board* aBoard = new STTell1Board(anID,nStripsPerHybrid, m_detType );

     for (unsigned iH = 0 ; iH < m_hybridsPerBoard; ++iH, ++vecLoc){
       STChannelID sectorID((unsigned int)tMap[vecLoc]);
       aBoard->addSector(sectorID, (unsigned int)orientation[vecLoc], serviceBoxes[vecLoc]);


       // add to the list of service boxs if not already there
       if (std::find(m_serviceBoxes.begin(), m_serviceBoxes.end(), 
                     serviceBoxes[vecLoc]) ==  m_serviceBoxes.end()) {
	 m_serviceBoxes.push_back(serviceBoxes[vecLoc]);
       }

     } // iH

     m_boards.push_back(aBoard);

   } // boards per region
  } // iterS

  
  // validate the mapping --> all sectors should go somewhere !
  StatusCode sc = validate();
  if (sc.isFailure() ){
    return Error("Failed to validate mapping",StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}


