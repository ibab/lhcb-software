// Gaudi
#include "GaudiKernel/AlgFactory.h"
// LHCb
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "Event/VPCluster.h"
// Local
#include "PrPixelMonitor.h"

DECLARE_ALGORITHM_FACTORY(PrPixelMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrPixelMonitor::PrPixelMonitor(const std::string &name,
                               ISvcLocator *pSvcLocator)
    : GaudiTupleAlg(name, pSvcLocator),
      m_hitManager(NULL) {

}

//=============================================================================
// Destructor
//=============================================================================
PrPixelMonitor::~PrPixelMonitor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrPixelMonitor::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;
  m_hitManager = tool<PrPixelHitManager>("PrPixelHitManager");
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrPixelMonitor::execute() {

  const unsigned int firstModule = m_hitManager->firstModule();
  const unsigned int lastModule = m_hitManager->lastModule();
  for (unsigned int i = firstModule; i < lastModule; ++i) {
    for (auto hit : m_hitManager->hits(i)) {
      const float x = hit->x();
      const float y = hit->y();
      const float z = hit->z();
      const float r = sqrt(x * x + y * y);
      if (!hit->isUsed()) {
        plot3D(x, y, z, "UnusedHits3D", "Distribution of Unused Hits", 
               -50., 50., -50., 50., -500., 800., 100, 100, 200);
        plot2D(r, z, "UnusedHitsRZ", "Distribution of Unused Hits",
               0., 60., -500., 800., 100, 100);
        plot2D(x, y, "UnusedHitsXY", "Distribution of Unused Hits", 
               -50., 50., -50., 50., 100, 100);
      }
      plot3D(x, y, z, "Hits3D", "Distribution of Hits", 
             -50., 50., -50., 50., -500., 800., 100, 100, 200);
      plot2D(r, z, "HitsRZ", "Distribution of Hits", 
             0., 60., -500., 800., 100, 100);
      plot2D(x, y, "HitsXY", "Distribution of Hits", 
             -50., 50., -50., 50., 100, 100);
    }
  }

  const unsigned int nHits = m_hitManager->nbHits();
  const unsigned int nHitsUsed = m_hitManager->nbHitsUsed();
  plot(nHits, "HitsPerEvent", "Number of hits per event", 0.0, 8000.0, 80);
  if (nHits > 0) {
    plot(100. * nHitsUsed / nHits, "PercentUsedHitsPerEvent",
         "Percentage of hits assigned to tracks", 0.0, 100.0, 100);
  }

  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>(LHCb::TrackLocation::Velo);
  if (!tracks) {
    warning() << "No tracks in " << LHCb::TrackLocation::Velo << endmsg;
    return StatusCode::FAILURE;
  }
  plot(tracks->size(), "TracksPerEvent", "Number of tracks per event", 
       0., 800., 80);
  LHCb::VPClusters* clusters = getIfExists<LHCb::VPClusters>(LHCb::VPClusterLocation::Default);
  if (!clusters) {
    warning() << "No clusters in " << LHCb::VPClusterLocation::Default << endmsg;
  }

  unsigned int nFwd = 0;
  unsigned int nBwd = 0;
  for (LHCb::Track* track : *tracks) {
    const bool bwd = track->checkFlag(LHCb::Track::Backward);
    const unsigned int nHitsPerTrack = track->lhcbIDs().size();
    const float chi2 = track->chi2PerDoF();
    const float eta = track->pseudoRapidity();
    const float phi = track->phi() / Gaudi::Units::degree;
    if (bwd) {
      ++nBwd;
      plot(nHitsPerTrack, "BwdHitsPerTrack",
           "Number of hits per backward track", 0.5, 40.5, 40);
      plot(chi2, "BwdChi2", "Chi2/DoF of backward tracks", 0., 10., 50);
      plot(eta, "BwdEta", "Pseudorapidity of backward tracks", 1., 6., 50);
      plot(phi, "BwdPhi", "Phi-angle of backward tracks", -180., 180., 60);
      plot2D(eta, nHitsPerTrack, "BwdHitsPerTrackVsEta",
             "Hits/track vs pseudorapidity of backward tracks", 
             1., 6., 0.5, 15.5, 50, 15);
      plot2D(eta, chi2, "BwdChi2VsEta",
             "Chi2/DoF vs pseudorapidity of backward tracks",
             1., 6., 0., 10., 50, 20);
      plot2D(nHitsPerTrack, chi2, "BwdChi2VsHitsPerTrack",
             "Chi2/DoF vs hits/backward track", 0.5, 15.5, 0., 10., 15, 20);
    } else {
      ++nFwd;
      plot(nHitsPerTrack, "FwdHitsPerTrack",
           "Number of hits per forward track", 0.5, 40.5, 40);
      plot(chi2, "FwdChi2", "Chi2/DoF of forward tracks", 0., 10., 50);
      plot(eta, "FwdEta", "Pseudorapidity of forward tracks", 1., 6., 50);
      plot(phi, "FwdPhi", "Phi-angle of forward tracks", -180., 180., 60);
      plot2D(eta, nHitsPerTrack, "FwdHitsPerTrackVsEta",
             "Hits/track vs pseudorapidity of forward tracks", 
             1., 6., 0.5, 15.5, 50, 15);
      plot2D(eta, chi2, "FwdChi2VsEta",
             "Chi2/DoF vs pseudorapidity of forward tracks",
             1., 6., 0., 10., 50, 20);
      plot2D(nHitsPerTrack, chi2, "FwdChi2VsHitsPerTrack",
             "Chi2/DoF vs hits/forward track", 0.5, 15.5, 0., 10., 15, 20);
    }
    if (!clusters) continue;
    // Calculate radius at first and last hit 
    // (assume that hits are sorted by module)
    const LHCb::VPChannelID id0 = track->lhcbIDs().front().vpID();
    const LHCb::VPChannelID id1 = track->lhcbIDs().back().vpID();
    const LHCb::VPCluster* cluster0 = clusters->object(id0);
    if (!cluster0) continue;
    const LHCb::VPCluster* cluster1 = clusters->object(id1);
    if (!cluster1) continue;
    const float x0 = cluster0->x();
    const float y0 = cluster0->y();
    const float r0 = sqrt(x0 * x0 + y0 * y0);
    const float x1 = cluster1->x();
    const float y1 = cluster1->y();
    const float r1 = sqrt(x1 * x1 + y1 * y1);
    const float minR = r0 > r1 ? r1 : r0;
    const float maxR = r0 > r1 ? r0 : r1;
    plot(minR, "MinHitRadius", "Smallest hit radius [mm]", 0., 50., 100);
    plot(maxR, "MaxHitRadius", "Largest hit radius [mm]", 0., 50., 100);
  }
  plot(nFwd, "FwdTracksPerEvent", "Number of forward tracks per event", 
       0., 400., 40);
  plot(nBwd, "BwdTracksPerEvent", "Number of backward tracks per event", 
       0., 400., 40);
  return StatusCode::SUCCESS;

}

