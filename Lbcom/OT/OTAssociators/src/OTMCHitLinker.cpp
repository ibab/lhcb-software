// Include files 

// from Kernel
#include "Kernel/OTChannelID.h"

// from MCEvent
#include "Event/MCOTDeposit.h"

// from Event
#include "Event/OTTime.h"

//from Linker
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

// local
#include "OTMCHitLinker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTMCHitLinker
//
// 2007-07-02 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMCHitLinker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTMCHitLinker::OTMCHitLinker( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_outputLocation(""),
    m_inputLocation(""),
    m_spillOver(false)
{
  declareProperty("OutputLocation"  , m_outputLocation = LHCb::OTTimeLocation::Default + "2MCHits"    );
  declareProperty("InputputLocation", m_inputLocation  = LHCb::OTTimeLocation::Default + "2MCDeposits");
  declareProperty("LinkSpillOver"   , m_spillOver      = false                                        );
}

//=============================================================================
// Destructor
//=============================================================================
OTMCHitLinker::~OTMCHitLinker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTMCHitLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTMCHitLinker::execute() {

  /// Get MCOTTimes
  const LHCb::MCOTTimes* mcTimes = get<LHCb::MCOTTimes>(LHCb::MCOTTimeLocation::Default);

  /// Get MCHits
  const LHCb::MCHits* mcHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );
 
  /// Create linker instance; link channelID(key) to MCHit
  LinkerWithKey<LHCb::MCHit> channelToHit(evtSvc(), msgSvc(), outputLocation());
  
  /// loop over MCOTTimes and get hits
  for (LHCb::MCOTTimes::const_iterator iMCTime = mcTimes->begin(), iMCTimeEnd = mcTimes->end(); 
       iMCTime != iMCTimeEnd; ++iMCTime) {
    const LHCb::MCOTTime* mcTime = (*iMCTime);
    const unsigned int key       = unsigned(mcTime->channel());
    OTMCHitLinker::Hits hits;
    StatusCode sc = hitsToLink(mcTime, mcHits, hits);
    if (sc.isFailure()) return Error("Failed to find hits to link to!", StatusCode::FAILURE);
    /// loop over hits
    for (OTMCHitLinker::Hits::const_iterator iHit = hits.begin(), iHitEnd = hits.end(); 
         iHit != iHitEnd;  ++iHit) channelToHit.link(key, (*iHit));
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  hitsToLink
//=============================================================================
StatusCode OTMCHitLinker::hitsToLink(const LHCb::MCOTTime* aTime, const LHCb::MCHits* mcHits, Hits& hits) const {
    
  LinkedTo<LHCb::MCOTDeposit> channelToDeposit(evtSvc(), msgSvc(), inputLocation());
  if (channelToDeposit.notFound()) Error( "Failed to find OT MCDeposits linker table", StatusCode::FAILURE );
  
  /// Get channelID(key)
  const unsigned int key = unsigned(aTime->channel());
  
  /// Get associated deposits
  typedef std::vector<LHCb::MCOTDeposit*> Range;
  Range range = channelToDeposit.range(key);
  if (range.empty()) {
    if (msgLevel(MSG::DEBUG)) debug() << "There are no deposits associated to this channel: " <<  key << endmsg;
  } else {
    for (Range::const_iterator iDeposit = range.begin(), iDepositEnd = range.end();  
         iDeposit != iDepositEnd; ++iDeposit) {
      const LHCb::MCHit* mcHit = (*iDeposit)->mcHit();
      if ((0 != mcHit) && (m_spillOver || (mcHits == mcHit->parent()))) hits.push_back(mcHit);
    }
  }
  
  return StatusCode::SUCCESS;  // must be called after all other actions
}

//=============================================================================
