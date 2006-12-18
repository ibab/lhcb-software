// $Id: RawBankToSTClusterAlg.cpp,v 1.13 2006-12-18 10:49:45 cattanem Exp $

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

#include "ISTReadoutTool.h"
#include "STTell1Board.h"
#include "STTell1ID.h"

#include "SiDAQ/SiADCWord.h"
#include "STDAQGeneral.h"
#include "STDecoder.h"

#include "Kernel/STDetSwitch.h"

#include "STDet/DeSTDetector.h"

// GSL
#include "gsl/gsl_math.h"


using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBankToSTClusterAlg );

RawBankToSTClusterAlg::RawBankToSTClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator),
m_nBits(2){
 
 // Standard constructor, initializes variables
  declareProperty( "clusterLocation", m_clusterLocation = STClusterLocation::TTClusters); 
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
  RawEvent* rawEvt = get<RawEvent>(RawEventLocation::Default );

  // make a new digits container
  STClusters* clusCont = new STClusters();
  clusCont->reserve(2000);

  // decode banks
  StatusCode sc = decodeBanks(rawEvt,clusCont);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  // sort
  std::sort(clusCont->begin(),
            clusCont->end(),
            STDataFunctor::Less_by_Channel<const STCluster*>());

  // store
  put(clusCont, m_clusterLocation);

  return sc;
};


StatusCode RawBankToSTClusterAlg::decodeBanks(RawEvent* rawEvt, 
                                              STClusters* clusCont ) const{

  // create Clusters from this type

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());
  std::vector<RawBank* >::const_iterator iterBank;
  // loop over the banks of this type..
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // get the board and data
    STTell1Board* aBoard =  readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID())); 
    // make a decoder
    STDecoder decoder((*iterBank)->data());    
    
    if (decoder.hasError() == true){
      warning() << "bank has errors - skip event" << endmsg;
      return StatusCode::FAILURE;
    }
  
    // iterator over the data....
    STDecoder::posadc_iterator iterDecoder = decoder.posAdcBegin();
    for ( ;iterDecoder != decoder.posAdcEnd(); ++iterDecoder){
      StatusCode sc = createCluster(iterDecoder->first,aBoard,iterDecoder->second,clusCont);
      if (sc.isFailure()) {
        return StatusCode::FAILURE;
      }
    } // iterDecoder
    

    if (iterDecoder.bytesRead() != ((*iterBank)->size())){
      warning() << "Inconsistant byte count Read: "  << iterDecoder.bytesRead()
                << " Expected: " << (*iterBank)->size() << " " << (*iterBank)->sourceID()<< endmsg;
      return StatusCode::FAILURE;
    }
  } // iterBank
   
  return StatusCode::SUCCESS;

}

StatusCode RawBankToSTClusterAlg::createCluster(const STClusterWord& aWord,
                                                const STTell1Board* aBoard,
                                                const std::vector<SiADCWord>& adcValues,
                                                STClusters* clusCont) const{
  // stream the neighbour sum
  std::vector<SiADCWord>::const_iterator iterADC = adcValues.begin();
  char neighbour = *iterADC;  
  ++iterADC;

  // make some consistancy checks
  if ((adcValues.size() - 1u  < aWord.pseudoSize())) {
    warning() << "adc values do not match ! " << adcValues.size()-1 << " "
              <<  aWord.pseudoSize() << " chan "
              << aBoard->DAQToOffline(aWord.channelID()) << endmsg ;
    return StatusCode::FAILURE;
  }

  // make a temporary vector to contain the ADC
  std::vector<SiADCWord> tWords;
  // std::vector<SiADCWord>::iterator start = adcValues.begin()+1;
  tWords.insert(tWords.begin(), adcValues.begin()+1, adcValues.end());

  // estimate the offset
  double stripNum = mean(tWords); 
  double interStripPos = stripFraction(stripNum - floor(stripNum));
  if (interStripPos > 0.99) stripNum +=1; 
  unsigned int offset = (unsigned int)stripNum; 

  // decode the channel
  STChannelID nearestChan = aBoard->DAQToOffline(aWord.channelID());
  if (!tracker()->isValid(nearestChan)){
    warning() << "invalid channel " << endmsg;
    return StatusCode::FAILURE;
  }

  STCluster::ADCVector adcs ; 
  for (unsigned int i = 0; i < tWords.size() ; ++i){
    adcs.push_back(std::make_pair(i-offset,(int)tWords[i].adc()));
  } // iDigit

  // make cluster +set things
  STCluster* newCluster = new STCluster(this->word2LiteCluster(aWord, nearestChan),
                                        adcs,neighbour);

  // add to container
  clusCont->insert(newCluster,nearestChan);

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


STLiteCluster RawBankToSTClusterAlg::word2LiteCluster(STClusterWord aWord,
                                                      STChannelID chan) const{
  return STLiteCluster(aWord.fracStripBits(),aWord.pseudoSizeBits(),
                       aWord.hasHighThreshold() ,chan);
}


double RawBankToSTClusterAlg::stripFraction(const double interStripPos) const{
  return (LHCbMath::round((1<<m_nBits)*interStripPos))/double(1<<m_nBits);
}
