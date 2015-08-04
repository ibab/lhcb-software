//
// This File contains the implementation of the MCParticle2MCHitAlg interface
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Authors: Rutger van der Eijk
//   Created: 3-7-2002

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedFrom.h"

// local
#include "TrackerMCParticle2MCHitAlg.h"

// Boost
#include <boost/assign/list_of.hpp>

DECLARE_ALGORITHM_FACTORY( TrackerMCParticle2MCHitAlg )

using namespace LHCb;

TrackerMCParticle2MCHitAlg::TrackerMCParticle2MCHitAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator):
  GaudiAlgorithm ( name , pSvcLocator )
{
  // Standard constructor, initializes variables
  
  declareProperty( "OutputData",  m_outputData = 
                   "/Event/MC/Particles2MCTrackerHits" );
  std::vector<std::string> tmp = boost::assign::list_of("Velo")("IT")("TT")("OT")("Muon");
  declareProperty("Detectors", m_dets = tmp);
}

TrackerMCParticle2MCHitAlg::~TrackerMCParticle2MCHitAlg()
{
  // Destructor
}


StatusCode TrackerMCParticle2MCHitAlg::execute()
{
  // Main execution

  // get the MCParticles
  MCParticles* particles = get<MCParticles>(MCParticleLocation::Default );

  // save some typing
  typedef LinkedFrom<LHCb::MCHit,LHCb::MCParticle> HitLinks;

  // make the output table
  LinkerWithKey<MCParticle,MCHit> myLink( eventSvc(), msgSvc(), m_outputData );

  // get the sub-detector linkers
  std::vector<HitLinks> links; 
  for( unsigned int idet = 0 ; idet< m_dets.size(); ++idet ) {
    std::string linkPath = MCParticleLocation::Default + "2MC" + m_dets[idet] + "Hits";
    HitLinks link( evtSvc(),msgSvc(),linkPath);
    links.push_back(link); 
    if (link.notFound() == true)
      return Warning("Failed to find linker table", StatusCode::FAILURE);
  } // i

  // loop over MCParticles
  MCParticles::const_iterator iterP = particles->begin();
  for( ; iterP != particles->end(); ++iterP) {
    std::vector<HitLinks>::iterator iterDet = links.begin();
    for (; iterDet != links.end(); ++iterDet){
      MCHit* aHit = iterDet->first(*iterP);
      while( 0 != aHit ) {
	myLink.link(aHit,*iterP);
        aHit = iterDet->next();
      } // aHit
    } // iterDet
  } // iterP
  
  if( msgLevel(MSG::DEBUG) ) debug() << "Linker table stored at " << m_outputData << endmsg;

  return StatusCode::SUCCESS;
}
