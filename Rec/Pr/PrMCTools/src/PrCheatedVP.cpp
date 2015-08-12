// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCTrackInfo.h"
// Event/TrackEvent
#include "Event/Track.h"
#include "Event/StateParameters.h"
// Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// Local
#include "PrFitPolinomial.h"
#include "PrCheatedVP.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY(PrCheatedVP)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrCheatedVP::PrCheatedVP(const std::string& name,
                         ISvcLocator* pSvcLocator)
    : GaudiAlgorithm (name, pSvcLocator) {

  declareProperty("UseMCHits", m_useMCHits = false);
}

//=============================================================================
// Destructor
//=============================================================================
PrCheatedVP::~PrCheatedVP() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrCheatedVP::initialize() {

  // Initialise the base class.
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrCheatedVP::execute() {

  LHCb::Tracks* tracks = new LHCb::Tracks();
  put(tracks, LHCb::TrackLocation::Velo);

  // Get the MC particles.  
  const LHCb::MCParticles* particles = getIfExists<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  if (!particles) {
    return Error("No MC particles in " + LHCb::MCParticleLocation::Default);
  }
  // Get the association table between MC particles and clusters.
  LinkedFrom<LHCb::VPCluster, MCParticle> link(evtSvc(), msgSvc(), LHCb::VPClusterLocation::Default);
  // Get the track info table.
  MCTrackInfo trackInfo(evtSvc(), msgSvc());
  // Get the MC hits (if needed).
  LHCb::MCHits* hits = NULL;
  if (m_useMCHits) {
    hits = getIfExists<LHCb::MCHits>(LHCb::MCHitLocation::VP);
    if (!hits) {
      return Error("No MC hits in " + LHCb::MCHitLocation::VP);
    }
  } 
   
  for (const LHCb::MCParticle* particle : *particles) {
    // Skip particles without track info.
    if (0 == trackInfo.fullInfo(particle)) continue;
    // Skip particles not linked to a VELO track.
    if (!trackInfo.hasVelo(particle)) continue;
    // Skip electrons.
    if (abs(particle->particleID().pid()) == 11) continue;

    std::vector<Gaudi::XYZPoint> points;  
    LHCb::Track* track = new LHCb::Track;
    
    if (m_useMCHits) {
      std::vector<int> range = link.keyRange(particle);
      for (auto it = range.cbegin(), end = range.cend(); it != end; ++it) {
        LHCb::VPChannelID vpId(*it);
        track->addToLhcbIDs(LHCb::LHCbID(vpId));
      }
      for (const LHCb::MCHit* hit : *hits) {
        if (hit->mcParticle() == particle) {
          points.push_back(hit->midPoint());
        }
      }
    } else {
      // Get the clusters associated to the particle.
      const LHCb::VPCluster* cluster = link.first(particle);
      while (cluster) {
        LHCb::LHCbID id = LHCb::LHCbID(cluster->channelID());
        track->addToLhcbIDs(id);
        Gaudi::XYZPoint point(cluster->x(), cluster->y(), cluster->z());
        points.push_back(point);
        cluster = link.next();
      }
    }
    // Make a straight-line fit of the track. 
    double m_zVelo = 0.;
    PrFitPolinomial fitX(1);
    PrFitPolinomial fitY(1);
    for (auto it = points.cbegin(), end = points.cend(); it != end; ++it) {
      const double dz = (*it).z() - m_zVelo;
      fitX.fill((*it).x(), dz);
      fitY.fill((*it).y(), dz);
    }
    fitX.solve();
    fitY.solve();
    
    LHCb::State state;
    state.setLocation(LHCb::State::ClosestToBeam);
    state.setState(fitX.param(0), fitY.param(0), m_zVelo, 
                   fitX.param(1), fitY.param(1), 0.);
    track->addToStates(state);
    if (0 > particle->momentum().z()) {
      track->setFlag(LHCb::Track::Backward, true);
      // Cut out backwards tracks.
      // delete track;
      // continue; 
    }
    track->setType(LHCb::Track::Velo);
    tracks->insert(track);
  }
  
  return StatusCode::SUCCESS;
}

