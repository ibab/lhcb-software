// $Id: RawBankToSTLiteClusterAlg.cpp,v 1.11 2007-11-16 16:43:36 mneedham Exp $


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
#include "STDAQGeneral.h"

#include "Kernel/STDAQDefinitions.h"

#include "Kernel/STDecoder.h"

#include "Kernel/STDetSwitch.h"
#include "Kernel/STDataFunctor.h"

#include "Kernel/FastClusterContainer.h"

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
  declareProperty("rawEventLocation",m_rawEventLocation = RawEventLocation::Default); 
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

    int version = (*iterBank)->version();

    // read in the first half of the bank
    STDecoder::pos_iterator iterDecoder = decoder.posBegin();
    for ( ;iterDecoder != decoder.posEnd(); ++iterDecoder){
      
      STClusterWord aWord = *iterDecoder;
      unsigned int fracStrip = aWord.fracStripBits();
      
      //      unsigned int pseudoSize = aWord.pseudoSizeBits(); //no change
      //      bool secondThres = aWord.hasHighThreshold(); //no change
      STChannelID chan =  aBoard->DAQToOffline(aWord.channelID(),fracStrip,
                                               version);

      STLiteCluster liteCluster(fracStrip,
                                aWord.pseudoSizeBits(),
                                aWord.hasHighThreshold(),
                                chan);

      fCont->push_back(liteCluster); 
    } //decoder
      
  } // iterBank

  // sort 
  std::sort(fCont->begin(),fCont->end(), Less_by_Channel());
 
  return StatusCode::SUCCESS;

}




