// $Id: STDecodingBaseAlg.cpp,v 1.14 2008-07-15 11:20:26 mneedham Exp $

#include <algorithm>


// local
#include "STDecodingBaseAlg.h"

// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STCluster.h"
#include "Event/STSummary.h"

#include "Kernel/STDataFunctor.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STRawBankMap.h"
#include "Kernel/STDecoder.h"
#include "Kernel/STDetSwitch.h"

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"


#include "STDet/DeSTDetector.h"

using namespace LHCb;


//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------


STDecodingBaseAlg::STDecodingBaseAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
GaudiAlgorithm (name , pSvcLocator){
 
 // Standard constructor, initializes variables
 declareProperty("readoutTool", m_readoutToolName = "TTReadoutTool");     
 declareProperty("detType", m_detType = "TT" );
 declareProperty("summaryLocation", m_summaryLocation = STSummaryLocation::TTSummary);
 declareProperty("skipBanksWithErrors", m_skipErrors = false );
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
  STDetSwitch::flip(m_detType,m_bankTypeString);
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // bank type
  m_bankType =  STRawBankMap::stringToType(m_bankTypeString);
  if (m_bankType ==  LHCb::RawBank::Velo){
   fatal() << "Wrong detector type: only IT or TT !"<< endmsg;
   return StatusCode::FAILURE; 
  } 

  return StatusCode::SUCCESS;
}
    

void STDecodingBaseAlg::createSummaryBlock(const unsigned int nclus, const unsigned int pcn, 
                                           const bool pcnsync, const std::vector<unsigned int>& bankList, 
                                           const std::vector<unsigned int>& missing) const{

  STSummary* sum = new STSummary(nclus,pcn,pcnsync,bankList, missing);   
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
              << aBoard->DAQToOffline(aWord.channelID(),fracStrip,version) 
              << " source ID  " << aBoard->boardID()  <<  " chan "  << aWord.channelID()   
              << endmsg ;
      Warning("ADC values do not match", StatusCode::FAILURE);
      ok = false;
      //      break;
    }

    // decode the channel
    if (aBoard->validChannel(aWord.channelID()) == false){
      warning() << "invalid TELL1 channel number board: " << aBoard->boardID() << " chan " << aWord.channelID() << endmsg;
      Warning("Invalid tell1 channel", StatusCode::SUCCESS); 
      ok = false;
      // break;
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
      }
    } // iBoard
  }
  return missing;
}

StatusCode STDecodingBaseAlg::finalize() {
    
  return GaudiAlgorithm::finalize();
}

