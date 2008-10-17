// $Id: RawBankToSTClusterAlg.cpp,v 1.50 2008-10-17 14:10:22 mneedham Exp $

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RawBankToSTClusterAlg.h"


// Event
#include "Event/RawEvent.h"
#include "Event/STCluster.h"
#include "Event/STSummary.h"

#include "LHCbMath/LHCbMath.h"
#include "Event/STLiteCluster.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STFun.h"
#include "SiDAQ/SiADCWord.h"
#include "Kernel/STDecoder.h"


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
  declareSTConfigProperty("clusterLocation", m_clusterLocation , STClusterLocation::TTClusters);
  declareSTConfigProperty("BankType", m_bankTypeString , "TT");
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
  
  return StatusCode::SUCCESS;
}
    
StatusCode RawBankToSTClusterAlg::execute() {

  // make a new digits container
  STClusters* clusCont = new STClusters();
  clusCont->reserve(2000);
  put(clusCont, m_clusterLocation);

  if (!validSpill()) {
    return Warning("Not a valid spill",StatusCode::SUCCESS, 1);
  }

   // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation );


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
  STSummary::RecoveredInfo recoveredBanks;

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());

  std::vector<unsigned int> missing = missingInAction(tBanks);
  if (missing.empty() == false){
    counter("lost Banks") += missing.size();
    if (tBanks.size() == 0){
      createSummaryBlock(0, STDAQ::inValidPcn, false, bankList, missing, recoveredBanks);
      ++counter("no banks found");
      return StatusCode::SUCCESS;
    }
  }

  // vote on the pcns
  const unsigned int pcn = pcnVote(tBanks);
  debug() << "PCN was voted to be " << pcn << endmsg;
  if (pcn == STDAQ::inValidPcn && !m_skipErrors) {
    counter("skipped Banks") += tBanks.size();
    debug() << "PCN vote failed with " << tBanks.size() << endmsg; 
    return Warning("PCN vote failed", StatusCode::SUCCESS ,2 );
  }
    
  std::vector<RawBank* >::const_iterator iterBank;
  // loop over the banks of this type..
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    ++counter("# banks found");
    // get the board and data
    STTell1ID tell1ID = STTell1ID((unsigned int)(*iterBank)->sourceID()); 
    const STTell1Board* aBoard =  readoutTool()->findByBoardID(tell1ID); 

    if (!aBoard && !m_skipErrors){
      // not a valid IT
     std::string invalidSource = "Invalid source ID --> skip bank"+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(invalidSource,StatusCode::SUCCESS,2).ignore(); 
      ++counter("skipped Banks");
      continue;
    }

    ++counter("# valid banks");

    if ((*iterBank)->magic() != RawBank::MagicPattern) {
      std::string pattern = "wrong magic pattern "+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(pattern, StatusCode::SUCCESS, 2).ignore(); 
      ++counter("skipped banks");
      continue;
    }

    // make a decoder
    STDecoder decoder((*iterBank)->data());    
    // get verion of the bank
    const int version = forceVersion() ? m_forcedVersion: (*iterBank)->version();

    debug() << "decoding bank version " << version << endmsg;

    bool recover = false;
    if (decoder.hasError() == true && !m_skipErrors){
   
      if (!recoverMode()){
        bankList.push_back((*iterBank)->sourceID());
        std::string errorBank = "bank has errors, skip sourceID " +
          boost::lexical_cast<std::string>((*iterBank)->sourceID());
        Warning(errorBank, StatusCode::SUCCESS, 2).ignore();
        ++counter("skipped Banks");
        continue;
      }
      else {
	// flag that need to recover....
        recover = true;
        ++counter("recovered banks" +  boost::lexical_cast<std::string>((*iterBank)->sourceID()));
      }
    }

    const unsigned bankpcn = decoder.header().pcn();
    if (pcn != bankpcn && !m_skipErrors){
      bankList.push_back((*iterBank)->sourceID());
      std::string errorBank = "PCNs out of sync, sourceID " +
        boost::lexical_cast<std::string>((*iterBank)->sourceID());
      debug() << "Expected " << pcn << " found " << bankpcn << endmsg;
      Warning(errorBank, StatusCode::SUCCESS, 2).ignore();
      ++counter("skipped Banks");
      continue; 
    }

    // check the integrity of the bank --> always skip if not ok
    if (!m_skipErrors && checkDataIntegrity(decoder, aBoard , (*iterBank)->size() , version) == false) {
      bankList.push_back((*iterBank)->sourceID());
      continue;
    }

    // ok this is a bit ugly.....
    STTELL1BoardErrorBank* errorBank = 0;
    if (recover == true){
      errorBank = findErrorBank((*iterBank)->sourceID());
      // check what fraction we can recover
      if (errorBank !=0) recoveredBanks[(*iterBank)->sourceID()] += errorBank->fractionOK(pcn);
    } 

    // iterator over the data....
    STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
    for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){
      if (recover == false){
       createCluster(iterDecoder->first,aBoard,
                     iterDecoder->second,version, clusCont);
      }
      else {
	// check that this cluster is ok to be recovered
        if (errorBank != 0 && canBeRecovered(errorBank,iterDecoder->first, pcn) == true){
         createCluster(iterDecoder->first,aBoard,
                        iterDecoder->second,version, clusCont); 
	} 
      }
    } // iterDecoder
    

  } // iterBank
   
  createSummaryBlock(clusCont->size(), pcn, pcnSync, bankList, missing, recoveredBanks);

  return StatusCode::SUCCESS;

}

void RawBankToSTClusterAlg::createCluster( const STClusterWord& aWord,
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

  STTell1Board::chanPair nearestChan = aBoard->DAQToOffline(fracStrip,version,aWord.channelID());

  aBoard->ADCToOffline(aWord.channelID(),adcs,version,offset,interStripPos);

  // make cluster +set things
  STCluster* newCluster = new STCluster(this->word2LiteCluster(aWord, 
                                                               nearestChan.first,
                                                               nearestChan.second),
                                                               adcs,neighbour, aBoard->boardID().id(), 
                                                               aWord.channelID(), spill());

  if (!clusCont->object(nearestChan.first)) {
    clusCont->insert(newCluster,nearestChan.first);
  }   
  else {
    debug() << "Cluster already exists not inserted: " << aBoard->boardID()<< " " <<  aWord.channelID() << endmsg;  
    Warning("Failed to insert cluster --> exists in container", StatusCode::SUCCESS , 100);
    delete newCluster; 
  }
  
  return;
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
  const double processed = counter("# valid banks").flag();

  double eff = 0.0; 
  if (!LHCb::Math::Equal_To<double>()(processed, 0.0)){ 
    eff = 1.0 - (failed/processed); 
  }
  info() << "Successfully processed " << 100* eff << " %"  << endmsg;
    
  return STDecodingBaseAlg::finalize();
}



