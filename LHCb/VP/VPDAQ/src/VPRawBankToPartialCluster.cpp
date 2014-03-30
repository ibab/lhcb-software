// Include files:
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
// Event
#include "Event/VPCluster.h"
#include "Event/RawEvent.h"
// VPDet
#include "VPDet/DeVP.h"
// Local
#include "VPClusterWord.h"
#include "VPRawBankDecoder.h"
#include "VPRawBankToPartialCluster.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPRawBankToPartialCluster
//
// 2010-02-24 : Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPRawBankToPartialCluster)

//=============================================================================
// Constructor
//=============================================================================
VPRawBankToPartialCluster::VPRawBankToPartialCluster(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : Decoder::AlgBase(name, pSvcLocator)
  , m_isDebug(false)
  , m_det(NULL)
{
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Other, LHCb::RawEventLocation::Default};
  initRawEventSearch();
    
  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
VPRawBankToPartialCluster::~VPRawBankToPartialCluster(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPRawBankToPartialCluster::initialize() {
  StatusCode sc = Decoder::AlgBase::initialize();
  if (sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if (m_isDebug) debug() << "==> Initialise" << endmsg;
  m_det = getDet<DeVP>(DeVPLocation::Default);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPRawBankToPartialCluster::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // Make new clusters container
  LHCb::VPClusters* clusters = new LHCb::VPClusters();
  put(clusters, m_clusterLocation);

  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvent = findFirstRawEvent();
  
  if (NULL == rawEvent) {
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }
  
  // Decode RawBanks
  StatusCode sc = decodeRawBanks(rawEvent,clusters);
  if (sc.isFailure()) {
    return Error("Problems in decoding, event skipped", sc);
  }

  return sc;
}

//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode VPRawBankToPartialCluster::decodeRawBanks(RawEvent* rawEvt,
                     LHCb::VPClusters* clusters) const
{
  
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VP);
  if (tBanks.size() == 0) {
    return Warning("No VP RawBanks found",  StatusCode::SUCCESS);
  }
  // Loop over VP RawBanks  
  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for (iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    // Get sensor number
    unsigned int sensor = (*iterBank)->sourceID();
    // Decoder
    VPRawBankDecoder<VPPatternWord> decoderPattern((*iterBank)->data());
    VPRawBankDecoder<VPClusterWord> decoderCluster((*iterBank)->data());
    // Get version of the bank
    unsigned int bankVersion = (*iterBank)->version();
    if (m_isDebug) debug() << "Decoding bank version " << bankVersion << endmsg;
    // Decode lite clusters
    VPRawBankDecoder<VPClusterWord>::pos_iterator iterClu =
                                                            decoderCluster.posBegin();
    VPRawBankDecoder<VPPatternWord>::pos_iterator iterPat =
                                                            decoderPattern.posBegin();
    std::vector<int> testDouybleId;
    for(;iterClu != decoderCluster.posEnd(); ++iterClu) {
      // Test that the channel ID was not duplicated (should be the case but there might be a bug in digitization
      // producing it)
      int pixelClu = (*iterClu).pixel();
      bool isUsed = false;
      for (std::vector<int>::iterator itInt = testDouybleId.begin() ;  itInt!= testDouybleId.end() ; ++itInt) {
        if (pixelClu==(*itInt)) isUsed=true;
      }
      if (isUsed) {
        if (iterPat != decoderPattern.posEnd()) ++iterPat;
        Warning("Duplicated channelID there should be a bug in the digitization of VP").ignore();
        continue;
      }
      testDouybleId.push_back(pixelClu);
      createPartialCluster(sensor,*iterClu,clusters);
      if (iterPat != decoderPattern.posEnd()) ++iterPat;
    }
  } 
  return StatusCode::SUCCESS;
}


//=============================================================================
// Create cluster
//=============================================================================
void VPRawBankToPartialCluster::createPartialCluster(unsigned int sensor,
                     VPClusterWord word,
                     LHCb::VPClusters* clusters) const {
  LHCb::VPChannelID channel;
  channel.setModule(sensor);
  channel.setPixel(word.pixel());
  std::pair<double, double> xyFract;
  xyFract.first  = word.xFract() / 8.;
  xyFract.second = word.yFract() / 8.;
  std::vector<LHCb::VPChannelID> pixels;
  pixels.push_back(channel);
  VPCluster* newCluster = new VPCluster(xyFract, 0., 0., 0., pixels);
  clusters->insert(newCluster, channel);
}
