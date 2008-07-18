// $Id: RawBankToSTLiteClusterAlg.cpp,v 1.25 2008-07-18 09:37:26 mneedham Exp $


#include <algorithm>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RawBankToSTLiteClusterAlg.h"
#include "Kernel/ISTReadoutTool.h"

// Event
#include "Event/RawEvent.h"
#include "Event/STLiteCluster.h"


#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"

#include "Kernel/STDAQDefinitions.h"

#include "Kernel/STDecoder.h"

#include "Kernel/STDetSwitch.h"
#include "Kernel/STDataFunctor.h"

#include "Kernel/FastClusterContainer.h"

#include "boost/lexical_cast.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTLiteClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBankToSTLiteClusterAlg );

RawBankToSTLiteClusterAlg::RawBankToSTLiteClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator){
 

 // Standard constructor, initializes variables
  declareProperty( "clusterLocation", m_clusterLocation = STLiteClusterLocation::TTClusters);
  declareProperty("BankType", m_bankTypeString = "TT");
}

RawBankToSTLiteClusterAlg::~RawBankToSTLiteClusterAlg() {
  // Destructor
};

StatusCode RawBankToSTLiteClusterAlg::initialize() {

  // Initialization
  StatusCode sc = STDecodingBaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(detType(),m_clusterLocation); 
  
  return StatusCode::SUCCESS;
}
    
StatusCode RawBankToSTLiteClusterAlg::execute() {


  // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation);

 
  // decode banks
  StatusCode sc = decodeBanks(rawEvt);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  return sc;
};


StatusCode RawBankToSTLiteClusterAlg::decodeBanks(RawEvent* rawEvt) const{
 
  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());
  STLiteCluster::STLiteClusters* fCont = new STLiteCluster::STLiteClusters();
  fCont->reserve(5000);  
  put(fCont, m_clusterLocation);


  std::vector<unsigned int> missing = missingInAction(tBanks);
  if ( missing.empty() == false ){
    counter("lost Banks") += missing.size() ;
    if (tBanks.size() == 0){
      ++counter("no banks found");
      return StatusCode::SUCCESS;
    }
  } 

  const unsigned int pcn = pcnVote(tBanks);
  if (pcn == STDAQ::inValidPcn) {
    counter("skipped Banks") += tBanks.size();
    return Warning("PCN vote failed", StatusCode::SUCCESS);
  }
  
  // loop over the banks of this type..
  std::vector<RawBank* >::const_iterator iterBank =  tBanks.begin();
  for (; iterBank != tBanks.end() ; ++iterBank){

    ++counter("# valid banks");

    // get the board and data
    STTell1Board* aBoard = readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID()));
    if (!aBoard && !m_skipErrors){
      std::string invalidSource = "Invalid source ID --> skip bank"+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(invalidSource,StatusCode::SUCCESS); 
      ++counter("skipped Banks");
      continue;
    } 

   ++counter("# valid source ID");

   if ((*iterBank)->magic() != RawBank::MagicPattern) {
      std::string pattern = "wrong magic pattern "+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(pattern, StatusCode::SUCCESS);
      counter("skipped Banks") += tBanks.size();
      continue;
    }
 
    // make a SmartBank of shorts...
    STDecoder decoder((*iterBank)->data());
   
    // get number of clusters..
    if (decoder.hasError() == true && !m_skipErrors){
      std::string errorBank = "bank has errors, skip, sourceID"+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      Warning(errorBank, StatusCode::SUCCESS);
      ++counter("skipped Banks");
      continue;
    }

    const unsigned bankpcn = decoder.header().pcn();
    if (pcn != bankpcn && !m_skipErrors){
      std::string errorBank = "PCNs out of sync sourceID "+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      debug() << "Expected " << pcn << " found " << bankpcn << endmsg;
      Warning(errorBank, StatusCode::SUCCESS);
      ++counter("skipped Banks");
      continue; 
    }

    const int version = forceVersion() ? m_forcedVersion: (*iterBank)->version();

    // check the integrity of the bank --> always skip if not ok
    if (!m_skipErrors && checkDataIntegrity(decoder, aBoard , (*iterBank)->size() , version) == false) continue;

    // read in the first half of the bank
    STDecoder::pos_iterator iterDecoder = decoder.posBegin();
    for ( ;iterDecoder != decoder.posEnd(); ++iterDecoder){
      
      STClusterWord aWord = *iterDecoder;
      unsigned int fracStrip = aWord.fracStripBits();
      
      STTell1Board::chanPair chan = aBoard->DAQToOffline(aWord.channelID(),fracStrip,
                                                         version);

      STLiteCluster liteCluster(chan.second,
                                aWord.pseudoSizeBits(),
                                aWord.hasHighThreshold(),
                                chan.first);

      fCont->push_back(liteCluster);

    } //decoder
      
  } // iterBank

  // sort 
  std::stable_sort(fCont->begin(),fCont->end(), Less_by_Channel());
  const size_t decodedSize = fCont->size();
  std::unique(fCont->begin(), fCont->end(), Equal_Channel()); 
  if (fCont->size() != decodedSize){
    return Warning("Removed duplicate clusters in the decoding", StatusCode::SUCCESS, 100);
  }

  return StatusCode::SUCCESS;

}


StatusCode RawBankToSTLiteClusterAlg::finalize() {

  const double failed = counter("skipped Banks").flag();
  const double processed = counter("# valid banks").flag();  
  const double eff = 1.0 - (failed/processed); 

  info() << "Successfully processed " << 100* eff << " %"  << endmsg;
    
  return STDecodingBaseAlg::finalize();
}

