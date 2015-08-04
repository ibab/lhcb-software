//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

#include "STDataSizeMonitor.h"

//Event
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

#include "Kernel/STTell1ID.h"
#include "Kernel/STRawBankMap.h"

#include "Kernel/ISTReadoutTool.h"
#include "AIDA/IProfile1D.h"

namespace ST {
  DECLARE_ALGORITHM_FACTORY( STDataSizeMonitor )
}
using namespace LHCb;

//--------------------------------------------------------------------
//
//  Check the data size of the ST raw banks per tell1
//
//--------------------------------------------------------------------

ST::STDataSizeMonitor::STDataSizeMonitor(const std::string& name, ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase(name, pSvcLocator)
  , m_bankType(RawBank::Velo)
  , m_nTELL1s(0)
  , m_prof_dataSizeVsTELL1(0)
 
{
  // constructer
}

ST::STDataSizeMonitor::~STDataSizeMonitor(){
  // destructer
}

StatusCode ST::STDataSizeMonitor::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  // configure banktype
  sc = configureBankType();
  if (sc.isFailure()){
    return Error("unknown bank type", sc);
  }

  // Get the tell1 mapping from source ID to tell1 number
  m_nTELL1s = readoutTool()->nBoard();
  m_prof_dataSizeVsTELL1 = bookProfile1D("Mean Raw Bank Size vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);

  return StatusCode::SUCCESS;
}

StatusCode ST::STDataSizeMonitor::execute(){

  // execute once per event
 
  // init counters  
//   unsigned int maxBoardSize = 0;
//   STTell1ID hotBoard(0);
//   unsigned int eventDataSize = 0; 
 
  // get banks and loop
  
  // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(RawEventLocation::Default);
  const std::vector<RawBank*>& tBanks = rawEvt->banks(m_bankType);
  std::vector<RawBank* >::const_iterator iterBank;
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // board info....  
    size_t bankSize = (*iterBank)->size()/sizeof(char);  
    unsigned int sourceID = (*iterBank)->sourceID();
//     STTell1ID aBoard(sourceID);
    unsigned int TELL1ID = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    m_prof_dataSizeVsTELL1->fill(TELL1ID, bankSize);

//     // event counters
//     if (bankSize > maxBoardSize){
//       maxBoardSize = bankSize;
//       hotBoard = aBoard;
//     }
//     eventDataSize += bankSize;

//     // histogram per board
//     plot((double)bankSize, "board data size", 0., 200., 200);
  
//     // data size per board
//     unsigned int id = (aBoard.region()*20) + aBoard.subID();
//     plot(id, "data size", 0., 100. , 100,(double)bankSize);

  } // iterBank

//   // data size
//   plot((double)eventDataSize,1, "event data size",0. , 10000., 500);

//   // include standard header HARDCODE !!!
//   unsigned int headerSize = tBanks.size()*2u;
//   plot((double)(eventDataSize+headerSize),2, "total data size",0.,10000., 500);  

//   plot((double)maxBoardSize,3, "hot board size", 0., 200., 200);
//   unsigned int id = (hotBoard.region()*20) + hotBoard.subID();
//   plot((double)id,4,"hot board ID", 0., 100., 100);

  return StatusCode::SUCCESS;
}


StatusCode ST::STDataSizeMonitor::configureBankType(){

  StatusCode sc = StatusCode::SUCCESS;

  m_bankType = STRawBankMap::stringToType(detType());
  if (m_bankType == RawBank::Velo){
    sc = StatusCode::FAILURE;
  }
  return sc;
}






