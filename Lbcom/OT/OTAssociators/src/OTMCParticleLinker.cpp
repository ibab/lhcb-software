// Include files 

// from MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTTime.h"

//from Event
#include "Event/OTTime.h"

// from Kernel
#include "Kernel/OTChannelID.h"

// from Linker
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"

// local
#include "OTMCParticleLinker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTMCParticleLinker
//
// 2007-07-02 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMCParticleLinker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTMCParticleLinker::OTMCParticleLinker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_outputLocation(""),
    m_inputLocation("")
{
  declareProperty("OutputLocation",   m_outputLocation = LHCb::OTTimeLocation::Default          );
  declareProperty("InputputLocation", m_inputLocation  = LHCb::OTTimeLocation::Default+"2MCHits");

}
//=============================================================================
// Destructor
//=============================================================================
OTMCParticleLinker::~OTMCParticleLinker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTMCParticleLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTMCParticleLinker::execute() {
  /// Get MCOTTimes
  const LHCb::MCOTTimes* mcTimes = get<LHCb::MCOTTimes>(LHCb::MCOTTimeLocation::Default);

  /// Create linker instance; link channelID(key) to MCParticle
  LinkerWithKey<LHCb::MCParticle> channelToParticle(evtSvc(), msgSvc(), outputLocation());

  for (LHCb::MCOTTimes::const_iterator iMCTime = mcTimes->begin(), iMCTimeEnd = mcTimes->end(); 
       iMCTime != iMCTimeEnd; ++iMCTime) {
    const LHCb::MCOTTime* mcTime = (*iMCTime);
    const unsigned int key = unsigned(mcTime->channel());
    OTMCParticleLinker::Particles particles;
    StatusCode sc = particlesToLink(mcTime, particles);
    if (sc.isFailure()) return Error("Failed to find particles to link to!", StatusCode::FAILURE);
    for (OTMCParticleLinker::Particles::const_iterator iParticle = particles.begin(), iParticleEnd = particles.end(); 
         iParticle != iParticleEnd; ++iParticle) channelToParticle.link(key, (*iParticle));
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  particlesToLink
//=============================================================================
StatusCode OTMCParticleLinker::particlesToLink(const LHCb::MCOTTime* aTime, Particles& particles) const {
  
  LinkedTo<LHCb::MCHit> channelToHit(evtSvc(), msgSvc(), inputLocation());
  if (channelToHit.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE );
   
  const unsigned int key = unsigned(aTime->channel());
  
  /// Get hits
  typedef std::vector<LHCb::MCHit*> Range;
  Range range = channelToHit.range(key);
  for (Range::const_iterator iHit = range.begin(), iHitEnd = range.end(); iHit != iHitEnd; ++iHit) {  
    const LHCb::MCParticle* aParticle = (*iHit)->mcParticle(); 
    if (aParticle) particles.push_back(aParticle);
  }
  
  return StatusCode::SUCCESS;  // must be called after all other actions
}

//=============================================================================
