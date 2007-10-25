// $Id: TTReadoutTool.cpp,v 1.6 2007-10-25 10:30:07 cattanem Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// STDAQ
#include "TTReadoutTool.h"
#include "STTell1Board.h"
#include "STTell1ID.h"

// IT
#include "Kernel/STChannelID.h"

// Det Desc
#include "DetDesc/Condition.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( TTReadoutTool );

TTReadoutTool::TTReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent):
  STReadoutTool( type, name, parent )
{
  // constructer

  declareProperty( "conditionLocation", 
                    m_conditionLocation  = "/dd/Conditions/ReadoutConf/TT/ReadoutMap");

  // need a line here to get the interface correct !!!!
  declareInterface<ISTReadoutTool>(this);

}

StatusCode TTReadoutTool::initialize() {

  // initialization phase...
  StatusCode sc = STReadoutTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

   // Update Manager
  IUpdateManagerSvc * ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);

  ums->registerCondition( this, m_conditionLocation,
                                &TTReadoutTool::createBoards );

  // force first updates
  sc = ums->update(this);
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );
  

  if (m_printMapping == true){
    printMapping();
  }
 
  return StatusCode::SUCCESS;
}

unsigned int TTReadoutTool::region(const STChannelID aChan) const{
  // convert channel to region
  return (aChan.station() == 1 ?  aChan.layer() - 1 : m_nRegionA + aChan.layer() - 1 );
}

StatusCode TTReadoutTool::createBoards() {

  clear();

  // load conditions
  Condition* rInfo = getDet<Condition>(m_conditionLocation);

  // vector of layer types
  // const std::vector<std::string>& layers = rInfo->paramAsStringVect("layers");
  const std::vector<std::string> layers =  rInfo->param<std::vector<std::string> >("layers");
  const std::vector<int>& nBoards = rInfo->paramAsIntVect("nBoardsPerLayer");
  m_hybridsPerBoard = rInfo->param<int>("hybridsPerBoard");
  m_nRegionA = rInfo->param<int>("nRegionsInTTa");
  unsigned int nStripsPerHybrid =  STDAQ::nStripsPerBoard/m_hybridsPerBoard;

  for (unsigned int iReg = 0; iReg < layers.size(); ++iReg){

   m_firstBoardInRegion.push_back(m_boards.size());
   m_nBoard += nBoards[iReg];   

   const std::vector<int>& tMap = rInfo->param<std::vector<int> >(layers[iReg]);
   std::string orLoc = layers[iReg]+"HybridOrientation";
   const std::vector<int>& orientation = rInfo->param<std::vector<int> >(orLoc);
   unsigned int vecLoc = 0;
   if ( iReg == 0){
     STChannelID firstChan = STChannelID(tMap[0]);
     m_firstStation = firstChan.station() ;
   }

   for (unsigned int iBoard = 0; iBoard < (unsigned int)nBoards[iReg]; ++iBoard){
   
     // make new board
     STTell1ID anID = STTell1ID(iReg,iBoard);
     STTell1Board* aBoard = new STTell1Board(anID, nStripsPerHybrid);

     for (unsigned iH = 0 ; iH < m_hybridsPerBoard; ++iH, ++vecLoc){
       STChannelID sectorID((unsigned int)tMap[vecLoc]);
       aBoard->addSector(sectorID, (unsigned int)orientation[vecLoc]);
     } // iH

     m_boards.push_back(aBoard);

   } // boards per region
  } // iterS

  return StatusCode::SUCCESS;
}


