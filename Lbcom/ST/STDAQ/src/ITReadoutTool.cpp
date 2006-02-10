// $Id: ITReadoutTool.cpp,v 1.1 2006-02-10 08:59:31 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// STDAQ
#include "ITReadoutTool.h"
#include "STTell1Board.h"
#include "STTell1ID.h"

// IT
#include "Kernel/STChannelID.h"

// Det Desc
#include "DetDesc/Condition.h"

using namespace LHCb;

static const ToolFactory<ITReadoutTool>  s_factory;
const IToolFactory& ITReadoutToolFactory = s_factory;

ITReadoutTool::ITReadoutTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent):
  STReadoutTool( type, name, parent )
{
  // constructer

  declareProperty( "conditionLocation", 
                   m_conditionLocation  = "/dd/Conditions/ReadoutConf/IT/ReadoutMap" );

  // need a line here to get the interface correct !!!!
  declareInterface<ISTReadoutTool>(this);

}

StatusCode ITReadoutTool::initialize() {

  // initialization phase...
  StatusCode sc = STReadoutTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

   // Update Manager
  IUpdateManagerSvc * ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);

  ums->registerCondition( this, m_conditionLocation,
                                &ITReadoutTool::createBoards );

  // force first updates
  sc = ums->update(this);
  ums->release();
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );

  if (m_printMapping == true){
    printMapping();
  }
 
  return StatusCode::SUCCESS;
}

unsigned int ITReadoutTool::region(const STChannelID aChan) const{
  // convert channel to region
  return (aChan.station()-m_firstStation);
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
   unsigned int vecLoc = 0;
  
   if ( iterS == stations.begin()){
     STChannelID firstChan = STChannelID(tMap[0]);
     m_firstStation = firstChan.station() ;
   }
  
   for (unsigned int iBoard = 0; iBoard < nBoardPerStation; ++iBoard){
   
     // make new board
     STTell1ID anID = STTell1ID(iReg,iBoard);
     STTell1Board* aBoard = new STTell1Board(anID,nStripsPerHybrid );

     for (unsigned iH = 0 ; iH < m_hybridsPerBoard; ++iH, ++vecLoc){
       STChannelID sectorID((unsigned int)tMap[vecLoc]);
       aBoard->addSector(sectorID);
     } // iH

     m_boards.push_back(aBoard);

   } // boards per region
  } // iterS
  return StatusCode::SUCCESS;
}


