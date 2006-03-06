// $Id: RawBankToSTLiteClusterAlg.cpp,v 1.3 2006-03-06 12:29:41 mneedham Exp $


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
#include "Event/STLiteCluster.h"


#include "STTell1Board.h"
#include "STTell1ID.h"
#include "STDAQGeneral.h"

//#include "SiDAQ/SiHeaderWord.h"
#include "STDecoder.h"

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


  // decode banks
  StatusCode sc = decodeBanks(rawEvt);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  
  return sc;
};


StatusCode RawBankToSTLiteClusterAlg::decodeBanks(RawEvent* rawEvt) const{
 
  typedef std::pair< STDecoder , STTell1Board* > Data;
  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType());
  std::vector<Data> tmpCont = std::vector<Data>();
  tmpCont.reserve(tBanks.size());
  unsigned int nClus = 0;

  // loop over the banks of this type..
  std::vector<RawBank* >::const_iterator iterBank =  tBanks.begin();
  for (; iterBank != tBanks.end() ; ++iterBank){

    // get the board and data
    STTell1Board* aBoard = readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID()));
 
    // make a SmartBank of shorts...
    STDecoder decoder((*iterBank)->data());
   
    // get number of clusters..

    if (decoder.hasError() == true){
      warning() << "bank has errors - skip event" << endmsg;
      return StatusCode::FAILURE;
    }
				     
    nClus+= decoder.nClusters();
     
    tmpCont.push_back(std::make_pair(decoder,aBoard));

  } // iterBank

  STLiteCluster::FastContainer* fCont = new STLiteCluster::FastContainer();
  fCont->resize(nClus);  

  unsigned int i = 0;
  for (std::vector<Data>::iterator iter = tmpCont.begin(); 
       iter !=  tmpCont.end(); ++iter){

    // read in the first half of the bank
    STDecoder::pos_iterator iterDecoder = iter->first.posBegin();
    for ( ;iterDecoder != iter->first.posEnd(); ++iterDecoder){
     
      STClusterWord aWord = *iterDecoder; 
      STLiteCluster liteCluster(aWord.fracStripBits(),
                                aWord.pseudoSizeBits(),
                                aWord.hasHighThreshold(),
                                iter->second->DAQToOffline(aWord.channelID()));
      fCont->at(i) = liteCluster; 
     
    } //iter
  } // iterData

  // sort
  std::sort(fCont->begin(),fCont->end(), Less_by_Channel());

  put(fCont, m_clusterLocation);
  
  return StatusCode::SUCCESS;

}

