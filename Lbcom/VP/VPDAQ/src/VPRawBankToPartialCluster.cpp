// Include files:
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
#include "Kernel/FastClusterContainer.h"
// Event
#include "Event/VPLiteCluster.h"
#include "Event/VPCluster.h"
#include "Event/RawEvent.h"
// VPelDet
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
  : GaudiAlgorithm(name, pSvcLocator)
  , m_vPelDet(NULL)
{
  declareProperty( "RawEventLocation",  m_rawEventLocation = "",
                   "OBSOLETE. Use RawEventLocations instead" );
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");

  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::VPClusterLocation);
}

//=============================================================================
// Destructor
//=============================================================================
VPRawBankToPartialCluster::~VPRawBankToPartialCluster(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPRawBankToPartialCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  m_vPelDet = getDet<DeVP>(DeVPLocation::Default);
  // Initialise the RawEvent locations
  bool usingDefaultLocation = m_rawEventLocations.empty() && m_rawEventLocation.empty();
  if (! m_rawEventLocation.empty()) {
    warning() << "The RawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }
  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path 
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Other);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPRawBankToPartialCluster::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // Make new clusters container
  VPCluster::Container* clusCont =
                      new VPCluster::Container();
  put(clusCont, m_clusterLocation);

  // Retrieve the RawEvent:
  LHCb::RawEvent* rawEvent = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    rawEvent = getIfExists<LHCb::RawEvent>(*p);
    if ( NULL != rawEvent ){
      break;
    }
  }

  if( NULL == rawEvent ){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }
  
  // Decode RawBanks
  StatusCode sc = decodeRawBanks(rawEvent,clusCont);
  if(sc.isFailure()){
    return Error("Problems in decoding, event skipped", sc);
  }

  return sc;
}

//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode VPRawBankToPartialCluster::decodeRawBanks(RawEvent* rawEvt,
                     VPCluster::Container* clusCont) const
{
  
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VP);
  if(tBanks.size() == 0) {
    Warning("No VP RawBanks found");
    return StatusCode::SUCCESS;
  }
  // Loop over VP RawBanks  
  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for(iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    // Get sensor number
    unsigned int sensor = (*iterBank)->sourceID();
    // Decoder
    VPRawBankDecoder<VPPatternWord> decoderPattern((*iterBank)->data());
    VPRawBankDecoder<VPClusterWord> decoderCluster((*iterBank)->data());
    // Get version of the bank
    unsigned int bankVersion = (*iterBank)->version();
    if(m_isDebug) debug() << "Decoding bank version " << bankVersion << endmsg;
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
      for (std::vector<int>::iterator itInt = testDouybleId.begin() ;  itInt!= testDouybleId.end() ; ++itInt){
        if(pixelClu==(*itInt))isUsed=true;
      }
      if (isUsed){
        if (iterPat != decoderPattern.posEnd()) ++iterPat;
        Warning("Duplicated channelID there should be a bug in the digitization of VP");
        continue;
      }
      testDouybleId.push_back(pixelClu);
      createPartialCluster(sensor,*iterClu,*iterPat,clusCont);
      if (iterPat != decoderPattern.posEnd()) ++iterPat;
    }
  } 

  return StatusCode::SUCCESS;
}


//=============================================================================
// Create liteCluster
//=============================================================================
void VPRawBankToPartialCluster::createPartialCluster(
                     unsigned int sensor,
                     VPClusterWord aWord,
                     VPPatternWord aPattern,
                     VPCluster::Container* clusCont) const 
{
  LHCb::VPChannelID achan;
  achan.setSensor(sensor);

  achan.setPixel(aWord.pixel());
  std::pair<unsigned int,unsigned int> xyFract;
  xyFract.first  = aWord.xFract();
  xyFract.second = aWord.yFract();
  const VPLiteCluster newLiteCluster(achan,aWord.totValue(),xyFract,
                                      aWord.hasIsLong()); 
  LHCb::VPChannelID achan_central;
  achan_central.setSensor(sensor);
  achan_central.setPixel( aPattern.pixel());

  const std::vector< std::pair< LHCb::VPChannelID, int > >  vectorCHID;

  VPCluster* newCluster = new VPCluster(newLiteCluster,vectorCHID);

  clusCont->insert(newCluster,achan_central);
  
  if (achan_central.pixel()!=achan.pixel())info()<<"Barycenter channelID different from central channelID"<<endmsg;
  return;
}


//============================================================================
StatusCode VPRawBankToPartialCluster::finalize() {

  return GaudiAlgorithm::finalize();

}
