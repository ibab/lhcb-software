// Include files:
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
// Event
#include "Event/VPLiteCluster.h"
#include "Event/RawEvent.h"
// VPelDet
#include "VPDet/DeVP.h"
// Local
#include "PrPixelClustering.h"

using namespace LHCb;
using namespace PrPixel;

//-----------------------------------------------------------------------------
// Implementation file for class : PrPixelClustering
//
// 2014-02-26 : Kurt Rinnert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(PrPixelClustering)

//=============================================================================
// Constructor
//=============================================================================
PrPixelClustering::PrPixelClustering(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
  , m_isDebug(false)
{
  declareProperty( "RawEventLocation",  m_rawEventLocation = "",
                   "OBSOLETE. Use RawEventLocations instead" );
  declareProperty( "RawEventLocations", m_rawEventLocations,
                   "List of possible locations of the RawEvent object in the"
                   " transient store. By default it is LHCb::RawEventLocation::Other,"
                   " LHCb::RawEventLocation::Default.");
  declareProperty("ClusterLocation", m_clusterLocation = 
                  VPLiteClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
PrPixelClustering::~PrPixelClustering(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode PrPixelClustering::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
 

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

  // reset all buffers
  memset(m_buffer,0,SENSOR_PIXELS*sizeof(unsigned char));
  m_pixel_idx.clear();
  m_stack.clear();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode PrPixelClustering::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // Make new clusters container
  m_clusters = new VPLiteCluster::VPLiteClusters();
  put(m_clusters, m_clusterLocation);
  m_clusters->reserve(4000);


  // Retrieve the RawEvent:
  m_rawEvent = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    m_rawEvent = getIfExists<LHCb::RawEvent>(*p);
    if ( NULL != m_rawEvent ){
      break;
    }
  }

  if( NULL == m_rawEvent ){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }

  // Decode RawBanks
  StatusCode sc = decodeRawBanks();
  if(sc.isFailure()){
    return Error("Problems in decoding, event skipped", StatusCode::SUCCESS );
  }

  std::sort(m_clusters->begin(),m_clusters->end(),LessThanByChannel());

  return sc;

}


//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode PrPixelClustering::decodeRawBanks() 
{
  const std::vector<RawBank*>& tBanks = m_rawEvent->banks(LHCb::RawBank::VP);
  if(tBanks.size() == 0) {
    return Warning("No VP RawBanks found", StatusCode::SUCCESS);
  }
 
  const unsigned int bankVersion = (*tBanks.begin())->version();
  switch (bankVersion) {
    case 1:
      return decodeLCRawBanks(tBanks);
    case 2:
      return decodeSPRawBanks(tBanks);
    default:
      return Error("Unsupported VP RawBank version.", StatusCode::FAILURE);
  } 
}

//=============================================================================
// Decode Lite Cluster RawBanks (bank version <2)
//=============================================================================
StatusCode PrPixelClustering::decodeLCRawBanks(const std::vector<RawBank*>& tBanks) 
{
  const unsigned int nb = tBanks.size();
  for (unsigned int ib=0; ib < nb; ++ib) {
    const RawBank& bank = *tBanks[ib];
    const unsigned int module = bank.sourceID();
    const unsigned int *bank_data = bank.data();
    const unsigned int header = bank_data[0];
    const unsigned int nc = (header & HEADERNCLUMASK) >> HEADERNCLUSHIFT;
    for (unsigned int ic=0; ic < nc; ++ic) {
      const unsigned int cw     = bank_data[ic+1];
      const unsigned int pixel  = (cw & PIXELMASK)    >> PIXELSHIFT;
      const unsigned int tot    = (cw & TOTVALUEMASK) >> TOTVALUESHIFT;
      const unsigned int xfract = (cw & XFRACTMASK)   >> XFRACTSHIFT;
      const unsigned int yfract = (cw & YFRACTMASK)   >> YFRACTSHIFT;
      const unsigned int islong = (cw & ISLONGMASK)   >> ISLONGSHIFT;
      LHCb::VPChannelID channelID;
      channelID.setModule(module);
      channelID.setPixel(pixel);
      m_clusters->push_back(VPLiteCluster(channelID,tot,std::make_pair(xfract,yfract),islong));
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Decode Super Pixel RawBanks (bank version >=2)
//=============================================================================
StatusCode PrPixelClustering::decodeSPRawBanks(const std::vector<RawBank*>& tBanks) 
{
  
#define PRPIXELCLUSTERING_CLIENT 1
#include "PrPixelClustering.icpp"
#undef PRPIXELCLUSTERING_CLIENT 
  
  return StatusCode::SUCCESS;
}

