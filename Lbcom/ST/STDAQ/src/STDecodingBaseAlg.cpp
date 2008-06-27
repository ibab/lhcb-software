// $Id: STDecodingBaseAlg.cpp,v 1.9 2008-06-27 12:09:59 mneedham Exp $

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

#include "SiDAQ/SiADCWord.h"
#include "Kernel/STRawBankMap.h"
#include "SiDAQ/SiHeaderWord.h"
#include "Kernel/STDecoder.h"
#include "Kernel/STDetSwitch.h"

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
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // bank type
  m_bankType =  STRawBankMap::stringToType(m_detType);

  // tracker
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));


  return StatusCode::SUCCESS;
}
    

void STDecodingBaseAlg::createSummaryBlock(const unsigned int nclus, const unsigned int pcn, 
                                           const bool pcnsync, const std::vector<unsigned int>& bankList) const{

  STSummary* sum = new STSummary(nclus,pcn,pcnsync,bankList);   
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


StatusCode STDecodingBaseAlg::finalize() {

  const double failed = counter("skipped Banks").flag();
  const double processed = counter("found Banks").flag();  
  const double eff = 1.0 - (failed/processed); 

  info() << "Successfully processed " << 100* eff << " %"  << endmsg;
    
  return GaudiAlgorithm::finalize();
}

