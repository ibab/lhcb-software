// STDAQ
#include "UTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STBoardMapping.h"

// IT
#include "Kernel/STChannelID.h"

#include <algorithm>

// Det Desc
#include "DetDesc/Condition.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( UTReadoutTool )

UTReadoutTool::UTReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : STReadoutTool( type, name, parent)
  , m_nRegionA(512)
  , m_firstStation(512)
{
  // constructor

  declareProperty( "conditionLocation", 
                    m_conditionLocation  = "/dd/Conditions/ReadoutConf/UT/ReadoutMap");

  // need a line here to get the interface correct !!!!
  declareInterface<ISTReadoutTool>(this);

  m_detType = "UT";

}

StatusCode UTReadoutTool::initialize() {

  // initialization phase...
  StatusCode sc = STReadoutTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  registerCondition(m_conditionLocation,
                    &UTReadoutTool::createTell1Map);

  registerCondition(m_conditionLocation,
                    &UTReadoutTool::createBoards);

  sc = runUpdate(); // force update
  if (sc.isFailure()) return Error ( "Failed first UMS update for readout tool", sc );
  
  if (m_printMapping == true){
    printMapping();
  }
 
  return StatusCode::SUCCESS;
}

unsigned int UTReadoutTool::region(const STChannelID aChan) const{
  // convert channel to region
  return (aChan.station() == 1 ?  aChan.layer() - 1 : m_nRegionA + aChan.layer() - 1 );
}

// Add the mapping of source ID to TELL1 board number
const  std::map<unsigned int, unsigned int>& UTReadoutTool::SourceIDToTELLNumberMap() const {
  return STBoardMapping::UTSourceIDToNumberMap();
}

// Add the mapping of TELL1 board number to source ID
const  std::map<unsigned int, unsigned int>& UTReadoutTool::TELLNumberToSourceIDMap() const {
  return STBoardMapping::UTNumberToSourceIDMap();
}

StatusCode UTReadoutTool::createTell1Map() 
{
  Condition* rInfo = getDet<Condition>(m_conditionLocation);
  const std::vector<std::string> layers =  rInfo->param<std::vector<std::string> >("layers");

  STBoardMapping::ClearUTMap();
  
  unsigned int sourceIDBase = 0;
  for (unsigned int iReg = 0; iReg < layers.size(); ++iReg){
    std::string tell1Loc = layers[iReg]+"TELL1";
    if ( rInfo->exists(tell1Loc) ) {
      //      printf("Extracting TELL1 map from %s\n", tell1Loc.c_str());
      
      const std::vector<int>& tell1 = rInfo->param<std::vector<int> >(tell1Loc);
      for ( unsigned int i=0; i<tell1.size(); i++ ) {
        STBoardMapping::AddUTMapEntry(sourceIDBase+i, tell1.at(i));
      }
    }
    sourceIDBase += 64;
  }

  //  printf("%s\n", STBoardMapping::printUTMap().c_str());
  
  return StatusCode::SUCCESS;
}



StatusCode UTReadoutTool::createBoards() {

  bool isUT = true;
  clear();

  // load conditions
  Condition* rInfo = getDet<Condition>(m_conditionLocation);

  // vector of layer types
  // const std::vector<std::string>& layers = rInfo->paramAsStringVect("layers");
  const std::vector<std::string> layers =  rInfo->param<std::vector<std::string> >("layers");
  const std::vector<int>& nBoards = rInfo->paramAsIntVect("nBoardsPerLayer");

  m_hybridsPerBoard = rInfo->param<int>("hybridsPerBoard");
  m_nRegionA = rInfo->param<int>("nRegionsInUTa");
  unsigned int nStripsPerHybrid =  STDAQ::nStripsPerBoard/m_hybridsPerBoard;

  for (unsigned int iReg = 0; iReg < layers.size(); ++iReg){

   m_firstBoardInRegion.push_back(m_boards.size());
   m_nBoard += nBoards[iReg];   

   const std::vector<int>& tMap = rInfo->param<std::vector<int> >(layers[iReg]);
   std::string orLoc = layers[iReg]+"HybridOrientation";
   const std::vector<int>& orientation = rInfo->param<std::vector<int> >(orLoc);
   const std::vector<std::string>& serviceBoxes = rInfo->param<std::vector<std::string> >(layers[iReg]+"ServiceBox");


   unsigned int vecLoc = 0;
   if ( iReg == 0){
     STChannelID firstChan = STChannelID(tMap[0]);
     m_firstStation = firstChan.station() ;
   }

   for (unsigned int iBoard = 0; iBoard < (unsigned int)nBoards[iReg]; ++iBoard){
   
     // make new board
     STTell1ID anID = STTell1ID(iReg,iBoard, isUT);
     STTell1Board* aBoard = new STTell1Board(anID, nStripsPerHybrid, m_detType);

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


