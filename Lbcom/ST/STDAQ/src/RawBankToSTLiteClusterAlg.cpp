// $Id: RawBankToSTLiteClusterAlg.cpp,v 1.1 2006-02-10 08:59:31 mneedham Exp $


#include <algorithm>
#include <vector>


// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBankToSTLiteClusterAlg.h"
#include "ISTReadoutTool.h"

// Event
#include "Event/RawEvent.h"
#include "Event/ByteStream.h"
#include "Event/STLiteCluster.h"


#include "STTell1Board.h"
#include "STTell1ID.h"
#include "STDAQGeneral.h"
#include "SiDAQ/SiHeaderWord.h"

#include "Kernel/STDetSwitch.h"
#include "Kernel/STDataFunctor.h"

#include "STDet/DeSTDetector.h"

// GSL
#include "gsl/gsl_math.h"

#include "Kernel/FastClusterContainer.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTLiteClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawBankToSTLiteClusterAlg>          s_factory ;
const IAlgFactory& RawBankToSTLiteClusterAlgFactory = s_factory ;

RawBankToSTLiteClusterAlg::RawBankToSTLiteClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator){
 
 // Standard constructor, initializes variables
  declareProperty( "clusterLocation", m_clusterLocation = STLiteClusterLocation::TTClusters); 
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
  RawEvent* rawEvt = get<RawEvent>(RawEventLocation::Default );

  // make a new digits container
  //  STClusters* clusCont = new STClusters();
  //clusCont->reserve(2000);

  // decode banks
  StatusCode sc = decodeBanks(rawEvt);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  // sort
 
  // store
  //put(clusCont, m_clusterLocation);

  return sc;
};


StatusCode RawBankToSTLiteClusterAlg::decodeBanks(RawEvent* rawEvt) const{
 
  typedef std::pair< std::pair<unsigned int, ByteStream >, STTell1Board*> Data;
  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());
  std::vector<Data> tmpCont = std::vector<Data>();
  tmpCont.reserve(tBanks.size());
  unsigned int i = 0;
  unsigned int nClus = 0;

  // loop over the banks of this type..
  std::vector<RawBank* >::const_iterator iterBank =  tBanks.begin();
  for (; iterBank != tBanks.end() ; ++iterBank, ++i){

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

    nClus+= aHeader.nClusters();
     
    tmpCont.push_back(std::make_pair(
                      std::make_pair(aHeader.nClusters(),stream),aBoard));

  } // iterBank

  STLiteCluster::FastContainer* fCont = new STLiteCluster::FastContainer();
  fCont->resize(nClus);  

  i = 0;
  for (std::vector<Data>::iterator iter = tmpCont.begin(); 
       iter !=  tmpCont.end(); ++iter){

    // read in the first half of the bank
    STClusterWord aWord; 
        
    for (unsigned int iW = 0; iW < iter->first.first; ++iW, ++i){
      iter->first.second >> aWord;
      STLiteCluster liteCluster(aWord.fracStripBits(),
                                aWord.pseudoSizeBits(),
                                aWord.hasHighThreshold(),
                                iter->second->DAQToOffline(aWord.channelID()));
      fCont->at(i) = liteCluster;
    } // iW
  } // iterData

  put(fCont, m_clusterLocation);
  
  return StatusCode::SUCCESS;

}

