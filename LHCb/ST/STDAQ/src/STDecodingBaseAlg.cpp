#include <algorithm>


// local
#include "STDecodingBaseAlg.h"

// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STCluster.h"
#include "Event/STSummary.h"
#include "Event/ODIN.h"

#include "Kernel/STDataFunctor.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STRawBankMap.h"
#include "Kernel/STDecoder.h"
#include "Kernel/STLexicalCaster.h"


#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "boost/lexical_cast.hpp"


#include "STDet/DeSTDetector.h"

#include <boost/foreach.hpp>


using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------


STDecodingBaseAlg::STDecodingBaseAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
ST::AlgBase (name , pSvcLocator),
m_bankTypeString(""){
 
 // Standard constructor, initializes variables

 
 declareSTConfigProperty("ErrorLocation",m_errorLocation, STTELL1BoardErrorBankLocation::TTErrors );
 declareSTConfigProperty("summaryLocation", m_summaryLocation , STSummaryLocation::TTSummary);
 declareSTConfigProperty("ErrorBank", m_errorBankString , "TTError"); 
 declareSTConfigProperty("PedestalBank", m_pedestalBankString , "TTPedestal"); 
 declareSTConfigProperty("FullBank", m_fullBankString , "TTFull"); 
 
 declareProperty("skipBanksWithErrors", m_skipErrors = false );
 declareProperty("recoverMode", m_recoverMode = true);
 
 declareProperty( "rawEventLocation",  m_rawEventLocation = "", 
                  "OBSOLETE. Use RawEventLocations instead" );
 declareProperty( "RawEventLocations", m_rawEventLocations={LHCb::RawEventLocation::Other,LHCb::RawEventLocation::Default},
                  "List of possible locations of the RawEvent object in the"
                  " transient store. By default it is LHCb::RawEventLocation::Other,"
                  " LHCb::RawEventLocation::Default.");
 declareProperty("forcedVersion", m_forcedVersion = STDAQ::inValidVersion);
 declareProperty("checkValidity", m_checkValidSpill = true);

 setForcedInit();
}

STDecodingBaseAlg::~STDecodingBaseAlg() {
  // Destructor
}

StatusCode STDecodingBaseAlg::initialize() {

  // Initialization
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return sc;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> initialize " << endmsg;

  if (! m_rawEventLocation.empty()) {
    warning() << "The rawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }

  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  // Initialise the RawEvent locations
  bool usingDefaultLocation = true;
  //if (!m_rawEventLocation.empty()) usingDefaultLocation=false;
  if (m_rawEventLocations.size()>2 || m_rawEventLocations.empty()) usingDefaultLocation=false;
  if (m_rawEventLocations.size()>0 
      && m_rawEventLocations[0]!=LHCb::RawEventLocation::Other 
      && m_rawEventLocations[0]!=LHCb::RawEventLocation::Default) usingDefaultLocation=false;
  
  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }

  // Assume that is a spillover location is wanted, it has been requested as first location!
  std::string spill = toSpill(m_rawEventLocations[0]);
  m_spillOffset = spillOffset(spill); 
   
  // bank type
  if (m_bankTypeString != ""){
    m_bankType =  STRawBankMap::stringToType(m_bankTypeString);
    if (m_bankType ==  LHCb::RawBank::Velo){
      fatal() << "Wrong detector type: only IT or TT !"<< endmsg;
      return StatusCode::FAILURE; 
    }
  }

  // bank type
  m_errorType =  STRawBankMap::stringToType(m_errorBankString);
  if (m_errorType ==  LHCb::RawBank::Velo){
    fatal() << "Wrong detector type: only IT or TT error banks!"<< endmsg;
    return StatusCode::FAILURE; 
  } 

  // pedestal bank
  m_pedestalType =  STRawBankMap::stringToType(m_pedestalBankString);
  if (m_bankType ==  LHCb::RawBank::Velo){
    fatal() << "Wrong detector type: only IT or TT !"<< endmsg;
    return StatusCode::FAILURE; 
  }

  // full bank
  m_fullType =  STRawBankMap::stringToType(m_fullBankString);
  if (m_fullType ==  LHCb::RawBank::Velo){
    fatal() << "Wrong detector type: only IT or TT !"<< endmsg;
    return StatusCode::FAILURE; 
  }

  return StatusCode::SUCCESS;
}
    

void STDecodingBaseAlg::createSummaryBlock(RawEvent* rawEvt, const unsigned int& nclus, const unsigned int& pcn, 
                                           const bool pcnsync, const unsigned int bytes,
                                           const std::vector<unsigned int>& bankList, 
                                           const std::vector<unsigned int>& missing,
                                           const LHCb::STSummary::RecoveredInfo& recoveredBanks) const{
  unsigned totalBytes = bytes;
  
  // get the error banks
  const std::vector<LHCb::RawBank*>& errorBanks = rawEvt->banks(LHCb::RawBank::BankType(m_errorType));
  totalBytes += byteSize(errorBanks);
 
  // get the pedestal banks
  const std::vector<LHCb::RawBank*>& pBanks = rawEvt->banks(LHCb::RawBank::BankType(m_pedestalType));
  totalBytes += byteSize(pBanks);
 
  // get the full banks
  const std::vector<LHCb::RawBank*>& fullBanks = rawEvt->banks(LHCb::RawBank::BankType(m_fullType));
  totalBytes += byteSize(fullBanks);
 
  STSummary* sum = new STSummary(nclus,pcn,pcnsync, totalBytes, 
                                 fullBanks.size(), pBanks.size(), 
                                 errorBanks.size() , bankList, missing, recoveredBanks);

   
  put(sum, m_summaryLocation);
}

unsigned int STDecodingBaseAlg::pcnVote(const std::vector<RawBank* >& banks) const{

  // make a majority vote to get the correct PCN in the event
  std::map<unsigned int, unsigned int> pcns;
  std::vector<RawBank*>::const_iterator iterBank = banks.begin();
  for (iterBank = banks.begin(); iterBank != banks.end() ; ++iterBank){
    STDecoder decoder((*iterBank)->data());    
    // only the good are allowed to vote [the US system..]
    if (decoder.header().hasError() == false) pcns[(decoder.header().pcn())] += 1;
   } // iterBank

  unsigned int majorityVote = STDAQ::inValidPcn;
  unsigned int maxValue = 0;
  std::map<unsigned int, unsigned int>::iterator iter = pcns.begin();
  for (; iter != pcns.end() ; ++iter){
    if (iter->second > maxValue){
      majorityVote = iter->first;
      maxValue = iter->second;
    }
  } // iter

  return majorityVote;
}

bool STDecodingBaseAlg::checkDataIntegrity(STDecoder& decoder, const STTell1Board* aBoard, 
                                           const unsigned int bankSize, const STDAQ::version& bankVersion) const{

  // check the consistancy of the data

  bool ok = true;

  STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
  for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){

    const STClusterWord aWord = iterDecoder->first;

    // make some consistancy checks
    if ((iterDecoder->second.size() - 1u  < aWord.pseudoSize())) {
      unsigned int fracStrip = aWord.fracStripBits();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "adc values do not match ! " << iterDecoder->second.size()-1u << " "
                << aWord.pseudoSize() << " offline chan "
                << aBoard->DAQToOffline(fracStrip,bankVersion,STDAQ::StripRepresentation(aWord.channelID())) 
                << " source ID  " << aBoard->boardID()  <<  " chan "  << aWord.channelID()   
                << endmsg ;
      Warning("ADC values do not match", StatusCode::SUCCESS,2);
      ok = false;
      break;
    }

    // decode the channel
    if (aBoard->validChannel(aWord.channelID()) == false){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "invalid TELL1 channel number board: " << aBoard->boardID() << " chan " << aWord.channelID() << endmsg;
      Warning("Invalid tell1 channel", StatusCode::SUCCESS,2); 
      ok = false;
      break;
    }

  } // loop clusters

  // final check that we read the total number of bytes in the bank
  if (ok && (unsigned int)iterDecoder.bytesRead() != bankSize){
    ok = false;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Inconsistant byte count " << aBoard->boardID() << " Read: "  << iterDecoder.bytesRead()
              << " Expected: " << bankSize << endmsg;
    Warning("Inconsistant byte count", StatusCode::SUCCESS);
  }

  if (!ok) ++counter("skipped Banks");
 
  return ok;
}

std::vector<unsigned int> STDecodingBaseAlg::missingInAction(const std::vector<RawBank* >& banks) const{

  std::vector<unsigned int> missing;

  if ( banks.size() != readoutTool()->nBoard()) {
    for (unsigned int iBoard = 0u; iBoard < readoutTool()->nBoard() ; ++iBoard ){
      int testID = readoutTool()->findByOrder(iBoard)->boardID().id();
      std::vector<RawBank* >::const_iterator iterBank = banks.begin();
      for (; iterBank != banks.end() && (*iterBank)->sourceID() != testID; ++iterBank){} // iterBank 
      if (iterBank == banks.end()){
        missing.push_back((unsigned int)testID);
        std::string lostBank = "lost bank "+
	 boost::lexical_cast<std::string>(testID);  
        Warning(lostBank, StatusCode::SUCCESS,0).ignore();
      }
    } // iBoard
  }
  return missing;
}

LHCb::STTELL1BoardErrorBanks* STDecodingBaseAlg::getErrorBanks() const{
  if (!exist<STTELL1BoardErrorBanks>(m_errorLocation)) {
    // we have to do the decoding
    StatusCode sc = decodeErrors();
    if (sc.isFailure()){
      Warning("Error Bank Decoding failed",StatusCode::FAILURE,2);
      return 0;
    }
  }
  return get<STTELL1BoardErrorBanks>(m_errorLocation);
}

StatusCode STDecodingBaseAlg::decodeErrors() const{

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute " << endmsg;

  // get the raw event + odin info
  LHCb::RawEvent* raw = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    if (exist<LHCb::RawEvent>(*p)){
      raw = get<LHCb::RawEvent>(*p);
      break;
    }
  }
  if( raw == NULL ) return Error("Failed to find raw data");
  
 // make an empty output vector
 STTELL1BoardErrorBanks* outputErrors = new STTELL1BoardErrorBanks();
 
 // Pick up ITError bank 
 const std::vector<LHCb::RawBank*>& itf = raw->banks(LHCb::RawBank::BankType(m_errorType)); 
 
 if (itf.size() == 0u){
   if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
     debug() <<"event has no error banks " << endmsg;
 }
 else {
    ++counter("events with error banks");
    counter("total # error banks") += itf.size(); 
 }

 if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
   debug() << "Starting to decode " << itf.size() << detType() <<"Error bank(s)" <<  endmsg;
  
 for( std::vector<LHCb::RawBank*>::const_iterator itB = itf.begin(); itB != itf.end(); ++itB ) {

   std::string errorBank = "sourceID "+
	boost::lexical_cast<std::string>((*itB)->sourceID());  
   ++counter(errorBank);

   if ((*itB)->magic() != RawBank::MagicPattern) {
     std::string pattern = "wrong magic pattern "+
	boost::lexical_cast<std::string>((*itB)->sourceID());  
     Warning(pattern, StatusCode::SUCCESS,2); 
     continue;
   }

   const unsigned int* p = (*itB)->data();
   unsigned int w=0;
   const unsigned int bankEnd = (*itB)->size()/sizeof(unsigned int);

   // bank has to be at least 28 words 
   if (bankEnd < STDAQ::minErrorBankWords){
     warning() << "Error bank length is " << bankEnd << " and should be at least " << STDAQ::minErrorBankWords << endmsg;
     Warning("Error bank too short --> not decoded for TELL1 " + ST::toString((*itB)->sourceID()), StatusCode::SUCCESS,2);
     continue;
   }

   // and less than 56 words
   if (bankEnd > STDAQ::maxErrorBankWords){
     warning() << "Error bank length is " << bankEnd << " and should be at most " << STDAQ::maxErrorBankWords << endmsg;
     Warning("Error bank too long --> not decoded for TELL1 " + ST::toString((*itB)->sourceID()), StatusCode::SUCCESS,2);
     continue;
   }

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Decoding bank number of type "<< detType() << "Error (TELL1 ID: " <<  (*itB)->sourceID() 
              << ", Size: " <<  (*itB)->size() << " bytes)" << endmsg;

       
   // make an empty tell1 data object
   STTELL1BoardErrorBank* myData = new STTELL1BoardErrorBank();
   outputErrors->insert(myData, (*itB)->sourceID()); 
   
   for (unsigned int ipp = 0; ipp < STDAQ::npp && w != bankEnd ; ++ipp ){

     if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
       debug() << "#######  Parsing now data from PP " << ipp << " #####################"<< endmsg; 

     // we must find 5 words
     if (bankEnd - w < 5 ){
       Warning("Ran out of words to read", StatusCode::SUCCESS,2);
       break;
     }
 
     STTELL1Error* errorInfo = new STTELL1Error(p[w], p[w+1], p[w+2], p[w+3], p[w+4]);
     myData->addToErrorInfo(errorInfo); 
     w +=5; // read 5 first words

     const unsigned int  nOptional = errorInfo->nOptionalWords();

     // we must find the optional words + 2 more control words
     if (bankEnd - w < nOptional + 2 ){
        Warning("Ran out of words to read", StatusCode::SUCCESS,2);
        break;
      }

     const unsigned int *eInfo;
     eInfo = 0;

     if (errorInfo->hasErrorInfo()){
       //errorInfo->setOptionalErrorWords(p[w], p[w+1], p[w+2], p[w+3], p[w+4]);
       eInfo = &p[w];
       w+= 5;
     } // has error information

     errorInfo->setWord10(p[w]); ++w;
     errorInfo->setWord11(p[w]); ++w;
 
     // then some more optional stuff
     if (errorInfo->hasNZS() == true){
       errorInfo->setWord12(p[w]); ++w;
     }  // nsz info...

     // then some more optional stuff
     if (errorInfo->hasPed() == true){
       errorInfo->setWord13(p[w]); ++w;
     }

     if (errorInfo->hasErrorInfo()){
       errorInfo->setOptionalErrorWords(eInfo[0], eInfo[1], eInfo[2], eInfo[3], eInfo[4]);
     } // has error information
      
   } //  loop ip [ppx's]
   
   if (w != bankEnd){
     error() << "read " << w << " words, expected: " << bankEnd << endmsg;
   }  

  }// end of loop over banks of a certain type

 put(outputErrors, m_errorLocation);
 
 return StatusCode::SUCCESS;
} 


std::string STDecodingBaseAlg::toSpill(const std::string& location) const{

  std::string theSpill = "";
  std::string names[2] = {"Prev", "Next"};
  for (unsigned int is = 0; is < 2; ++is){ 
    std::string::size_type iPos = location.find(names[is]);
    if (iPos != std::string::npos){
      std::string startSpill = location.substr(iPos,location.size() - iPos);
      std::string::size_type iPos2 = startSpill.find("/");
      theSpill = startSpill.substr(0,iPos2); 
      break;
    }
  }  // is
  return theSpill;
}

LHCb::STCluster::Spill STDecodingBaseAlg::spillOffset(const std::string& spill) const{

  // convert spill to offset in time
  return (spill.size() > 4u ? LHCb::STCluster::SpillToType(spill) : LHCb::STCluster::Central);    
}

bool STDecodingBaseAlg::validSpill() const{

  if (m_checkValidSpill == false) return true;

  // check spill is actually read out using the ODIN
  const ODIN* odin = get<ODIN>(ODINLocation::Default); 
  const unsigned int numberOfSpills = odin->timeAlignmentEventWindow();
  return (unsigned int)abs(m_spillOffset) <= numberOfSpills ;
}


unsigned int STDecodingBaseAlg::byteSize(const std::vector<RawBank*>& banks) const{

  unsigned int totalSize = 0u;
  BOOST_FOREACH(RawBank* bank, banks) {
    totalSize += bank->totalSize();
  }

  return totalSize;
}


