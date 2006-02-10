//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "STRawBankMonitor.h"

//Event
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

#include "STTell1ID.h"
#include "STDAQGeneral.h"


static const AlgFactory<STRawBankMonitor> s_Factory;
const IAlgFactory& STRawBankMonitorFactory = s_Factory;

using namespace LHCb;

//--------------------------------------------------------------------
//
//  ITMCtuner : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

STRawBankMonitor::STRawBankMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator)
 
{
  // constructer
  declareProperty("type", m_type ="TT");
}

STRawBankMonitor::~STRawBankMonitor(){
  // destructer
}

StatusCode STRawBankMonitor::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(m_type);
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  // configure banktype
  sc = configureBankType();
  if (sc.isFailure()){
    return Error("unknown bank type", sc);
  }

  return StatusCode::SUCCESS;
}

StatusCode STRawBankMonitor::execute(){

  // execute once per event
 
  // init counters  
  unsigned int maxBoardSize = 0;
  STTell1ID hotBoard(0);
  unsigned int eventDataSize = 0; 
 
  // get banks and loop
  
  // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(RawEventLocation::Default);
  const std::vector<RawBank*>& tBanks = rawEvt->banks(m_bankType);
  std::vector<RawBank* >::const_iterator iterBank;
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // board info....  
    size_t bankSize = (*iterBank)->size()/sizeof(char);  
    STTell1ID aBoard((*iterBank)->sourceID());

    // event counters
    if (bankSize > maxBoardSize){
      maxBoardSize = bankSize;
      hotBoard = aBoard;
    }
    eventDataSize += bankSize;

    // histogram per board
    plot((double)bankSize, "board data size", 0., 200., 200);
  
    // data size per board
    unsigned int id = (aBoard.region()*20) + aBoard.subID();
    plot(id, "data size", 0., 100. , 100,(double)bankSize);

  } // iterBank

  // data size
  plot((double)eventDataSize, "event data size",0. , 10000., 500);

  // include standard header HARDCODE !!!
  unsigned int headerSize = tBanks.size()*2u;
  plot((double)(eventDataSize+headerSize), "total data size",0.,10000., 500);  

  plot((double)maxBoardSize, "hot board size", 0., 200., 200);
  unsigned int id = (hotBoard.region()*20) + hotBoard.subID();
  plot((double)id,"hot board ID", 0., 100., 100);

  return StatusCode::SUCCESS;
}


StatusCode STRawBankMonitor::configureBankType(){

  StatusCode sc = StatusCode::SUCCESS;

  m_bankType = STDAQGeneral::stringToRawBankType(m_type);
  if (m_bankType == RawBank::Velo){
    sc = StatusCode::FAILURE;
  }
  return sc;
}






