// $Id: RawBankToSTClusterAlg.cpp,v 1.1.1.1 2005-12-20 12:47:27 mneedham Exp $

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
#include "Kernel/STDataFunctor.h"

#include "STDAQ/ISTReadoutTool.h"
#include "STDAQ/STTell1Board.h"
#include "STDAQ/STTell1ID.h"

#include "SiDAQ/SiADCWord.h"
#include "STDAQ/STDAQGeneral.h"
#include "STDAQ/STDAQChannelID.h"
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
GaudiAlgorithm (name , pSvcLocator){
 
 // Standard constructor, initializes variables
 declareProperty( "clusterLocation", m_clusterLocation = STClusterLocation::TTClusters);
 declareProperty("readoutTool", m_readoutToolName = "STReadoutTool");     
 declareProperty("detType", m_detType = "TT" );

}

RawBankToSTClusterAlg::~RawBankToSTClusterAlg() {
  // Destructor
};

StatusCode RawBankToSTClusterAlg::initialize() {

  // Initialization
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_clusterLocation); 
  STDetSwitch::flip(m_detType,m_readoutToolName);

  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
  
  // det element
  m_tracker = get<DeSTDetector>(DeSTDetLocation::location(m_detType));
 
  // bank type
  m_bankType =  STDAQGeneral::stringToRawBankType(m_detType);

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

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(m_bankType);  
  std::vector<RawBank* >::const_iterator iterBank;
  // loop over the banks of this type..
  for (iterBank = tBanks.begin(); iterBank != tBanks.end() ; ++iterBank){

    // get the board and data
    STTell1Board* aBoard =  m_readoutTool->findByBoardID(STTell1ID((*iterBank)->sourceID()));
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
  STChannelID nearestChan = aBoard->DAQToOffline(STDAQChannelID(aWord.channelID()));
  if (!m_tracker->isValid(nearestChan)){
    warning() << "invalid channel " << endmsg;
    return StatusCode::FAILURE;
  }

  STChannelID firstChan = STChannelID(nearestChan.station(),
                                      nearestChan.layer(),
                                      nearestChan.detRegion(), 
                                      nearestChan.sector(),
                                      nearestChan.strip() - offset);
  SmartRefVector<STDigit> digits; 
  for (unsigned int iDigit = 0; iDigit < adcValues.size() ; ++iDigit){
    STDigit* newDigit = new STDigit(adcValues[iDigit]);
    STChannelID aChannel = STChannelID(firstChan.station(), 
                                       firstChan.layer(), firstChan.detRegion(),
                                       firstChan.sector(), firstChan.strip()+iDigit);
    digits.push_back(newDigit);
  } // iDigit

  // make cluster +set things
  STCluster* newCluster = new STCluster(aWord.interStripPosition(),
                                        neighbour,
                                        aWord.hasHighThreshold());
  newCluster->setDigits(digits);

  // add to container
  clusCont->insert(newCluster,nearestChan);

  return StatusCode::SUCCESS;
}


unsigned int RawBankToSTClusterAlg::mean(const std::vector<SiADCWord>& adcValues) const {

  double sum = 0;
  for (unsigned int i = 0; i < adcValues.size() ; ++i){
     sum += adcValues[i].adc()*i;
  } // i

  return (unsigned int)(sum/adcValues.size());
}

