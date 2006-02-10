// $Id: RawBankToSTClusterAlg.cpp,v 1.1 2006-02-10 08:59:31 mneedham Exp $

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBankToSTClusterAlg.h"


// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Kernel/STDataFunctor.h"

#include "ISTReadoutTool.h"
#include "STTell1Board.h"
#include "STTell1ID.h"

#include "SiDAQ/SiADCWord.h"
#include "STDAQGeneral.h"
#include "SiDAQ/SiHeaderWord.h"

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

// Declaration of the Algorithm Factory
static const  AlgFactory<RawBankToSTClusterAlg>          s_factory ;
const IAlgFactory& RawBankToSTClusterAlgFactory = s_factory ;

RawBankToSTClusterAlg::RawBankToSTClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator){
 
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
    STDAQ::rawInt* theData = (*iterBank)->data();
    size_t byteSize = (*iterBank)->size();    
 
    // make a SmartBank of shorts...
    ByteStream stream(theData,byteSize);    
   
    // get number of clusters..
    SiHeaderWord aHeader; stream >> aHeader; 
    if (aHeader.hasError() == true){
      warning() << "bank has errors - skip event" << endmsg;
      return StatusCode::FAILURE;
    }

    unsigned int nClus = aHeader.nClusters();
   
    // read in the first half of the bank
    std::vector<STClusterWord> clusVector(nClus);
    for (unsigned int iW = 0; iW < nClus; ++iW){
      stream >> clusVector[iW];
    } // iW

    // there is padding between the 2 halves
    if ((nClus % 2) == 1) stream.seek(2);     
    
    // loop again and make clusters
    for (unsigned int iW = 0 ; iW < nClus ; ++iW){
      StatusCode sc = createCluster(clusVector[iW],aBoard,stream,clusCont);
      if (sc.isFailure()) {
        return StatusCode::FAILURE;
      }
    } // iW

    if (stream.nRead() != byteSize ){
      warning() << "Inconsistant byte count Read: "  << stream.nRead() 
                << " Expected: " << byteSize << endmsg;
      return StatusCode::FAILURE;
    }

  } // iterBank
   
  return StatusCode::SUCCESS;

}

StatusCode RawBankToSTClusterAlg::createCluster(const STClusterWord& aWord,
                                                const STTell1Board* aBoard,
                                                LHCb::ByteStream& stream,
                                                STClusters* clusCont) const{
  // stream the neighbour sum
  char neighbour;  stream >> neighbour;  

  // and now get the digits
  std::vector<SiADCWord> adcValues;
  do {
    SiADCWord tWord;
    stream >> tWord;
    adcValues.push_back(tWord);
  } while((adcValues.back().endCluster() == false )
         &&(adcValues.size() < 4u));
  
  // make some consistancy checks
  unsigned int pseudoSize = GSL_MIN(adcValues.size(),3u);
  if ((adcValues.back().endCluster() == false) || (pseudoSize != aWord.pseudoSize())) {
    warning() << "adc values do not match !" << std::endl;
    return StatusCode::FAILURE;
  }

  // find the offset from nearest to first cluster...
  unsigned int offset = mean(adcValues); 

  // decode the channel
  STChannelID nearestChan = aBoard->DAQToOffline(aWord.channelID());
  if (!tracker()->isValid(nearestChan)){
    warning() << "invalid channel " << endmsg;
    return StatusCode::FAILURE;
  }

  STCluster::ADCVector adcs ; 
  for (unsigned int i = 0; i < adcValues.size() ; ++i){
    adcs.push_back(std::make_pair(i-offset,adcValues[i]));
  } // iDigit

  // make cluster +set things
  STCluster* newCluster = new STCluster(this->word2LiteCluster(aWord, nearestChan),
                                        adcs,neighbour);

  // add to container
  clusCont->insert(newCluster,nearestChan);

  return StatusCode::SUCCESS;
}


unsigned int RawBankToSTClusterAlg::mean(const std::vector<SiADCWord>& adcValues) const
{
 
  double sum = 0;
  for (unsigned int i = 0; i < adcValues.size() ; ++i){
     sum += adcValues[i].adc()*i;
  } // i
                                                                                        
  return (unsigned int)(sum/adcValues.size());
}


STLiteCluster RawBankToSTClusterAlg::word2LiteCluster(STClusterWord aWord,
                                                      STChannelID chan) const{
  return STLiteCluster(aWord.fracStripBits(),aWord.pseudoSizeBits(),
                       aWord.hasHighThreshold() ,chan);
}
