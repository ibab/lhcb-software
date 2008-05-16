// $Id: RawBankToSTClusterAlg.cpp,v 1.21 2008-05-16 07:29:14 mneedham Exp $

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
#include "STDAQGeneral.h"

#include "Kernel/STDecoder.h"
#include "Kernel/STDetSwitch.h"

#include "STDet/DeSTDetector.h"

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
  declareProperty("rawEventLocation",m_rawEventLocation = RawEventLocation::Default);
  declareProperty("createSummary", m_createSummary = true);
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
  unsigned int pcn = STDAQ::inValidPcn; // not allowed
  bool pcnSync = true;
  std::vector<unsigned int> bankList;

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());
  std::vector<RawBank* >::const_iterator iterBank;
  // loop over the banks of this type..
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // get the board and data
    const STTell1Board* aBoard =  readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID())); 
    if (!aBoard){
      // not a valid IT
      Warning("Invalid source ID --> skip bank",StatusCode::SUCCESS); 
      continue;
    }

    // make a decoder
    STDecoder decoder((*iterBank)->data());    
    // get verion of the bank
    const unsigned int version = (*iterBank)->version();
    
    if (decoder.hasError() == true){
      if (m_createSummary) bankList.push_back((*iterBank)->sourceID());
      Warning("bank has errors - skip bank", StatusCode::SUCCESS);
      continue;
    }

    if (m_createSummary){
      unsigned bankpcn = decoder.header().pcn();
      if (pcn == STDAQ::inValidPcn){
        pcn = bankpcn;
      }
      else {
        if (pcn != bankpcn) {
          pcnSync = false;
          Warning("PCNs out of sync", StatusCode::SUCCESS);
          continue; 
	}
      }
    } // create summary

    // iterator over the data....
    STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
    for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){
      StatusCode sc = createCluster(iterDecoder->first,aBoard,
                                    iterDecoder->second,clusCont,version);
      if (sc.isFailure()) {
        Warning("Error decoding ADCs", StatusCode::SUCCESS);
        continue;
      }
    } // iterDecoder
    

    if (iterDecoder.bytesRead() != ((*iterBank)->size())){
      debug() << "Inconsistant byte count Read: "  << iterDecoder.bytesRead()
                << " Expected: " << (*iterBank)->size() << " " << (*iterBank)->sourceID()<< endmsg;
      Warning("Inconsistant byte count", StatusCode::SUCCESS);
      continue;
    }
  } // iterBank
   
  if (m_createSummary) {
    createSummaryBlock(clusCont->size(), pcn, pcnSync, bankList);
  }

  return StatusCode::SUCCESS;

}

StatusCode RawBankToSTClusterAlg::createCluster(const STClusterWord& aWord,
                                                const STTell1Board* aBoard,
                                                const std::vector<SiADCWord>& 
                                                adcValues,
                                                STClusters* clusCont,
                                                const unsigned int version) const{
  // stream the neighbour sum
  std::vector<SiADCWord>::const_iterator iterADC = adcValues.begin();
  char neighbour = *iterADC;  
  ++iterADC;

  unsigned int fracStrip = aWord.fracStripBits();
    
  // make some consistancy checks
  if ((adcValues.size() - 1u  < aWord.pseudoSize())) {
    debug() << "adc values do not match ! " << adcValues.size()-1 << " "
              << aWord.pseudoSize() << " chan "
              << aBoard->DAQToOffline(aWord.channelID(),fracStrip,version) 
              << endmsg ;
    return Warning("ADC values do not match", StatusCode::FAILURE);
  }

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

  // decode the channel
  if (aBoard->validChannel(aWord.channelID()) == false){
    return Warning("Invalid tell1 channel --> skip cluster", StatusCode::SUCCESS); 
  }

  STChannelID nearestChan = aBoard->DAQToOffline(aWord.channelID(),fracStrip,
                                                 version);
  if (!tracker()->isValid(nearestChan)){
    return Warning("invalid detector channel number", StatusCode::SUCCESS);
  }

  aBoard->ADCToOffline(aWord.channelID(),adcs,version,offset,interStripPos);

  // make cluster +set things
  STCluster* newCluster = new STCluster(this->word2LiteCluster(aWord, 
                                                               nearestChan,
                                                               fracStrip),
                                        adcs,neighbour, aBoard->boardID().id());

  // add to container
  if (!clusCont->object(nearestChan)) {
    clusCont->insert(newCluster,nearestChan);
  }
  else {
    delete newCluster;
    debug() << "Cluster already exists not inserted: " << nearestChan << endmsg;  
    return Warning("Failed to insert cluster --> exists in container", StatusCode::SUCCESS , 100);
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





