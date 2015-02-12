// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

// LHCb
#include "Event/MCHit.h"
#include "Event/MCParticle.h"

// Local
#include "HCHitChecker.h"

DECLARE_ALGORITHM_FACTORY(HCHitChecker)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HCHitChecker::HCHitChecker(const std::string& name,
                           ISvcLocator* pSvcLocator) :
    GaudiTupleAlg(name, pSvcLocator) {

  declareProperty("HitLocation", m_hitsLocation = LHCb::MCHitLocation::HC);
}

//=============================================================================
// Destructor
//=============================================================================
HCHitChecker::~HCHitChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HCHitChecker::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  setHistoTopDir("HC/");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCHitChecker::execute() {

  // Get the hits.
  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits>(m_hitsLocation);
  if (!hits) {
    return Error("No MCHits at " + m_hitsLocation);
  }
  unsigned int nHitsB0 = 0;
  unsigned int nHitsB1 = 0;
  unsigned int nHitsB2 = 0;
  unsigned int nHitsF1 = 0;
  unsigned int nHitsF2 = 0;
  double edepB0 = 0.;
  double edepB1 = 0.;
  double edepB2 = 0.;
  double edepF1 = 0.;
  double edepF2 = 0.;
  for (const LHCb::MCHit* hit : *hits) {
    const double z = hit->midPoint().z() / Gaudi::Units::m;
    std::string station = "";
    if (fabs(z + 7.5) < 0.2) {
      station = "B0";
      ++nHitsB0;
      edepB0 += hit->energy();
    } else if (fabs(z + 19.7) < 0.2) {
      station = "B1";
      ++nHitsB1;
      edepB1 += hit->energy();
    } else if (fabs(z + 114.) < 0.2) {
      station = "B2";
      ++nHitsB2;
      edepB2 += hit->energy();
    } else if (fabs(z - 20.) < 0.2) {
      station = "F1";
      ++nHitsF1;
      edepF1 += hit->energy();
    } else if (fabs(z - 114.) < 0.2) {
      station = "F2";
      ++nHitsF2;
      edepF2 += hit->energy();
    } else {
      warning() << "Unexpected hit position (z = " << z << " m)" << endmsg;
      continue;
    } 
    double x = hit->entry().x() / Gaudi::Units::cm;
    double y = hit->entry().y() / Gaudi::Units::cm;
    const std::string title = "EntryXYStation" + station;
    plot2D(x, y, title, title, -30., 30., -30., 30., 60, 60);
  }
  edepB0 /= Gaudi::Units::MeV;
  edepB1 /= Gaudi::Units::MeV;
  edepB2 /= Gaudi::Units::MeV;
  edepF1 /= Gaudi::Units::MeV;
  edepF2 /= Gaudi::Units::MeV;
  plot(edepB0, "EdepB0", "Energy deposit B0", 0., 400., 100); 
  plot(edepB1, "EdepB1", "Energy deposit B1", 0., 400., 100); 
  plot(edepB2, "EdepB2", "Energy deposit B2", 0., 400., 100); 
  plot(edepF1, "EdepF1", "Energy deposit F1", 0., 400., 100); 
  plot(edepF2, "EdepF2", "Energy deposit F2", 0., 400., 100); 
  counter("Number of hits B0") += nHitsB0;
  counter("Number of hits B1") += nHitsB1;
  counter("Number of hits B2") += nHitsB2;
  counter("Number of hits F1") += nHitsF1;
  counter("Number of hits F2") += nHitsF2;
  return StatusCode::SUCCESS;

}

