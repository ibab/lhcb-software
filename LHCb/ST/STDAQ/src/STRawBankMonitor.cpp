//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

#include "STRawBankMonitor.h"

//Event
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

#include "Kernel/STTell1ID.h"
#include "Kernel/STRawBankMap.h"
#include "Kernel/ISTReadoutTool.h"

DECLARE_ALGORITHM_FACTORY( STRawBankMonitor )

using namespace LHCb;

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------

STRawBankMonitor::STRawBankMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase(name, pSvcLocator)
  , m_bankType(LHCb::RawBank::LastType)
 
{
  // constructor
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");
}

STRawBankMonitor::~STRawBankMonitor(){
  // destructor
}

StatusCode STRawBankMonitor::initialize(){

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

  // Initialise the RawEvent locations
  bool usingDefaultLocation = m_rawEventLocations.empty();
  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode STRawBankMonitor::execute(){

  // execute once per event
 
  // init counters  
  unsigned int maxBoardSize = 0;
  STTell1ID hotBoard(0, detType()=="UT");
  unsigned int eventDataSize = 0; 
 
  // get banks and loop
  
  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvt = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    if (exist<LHCb::RawEvent>(*p)){
      rawEvt = get<LHCb::RawEvent>(*p);
      break;
    }
  }
  if( rawEvt == NULL ) return Error("Failed to find raw data");

  const std::vector<RawBank*>& tBanks = rawEvt->banks(m_bankType);
  std::vector<RawBank* >::const_iterator iterBank;
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // board info....  
    size_t bankSize = (*iterBank)->size()/sizeof(char);  
    STTell1ID aBoard((*iterBank)->sourceID(), detType()=="UT");

    // event counters
    if (bankSize > maxBoardSize){
      maxBoardSize = bankSize;
      hotBoard = aBoard;
    }
    eventDataSize += bankSize;

    // histogram per board
    plot((double)bankSize, "board data size", 0., 200., 200);
  
    // data size per board
    //unsigned int id = (aBoard.region()*20) + aBoard.subID();
    //const std::map< unsigned int, unsigned int > & SourceIDToTELLmap = readoutTool()->SourceIDToTELLNumberMap();
    //unsigned int tellNumber = SourceIDToTELLmap.find((*iterBank)->sourceID())->second;
    unsigned int tellNumber = readoutTool()->SourceIDToTELLNumber( (*iterBank)->sourceID() );
  
    // These hard coded numbers come from here: https://lbtwiki.cern.ch/bin/view/Online/Tell1PortNum
    unsigned int doubleLinkedTTtell1s[] = {1, 2, 3, 4, 5,  6,  8,  9,  10, 13, 14, 15};
    unsigned int doubleLinkedITtell1s[] = {1, 2, 8, 9, 15, 16, 22, 23, 29, 30, 36, 37};
    unsigned int numberOfLinks = 1;
    
    if ( detType() == "IT" ) {
	for ( unsigned int i = 0; i < 12; i++) {
		if (tellNumber == doubleLinkedITtell1s[i]) numberOfLinks = 2;
	}
    }
    else if ( detType() == "TT" ) {
	for ( unsigned int i = 0; i < 12; i++) {
		if (tellNumber == doubleLinkedTTtell1s[i]) numberOfLinks = 2;
	}
    }

    double datasize = bankSize/(double)numberOfLinks;
    plot(tellNumber, "data size", 0., 100. , 100, datasize);
    plot(tellNumber, "data size unnormalised", 0., 100. , 100, (double)bankSize);

  } // iterBank

  // data size
  plot((double)eventDataSize,1, "event data size",0. , 10000., 500);

  // include standard header HARDCODE !!!
  unsigned int headerSize = tBanks.size()*2u;
  plot((double)(eventDataSize+headerSize),2, "total data size",0.,10000., 500);  

  plot((double)maxBoardSize,3, "hot board size", 0., 200., 200);
  unsigned int id = (hotBoard.region()*20) + hotBoard.subID();
  plot((double)id,4,"hot board ID", 0., 100., 100);

  return StatusCode::SUCCESS;
}


StatusCode STRawBankMonitor::configureBankType(){

  StatusCode sc = StatusCode::SUCCESS;

  m_bankType = STRawBankMap::stringToType(detType());
  if (m_bankType == RawBank::Velo){
    sc = StatusCode::FAILURE;
  }
  return sc;
}






