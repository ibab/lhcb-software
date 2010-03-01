// $Id: VeloPixRawBankToPartialCluster.cpp,v 1.1 2010-03-01 10:51:28 cocov Exp $
// Include files:
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/FastClusterContainer.h"
// Event
#include "Event/VeloPixLiteCluster.h"
#include "Event/VeloPixCluster.h"
#include "Event/RawEvent.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Local
#include "VeloPixClusterWord.h"
#include "VeloPixRawBankDecoder.h"
#include "VeloPixRawBankToPartialCluster.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixRawBankToPartialCluster
//
// 2010-02-24 : Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixRawBankToPartialCluster);

//=============================================================================
// Constructor
//=============================================================================
VeloPixRawBankToPartialCluster::VeloPixRawBankToPartialCluster(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("RawEventLocation", m_rawEventLocation =
                  LHCb::RawEventLocation::Default);
  declareProperty("ClusterLocation", m_clusterLocation = 
                  "VeloPix/MinimalClustersFromRaw");
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixRawBankToPartialCluster::~VeloPixRawBankToPartialCluster(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixRawBankToPartialCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  m_veloPixelDet = getDet<DeVeloPix>(DeVeloPixLocation::Default);
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixRawBankToPartialCluster::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Make new clusters container
  VeloPixCluster::Container* clusCont =
                      new VeloPixCluster::Container();
  put(clusCont, m_clusterLocation);
  // Retrieve the RawEvent
  if(!exist<RawEvent>(m_rawEventLocation)){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation);
  // Decode RawBanks
  StatusCode sc = decodeRawBanks(rawEvt,clusCont);
  if(sc.isFailure()){
    return Error("Problems in decoding, event skipped", sc);
  }

  return sc;
};


//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode VeloPixRawBankToPartialCluster::decodeRawBanks(RawEvent* rawEvt,
                     VeloPixCluster::Container* clusCont) const
{
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VeloPix);
  if(tBanks.size() == 0) {
    Warning("No VeloPix RawBanks found");
    return StatusCode::SUCCESS;
  }
  // Loop over VeloPix RawBanks  
  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for(iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    // Get sensor number
    unsigned int sensor = (*iterBank)->sourceID();
    // Decoder
    VeloPixRawBankDecoder<VeloPixPatternWord> decoderPattern((*iterBank)->data());
    VeloPixRawBankDecoder<VeloPixClusterWord> decoderCluster((*iterBank)->data());
    // Get version of the bank
    unsigned int bankVersion = (*iterBank)->version();
    debug() << "Decoding bank version " << bankVersion << endmsg;
    // Decode lite clusters
    VeloPixRawBankDecoder<VeloPixClusterWord>::pos_iterator iterClu =
                                                            decoderCluster.posBegin();
    VeloPixRawBankDecoder<VeloPixPatternWord>::pos_iterator iterPat =
                                                            decoderPattern.posBegin();
    for(;iterClu != decoderCluster.posEnd(); ++iterClu) {
      createPartialCluster(sensor,*iterClu,*iterPat,clusCont);
      if (iterPat != decoderPattern.posEnd()) ++iterPat;
    }
  } 

  return StatusCode::SUCCESS;
}


//=============================================================================
// Create liteCluster
//=============================================================================
void VeloPixRawBankToPartialCluster::createPartialCluster(
                     unsigned int sensor,
                     VeloPixClusterWord aWord,
                     VeloPixPatternWord aPattern,
                     VeloPixCluster::Container* clusCont) const 
{
  LHCb::VeloPixChannelID achan;
  achan.setSensor(sensor);
  achan.setPixel(aWord.pixel());
  std::pair<unsigned int,unsigned int> xyFract;
  xyFract.first  = aWord.xFract();
  xyFract.second = aWord.yFract();
  const VeloPixLiteCluster newLiteCluster(achan,aWord.totValue(),xyFract,
                                      aWord.hasIsLong());    
  LHCb::VeloPixChannelID achan_central;
  achan_central.setSensor(sensor);
  achan_central.setPixel( aPattern.pixel());
  // achan_central.set();

  const std::vector< std::pair< LHCb::VeloPixChannelID, int > >  vectorCHID;

  VeloPixCluster* newCluster = new VeloPixCluster(newLiteCluster,vectorCHID);
  //newCluster.setLCluster(newLiteCluster);
  clusCont->insert(newCluster,achan_central);
  //if (achan_central.pixel() == 23 )always()<<"found the 23 "<<newLiteCluster.channelID().pixel()<<" "<<achan_central.channelID()<<endmsg;
  
  if (newLiteCluster.channelID().pixel()!=newCluster->channelID().pixel())info()<<"Barycenter channelID different from central channelID"<<endmsg;
  return;
}


//============================================================================
StatusCode VeloPixRawBankToPartialCluster::finalize() {

  return GaudiAlgorithm::finalize();

}
