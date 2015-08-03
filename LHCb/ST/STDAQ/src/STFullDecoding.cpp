// Include files
// Event data
#include "Event/RawEvent.h"
#include "Event/STTELL1Data.h"

// From Kernel packages
#include "Kernel/STRawBankMap.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/ISTReadoutTool.h"

// standard packages
#include <bitset>
#include "boost/lexical_cast.hpp"

// local
#include "STFullDecoding.h"

using namespace LHCb;
using namespace STDAQ;

//-----------------------------------------------------------------------------
// Implementation file for class : STFullDecoding
//
// 2007-09-11: Mathias Knecht, Jeroen van Tilburg
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STFullDecoding )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STFullDecoding::STFullDecoding( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : ST::AlgBase ( name , pSvcLocator )
{
  declareProperty("PrintErrorInfo", m_printErrorInfo = true );     
  declareSTConfigProperty("InputLocation", m_inputLocation , 
                          RawEventLocation::Default );
  declareSTConfigProperty("OutputLocation", m_outputLocation,
                          STTELL1DataLocation::TTFull );
  declareSTConfigProperty("EventInfoLocation" , m_eventInfoLocation , 
                          STTELL1EventInfoLocation::TTEventInfo );
  
  setForcedInit();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode STFullDecoding::initialize() {
  StatusCode sc = ST::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // initialize bank type
  m_bankType = STRawBankMap::stringToType(detType()+"Full");
           
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STFullDecoding::execute()
{
  // Get the raw data
  if (!exist<RawEvent>(m_inputLocation)){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }
  RawEvent* raw = get<RawEvent>( m_inputLocation );

  // make container of TELL1 boards
  STTELL1Datas* outputData = new STTELL1Datas();
 
  // put the object on the transient event store
  put(outputData, m_outputLocation);

  // make a container of eventInfo
  STTELL1EventInfos* eventInfos = new STTELL1EventInfos(); 
  put(eventInfos,m_eventInfoLocation);
 
  // Initialize some counters
  unsigned int L0EvtID = 0;
 
  // Pick up IT/TTFull bank 
  const std::vector<RawBank*>& itf = raw->banks(RawBank::BankType(m_bankType));
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Starting to decode " << itf.size() << detType()
            << "Full bank(s)" << endmsg;
  }
  
  std::vector<RawBank*>::const_iterator itB;
  for( itB = itf.begin(); itB != itf.end(); ++itB ) {

    // Create an empty tell1 data object 
    STTELL1Data::Data tell1Data;
    tell1Data.resize(noptlinks);
    for( STTELL1Data::Data::iterator i =tell1Data.begin();
                                     i!=tell1Data.end(); ++i) {  
      i->resize(nports*nstrips, 0);
    }
 
    // Create an empty tell1 header object 
    STTELL1Data::Data tell1Header;
    tell1Header.resize(noptlinks);
    std::vector< std::vector <int> >::iterator j=tell1Header.begin();
    for( ; j!=tell1Header.end(); ++j) {
      j->resize(nports*nheaders, 0);
    }

    // Create an empty eventInfo object
    STTELL1Data::Info eventInfo;
    for (unsigned int i = 0 ; i < npp ; ++i){
      STTELL1EventInfo* evtInfo = new STTELL1EventInfo();
      eventInfo.push_back(evtInfo);
      eventInfos->insert(evtInfo);
    } 
    std::vector<unsigned int> sentPP;

    const RawBank* p = *itB;
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Decoding bank number [" << itB-itf.begin() << "] of type " 
              << detType() << "Full (TELL1 ID: " <<  p->sourceID() 
              << ", Size: " <<  p->size() << " bytes)" << endmsg;
    }
    
    // Check if the board is valid
    STTell1ID tell1ID = STTell1ID((unsigned int)p->sourceID(), detType()=="UT"); 
    const STTell1Board* aBoard =  readoutTool()->findByBoardID(tell1ID);
    if ( !aBoard ) { // not a valid b
      std::string invalidSource = "Invalid source ID --> skip bank "+
        boost::lexical_cast<std::string>(p->sourceID());  
      Warning(invalidSource,StatusCode::SUCCESS,2).ignore(); 
      ++counter("Skipped banks");
      continue;
    }

    if( (unsigned int)p->size() % nwordsFull != 0 ){
      error() << "Wrong bank size for this type!! You should have multiple of " 
              << nwordsFull << " bytes" << endmsg;
    }
    
    // Counters
    unsigned int cntWD = 0; // Word counter, resets for each PP. Range 0 to 223.
    unsigned int cntPP = 0; // PP-FPGA counter, goes from 0 to 3.
   
    // Now loop over all WORDS in a bank
    const unsigned int* w = p->begin<unsigned int>();
    for(; w != p->end<unsigned int>(); ++w )  {    

      if(cntWD % 224 == 0){	// Each 224 words we have a new PP-FPGA        
        cntWD = 0;
        cntPP = (*(w + 219) & STTELL1EventInfo::ChipAddrMask) 
          >> STTELL1EventInfo::ChipAddrBits;
        sentPP.push_back(cntPP);

        if (msgLevel(MSG::DEBUG)) {
          debug() << "#######  Parsing now data from PP " << cntPP 
                  << " ##################" << endmsg; 
        }
      }

      // Set the Tell1 sourceID for each PP
      STTELL1EventInfo* evtInfo = eventInfo[cntPP];
      evtInfo->setSourceID(p->sourceID()); 			   

      // Unpack the 32-bit word into 8-bit chunks
      unsigned int bits1 = 0;
      unsigned int bits2 = 8;
      unsigned int bits3 = 16;
      unsigned int bits4 = 24;
      unsigned int p1 = ( *w & mask1) >> bits1;
      unsigned int p2 = ( *w & mask2) >> bits2;
      unsigned int p3 = ( *w & mask3) >> bits3;
      unsigned int p4 = ( *w & mask4) >> bits4;

      if( cntWD < 216 ) { // Words below 216 contain data and header
        int iPort = cntWD/(nbeetles*36);
        int iWord = (cntWD%(nbeetles*36))/nbeetles;
        int iBeetle = 2*(cntWD%nbeetles) + nBeetlesPerPPx*cntPP;
        
        if ( iWord >= 1 && iWord < 4 ) { // Header part
          int iHeader = iWord-1;
          tell1Header[iBeetle][iHeader+3*iPort] = p1;
          tell1Header[iBeetle][iHeader+3*(iPort+2)] = p2;
          tell1Header[iBeetle+1][iHeader+3*iPort] = p3;
          tell1Header[iBeetle+1][iHeader+3*(iPort+2)] = p4;
        } 
        else if ( iWord >= 4 && iWord < 36 ) { // Data part
          int iChan = iWord-4;
          tell1Data[iBeetle][iChan+32*iPort] = p1;
          tell1Data[iBeetle][iChan+32*(iPort+2)] = p2;
          tell1Data[iBeetle+1][iChan+32*iPort] = p3;
          tell1Data[iBeetle+1][iChan+32*(iPort+2)] = p4;
        }
      }
      else if(cntWD == 216){ // Words 216-223 contains Event Info
        evtInfo->setWord0(*w);
        if (msgLevel(MSG::DEBUG)) {
          debug() << "(Event Info) Event Information (bits): " 
                  << std::bitset<8>(evtInfo->EventInformation()) 
                  << " | Bank List (bits): "
                  << std::bitset<8>(evtInfo->BankList())
                  << " | Detector ID (dec): "
                  << (unsigned int) evtInfo->DetectorID()
                  << " | Bunch Counter (dec): " << evtInfo->bCnt() << endmsg;
        }
      }
      else if(cntWD == 217){  
        evtInfo->setWord1(*w);
        if( msgLevel(MSG::DEBUG) ) {
          debug() << "(Event Info) L0-EventID (dec): " << (int)*w << endmsg;
        }
        if( L0EvtID == 0){ 
          // For each bank, L0EvtID is initialized. So the first time in the
          // bank, L0EvtID is checked.
          L0EvtID = (unsigned int) evtInfo->L0EvID();
        } 
        else{ 
          // The rest of the time (for all PPs, all TELL1), there's a check
          // that L0EvtID is the same for all.
          if( (unsigned int) evtInfo->L0EvID() != L0EvtID ){
            error() << "L0-Event ID not the same for all!" << endmsg;	     
          }
        }
      }
      else if( cntWD == 218 ){
        evtInfo->setWord2(*w);
        if( msgLevel(MSG::DEBUG) ) {
          debug() << "(Event Info) Reserved Bits (hex): " 
                  << std::hex << evtInfo->R1() 
                  << " | Process Info (bits): "
                  << std::bitset<8>(evtInfo->ProcessInfo())
                  << " | PCN (from Beetle 0) (dec): " 
                  << std::dec << (unsigned int) evtInfo->pcn() << endmsg;
        }
      }	
      else if(cntWD == 219){
        evtInfo->setWord3(*w);     
        if (msgLevel(MSG::DEBUG)){
          debug() << "(Event Info) Chip Addr (dec): " 
                  << (unsigned int) evtInfo->ChipAddr()
                  << " | Sync RAM Full (bits): " 
                  << std::bitset<6>(evtInfo->SyncRAMFull())
                      << " | TLK Link Loss (bits): " 
                  <<  std::bitset<6>(evtInfo->tlkLnkLoss()) << endmsg;
          debug() << "(Event Info) | Sync Evt Size Err. (bits): " 
                  <<  std::bitset<6>(evtInfo->SyncEvtSizeError())
                  << " | Opt. Link Disable (bits): " 
                  << std::bitset<6>(evtInfo->OptLnkDisable()) 
                  << " | Opt. Link NoEvent (bits): " 
                  << std::bitset<6>(evtInfo->OptLnkNoEvt()) << endmsg;
        }
        if( m_printErrorInfo ) { 
          if( evtInfo->SyncRAMFull() != 0 )
            error() << "Sync RAM Full in TELL1 ID " 
                    <<  p->sourceID() << ", PP-FPGA " << cntPP 
                    << ". Value (One bit per link): " 
                    << std::bitset<6>(evtInfo->SyncRAMFull()) << endmsg; 
          if( evtInfo->tlkLnkLoss() != 0 ) 
            error() << "TLK Link loss in TELL1 ID " 
                    <<  p->sourceID() << ", PP-FPGA " << cntPP 
                    << ". Value (One bit per link): "
                    << std::bitset<6>(evtInfo->tlkLnkLoss()) << endmsg;
          if( evtInfo->SyncEvtSizeError() != 0 )
            error() << "Sync Event size error in TELL1 ID " 
                    <<  p->sourceID() << ", PP-FPGA " << cntPP 
                    << ". Value (One bit per link): "
                    << std::bitset<6>(evtInfo->SyncEvtSizeError()) << endmsg;
          if( evtInfo->OptLnkNoEvt() != 0 ) 
            error() << "Optical Link No Event in TELL1 ID " 
                    <<  p->sourceID() << ", PP-FPGA " << cntPP 
                    << ". Value (One bit per link): " 
                    << std::bitset<6>(evtInfo->OptLnkNoEvt()) << endmsg;
        }
      }
      else if( cntWD == 220 ) {
        evtInfo->setWord4(*w);
        if (msgLevel(MSG::DEBUG)){
          debug() << "(Event Info) Reserved bit (bits): " 
                  << std::bitset<1>(evtInfo->R2())
                  << " | PCN Error (bits):" 
                  << std::bitset<1>(evtInfo->pcnError())
                  << " | Optical Link no clock (bits): " 
                  << std::bitset<6>(evtInfo->OptLnkNoClock()) << endmsg;
          debug() << "(Event Info) | Header Pseudo Err. (bits): " 
                  << std::bitset<24>(evtInfo->HeaderPseudoError()) << endmsg;
        }
        
        if( m_printErrorInfo ) { 
          if( evtInfo->HeaderPseudoError() != 0) 
            error() << "Header Pseudo Error in TELL1 ID " 
                    <<  p->sourceID() << ", PP-FPGA " << cntPP 
                    << ". Value (One bit per port=24 bits): " 
                    << std::bitset<24>(evtInfo->HeaderPseudoError()) << endmsg;
        }
      }		
      else if(cntWD == 221){
        evtInfo->setWord5(*w); 
        if (msgLevel(MSG::DEBUG)) {
          debug() << "(Event Info) Beetle3 PCN (dec): " 
                  << (unsigned int) evtInfo->pcnBeetle3()
                  << " | Beetle2 PCN (dec): " 
                  << (unsigned int) evtInfo->pcnBeetle2()
                  << " | Beetle1 PCN (dec): " 
                  << (unsigned int) evtInfo->pcnBeetle1()
                  << " | Beetle0 PCN (dec): " 
                  << (unsigned int) evtInfo->pcnBeetle0() <<endmsg;
        }
	    }	    
      else if(cntWD == 222){ 
        eventInfo[cntPP]->setWord6(*w);     
        if( msgLevel(MSG::DEBUG) ) {
          debug() << "(Event Info) Reserved bits (hex): " << std::hex 
                  << evtInfo->R3()
                  << " | Beetle5 PCN (dec): " << std::dec 
                  << (unsigned int) evtInfo->pcnBeetle5()
                  << " | Beetle4 PCN (dec): " 
                  << (unsigned int) evtInfo->pcnBeetle4() << endmsg;
        }        
      }
      else if(cntWD == 223){
        eventInfo[cntPP]->setWord7(*w);
        if( msgLevel(MSG::DEBUG) ) {
          debug() << "(Event Info) Reserved bits (hex): " <<  std::hex 
                  << (unsigned int) evtInfo->R4()
                  << " | I Headers: Beet.5 (dec): " << std::dec  
                  << (unsigned int) evtInfo->iHeaderBeetle5()
                  << " | Beet.4: " << (unsigned int) evtInfo->iHeaderBeetle4()
                  << " | Beet.3: " << (unsigned int) evtInfo->iHeaderBeetle3()
                  << " | Beet.2: " << (unsigned int) evtInfo->iHeaderBeetle2()
                  << " | Beet.1: " << (unsigned int) evtInfo->iHeaderBeetle1()
                  << " | Beet.0: " << (unsigned int) evtInfo->iHeaderBeetle0() 
                  << std::dec << endmsg;
        }
      }			
      else {
        error() << "Not the right number of words: word number " <<  cntWD 
                << ", you should have 224 words per PP" << endmsg;
      }		
			
      cntWD++;
    }// Loop over all words

    // make an empty tell1 data object
    STTELL1Data* myData = new STTELL1Data( tell1Data, tell1Header, 
                                           sentPP, eventInfo );
    
    // put into the container, second argument is TELL1 id
    outputData->insert(myData, int(p->sourceID()));
    
  }// end of loop over banks of a certain type
  
  return StatusCode::SUCCESS;
}
