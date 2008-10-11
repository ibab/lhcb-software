// $Id: STDecodingBaseAlg.cpp,v 1.21 2008-10-11 10:40:30 mneedham Exp $

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
#include "Kernel/STDetSwitch.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "boost/lexical_cast.hpp"


#include "STDet/DeSTDetector.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------


STDecodingBaseAlg::STDecodingBaseAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
GaudiAlgorithm (name , pSvcLocator),
m_bankTypeString(""){
 
 // Standard constructor, initializes variables
 declareProperty("readoutTool", m_readoutToolName = "TTReadoutTool");     
 declareProperty("detType", m_detType = "TT" );
 
 declareProperty("ErrorLocation",m_errorLocation=STTELL1BoardErrorBankLocation::TTErrors );
 declareProperty("summaryLocation", m_summaryLocation = STSummaryLocation::TTSummary);
 declareProperty("ErrorBank", m_errorBankString = "TTError"); 
 
 declareProperty("skipBanksWithErrors", m_skipErrors = false );
 declareProperty("recoverMode", m_recoverMode = true);
 
 declareProperty("rawEventLocation",m_rawEventLocation = RawEventLocation::Default);
 declareProperty("forcedVersion", m_forcedVersion = -1);
}

STDecodingBaseAlg::~STDecodingBaseAlg() {
  // Destructor
};

StatusCode STDecodingBaseAlg::initialize() {

  // Initialization
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_readoutToolName);
  STDetSwitch::flip(m_detType,m_summaryLocation);
  STDetSwitch::flip(m_detType,m_errorLocation);
  STDetSwitch::flip(m_detType,m_errorBankString);

  std::string spill = toSpill(m_rawEventLocation);
  m_spillOffset = spillOffset(spill); 

  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // bank type
  if (m_bankTypeString != "") {
    STDetSwitch::flip(m_detType,m_bankTypeString);
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

  return StatusCode::SUCCESS;
}
    

void STDecodingBaseAlg::createSummaryBlock(const unsigned int& nclus, const unsigned int& pcn, 
                                           const bool pcnsync, const std::vector<unsigned int>& bankList, 
                                           const std::vector<unsigned int>& missing,
                                           const LHCb::STSummary::RecoveredInfo& recoveredBanks) const{

  STSummary* sum = new STSummary(nclus,pcn,pcnsync,bankList, missing, recoveredBanks);   
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
                                           const unsigned int bankSize, const int version) const{

  // check the consistancy of the data

  bool ok = true;

  STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
  for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){

    const STClusterWord aWord = iterDecoder->first;

    // make some consistancy checks
    if ((iterDecoder->second.size() - 1u  < aWord.pseudoSize())) {
      unsigned int fracStrip = aWord.fracStripBits();
      debug() << "adc values do not match ! " << iterDecoder->second.size()-1u << " "
              << aWord.pseudoSize() << " offline chan "
              << aBoard->DAQToOffline(fracStrip,version,aWord.channelID()) 
              << " source ID  " << aBoard->boardID()  <<  " chan "  << aWord.channelID()   
              << endmsg ;
      Warning("ADC values do not match", StatusCode::SUCCESS,2);
      ok = false;
      break;
    }

    // decode the channel
    if (aBoard->validChannel(aWord.channelID()) == false){
      debug() << "invalid TELL1 channel number board: " << aBoard->boardID() << " chan " << aWord.channelID() << endmsg;
      Warning("Invalid tell1 channel", StatusCode::SUCCESS,2); 
      ok = false;
      break;
    }

  } // loop clusters

  // final check that we read the total number of bytes in the bank
  if (ok && (unsigned int)iterDecoder.bytesRead() != bankSize){
    ok = false;
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
        Warning(lostBank, StatusCode::SUCCESS);
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

 debug() << "==> Execute " << endmsg;

 // get the raw event + odin info
 RawEvent* raw = get<RawEvent>(RawEventLocation::Default );

 // make an empty output vector
 STTELL1BoardErrorBanks* outputErrors = new STTELL1BoardErrorBanks();
 
 // Pick up ITError bank 
 const std::vector<LHCb::RawBank*>& itf = raw->banks(LHCb::RawBank::BankType(m_errorType)); 
 
 if (itf.size() == 0u){
   debug() <<"event has no error banks " << endmsg;
 }
 else {
    ++counter("events with error banks");
    counter("total # error banks") += itf.size(); 
 }

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
     Warning("Error bank too short --> not decoded", StatusCode::SUCCESS,2);
     continue;
   }

   // and less than 52 words
   if (bankEnd > STDAQ::maxErrorBankWords){
     Warning("Error bank too long --> not decoded", StatusCode::SUCCESS,2);
     continue;
   }

   debug() << "Decoding bank number of type "<< detType() << "Error (TELL1 ID: " <<  (*itB)->sourceID() 
           << ", Size: " <<  (*itB)->size() << " bytes)" << endmsg;

       
   // make an empty tell1 data object
   STTELL1BoardErrorBank* myData = new STTELL1BoardErrorBank();
   outputErrors->insert(myData, (*itB)->sourceID()); 
   
   for (unsigned int ipp = 0; ipp < STDAQ::npp && w != bankEnd ; ++ipp ){

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



StatusCode STDecodingBaseAlg::finalize() {
    
  return GaudiAlgorithm::finalize();
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
    
   //std::string spillNumber = spill.substr(4u,spill.size() - 4u);
   //offset = boost::lexical_cast<unsigned int>(spillNumber);
   //  }
   // return offset;
}

bool STDecodingBaseAlg::validSpill() const{

  // check spill is actually read out using the ODIN
  const ODIN* odin = get<ODIN>(ODINLocation::Default); 
  const unsigned int numberOfSpills = odin->timeAlignmentEventWindow();
  return abs(m_spillOffset) <= numberOfSpills ;
}
