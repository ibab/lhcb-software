// $Id: RawBankToSTClusterAlg.cpp,v 1.33 2008-07-15 15:43:05 mneedham Exp $

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RawBankToSTClusterAlg.h"


// Event
#include "Event/RawEvent.h"
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STFun.h"

#include "SiDAQ/SiADCWord.h"

#include "Kernel/STDecoder.h"
#include "Kernel/STDetSwitch.h"


#include "boost/lexical_cast.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBankToSTClusterAlg );

RawBankToSTClusterAlg::RawBankToSTClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator){
 
 // Standard constructor, initializes variables
  declareProperty("clusterLocation", m_clusterLocation = STClusterLocation::TTClusters);
  declareProperty("BankType", m_bankTypeString = "TT");
}

RawBankToSTClusterAlg::~RawBankToSTClusterAlg() {
  // Destructor
};

StatusCode RawBankToSTClusterAlg::initialize() {

  // Initialization
  StatusCode sc = STDecodingBaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(detType(),m_clusterLocation); 
  
  return StatusCode::SUCCESS;
}
    
StatusCode RawBankToSTClusterAlg::execute() {

  // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation );

  // make a new digits container
  STClusters* clusCont = new STClusters();
  clusCont->reserve(2000);
  put(clusCont, m_clusterLocation);

  // decode banks
  StatusCode sc = decodeBanks(rawEvt,clusCont);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  // sort
  std::sort(clusCont->begin(),
            clusCont->end(),
            STDataFunctor::Less_by_Channel<const STCluster*>());

 
  return sc;
};


StatusCode RawBankToSTClusterAlg::decodeBanks(RawEvent* rawEvt, 
                                              STClusters* clusCont ) const{

  // create Clusters from this type 
  bool pcnSync = true;
  std::vector<unsigned int> bankList;

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());

  std::vector<unsigned int> missing = missingInAction(tBanks);
  if (missing.empty() == false){
    counter("lost Banks") += missing.size();
    if (tBanks.size() == 0){
       ++counter("no banks found");
      return StatusCode::SUCCESS;
    }
  }

  // vote on the pcns
  const unsigned int pcn = pcnVote(tBanks);
  if (pcn == STDAQ::inValidPcn && !m_skipErrors) {
    counter("skipped Banks") += tBanks.size();
    debug() << "PCN vote failed with " << tBanks.size() << endmsg; 
    return Warning("PCN vote failed", StatusCode::SUCCESS);
  }
    
  std::vector<RawBank* >::const_iterator iterBank;
  // loop over the banks of this type..
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    ++counter("found Banks");
    std::string foundBanks = "source id" +  boost::lexical_cast<std::string>((*iterBank)->sourceID());    
    ++counter(foundBanks);

    // get the board and data
    const STTell1Board* aBoard =  readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID())); 
    if (!aBoard && !m_skipErrors){
      // not a valid IT
      Warning("Invalid source ID --> skip bank",StatusCode::SUCCESS); 
      ++counter("skipped Banks");
      continue;
    }

    // make a decoder
    STDecoder decoder((*iterBank)->data());    
    // get verion of the bank
    const int version = forceVersion() ? m_forcedVersion: (*iterBank)->version();

    //    std::cout << "version " << version << std::endl;

    if (decoder.hasError() == true && !m_skipErrors){
      bankList.push_back((*iterBank)->sourceID());
      std::string errorBank = "bank has errors, skip sourceID "+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      Warning(errorBank, StatusCode::SUCCESS);
      ++counter("skipped Banks");
      continue;
    }

    const unsigned bankpcn = decoder.header().pcn();
    if (pcn != bankpcn && !m_skipErrors){
      bankList.push_back((*iterBank)->sourceID());
      std::string errorBank = "PCNs out of sync, sourceID "+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      debug() << "Expected " << pcn << " found " << bankpcn << endmsg;
      Warning(errorBank, StatusCode::SUCCESS);
      ++counter("skipped Banks");
      continue; 
    }

    // check the integrity of the bank --> always skip if not ok
    if (!m_skipErrors && checkDataIntegrity(decoder, aBoard , (*iterBank)->size() , version) == false) {
      bankList.push_back((*iterBank)->sourceID());
      continue;
    }

    // iterator over the data....
    STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
    for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){
       createCluster(iterDecoder->first,aBoard,
                     iterDecoder->second,version, clusCont);
    } // iterDecoder
    

  } // iterBank
   
  createSummaryBlock(clusCont->size(), pcn, pcnSync, bankList, missing);

  return StatusCode::SUCCESS;

}

StatusCode RawBankToSTClusterAlg::createCluster(const STClusterWord& aWord,
                                                const STTell1Board* aBoard,
                                                const std::vector<SiADCWord>& 
                                                adcValues,
                                                const unsigned int version,
                                                STClusters* clusCont) const{
  // stream the neighbour sum
  std::vector<SiADCWord>::const_iterator iterADC = adcValues.begin();
  char neighbour = *iterADC;  
  ++iterADC;

  unsigned int fracStrip = aWord.fracStripBits();
    
  // make a temporary vector to contain the ADC
  std::vector<SiADCWord> tWords;
  // std::vector<SiADCWord>::iterator start = adcValues.begin()+1;
  tWords.insert(tWords.begin(), adcValues.begin()+1, adcValues.end());

  // estimate the offset
  double stripNum = mean(tWords); 
  double interStripPos = STFun::stripFraction(stripNum - floor(stripNum));
  if (interStripPos > 0.99) stripNum +=1; 
  unsigned int offset = (unsigned int)stripNum; 

  STCluster::ADCVector adcs ; 
  for (unsigned int i = 0; i < tWords.size() ; ++i){
    adcs.push_back(std::make_pair(i-offset,(int)tWords[i].adc()));
  } // iDigit

  STTell1Board::chanPair nearestChan = aBoard->DAQToOffline(aWord.channelID(),fracStrip,
                                                            version);

  aBoard->ADCToOffline(aWord.channelID(),adcs,version,offset,interStripPos);

  // make cluster +set things
  STCluster* newCluster = new STCluster(this->word2LiteCluster(aWord, 
                                                               nearestChan.first,
                                                               nearestChan.second),
                                                               adcs,neighbour, aBoard->boardID().id(), 
                                                               aWord.channelID());

  if (!clusCont->object(nearestChan.first)) {
    clusCont->insert(newCluster,nearestChan.first);
  }   
  else {
    debug() << "Cluster already exists not inserted: " << aBoard->boardID()<< " " <<  aWord.channelID() << endmsg;  
    Warning("Failed to insert cluster --> exists in container", StatusCode::SUCCESS , 100);
    delete newCluster; 
  }
  
  return StatusCode::SUCCESS;
}


double RawBankToSTClusterAlg::mean(const std::vector<SiADCWord>& adcValues) const
{
 
  double sum = 0;
  double totCharge = 0;
  // note the first is the neighbour sum..
  for (unsigned int i = 0; i < adcValues.size() ; ++i){
     sum += adcValues[i].adc()*i;
     totCharge += adcValues[i].adc();
  } // i                                                                                        
  return (sum/totCharge);
}



StatusCode RawBankToSTClusterAlg::finalize() {

  const double failed = counter("skipped Banks").flag();
  const double processed = counter("found Banks").flag();  
  const double eff = 1.0 - (failed/processed); 

  info() << "Successfully processed " << 100* eff << " %"  << endmsg;
    
  return STDecodingBaseAlg::finalize();
}



