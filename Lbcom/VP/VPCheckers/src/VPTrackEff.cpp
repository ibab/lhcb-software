// LHCb
#include "Linker/LinkerTool.h"

#include "Event/Track.h"
#include "Event/MCHit.h"

// Local
#include "VPTrackEff.h"

DECLARE_ALGORITHM_FACTORY(VPTrackEff)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPTrackEff::VPTrackEff(const std::string& name,
                       ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg(name, pSvcLocator) {

  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Velo);
  declareProperty("MCHitLocation", m_mchitLocation = LHCb::MCHitLocation::VP);
}

//=============================================================================
// Destructor
//=============================================================================
VPTrackEff::~VPTrackEff() {}

//=============================================================================
// Initialization. 
//=============================================================================
StatusCode VPTrackEff::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()) return sc;

  m_det = getDet<DeVP>(DeVPLocation::Default);

  setHistoTopDir("VP/");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode VPTrackEff::execute() {

  LinkerTool<LHCb::Track, LHCb::MCParticle> linker(evtSvc(), m_trackLocation);
  LinkerTool<LHCb::Track, LHCb::MCParticle>::DirectType* directLinker = linker.direct();
  LinkerTool<LHCb::Track, LHCb::MCParticle>::InverseType* inverseLinker = linker.inverse();

  // Pick up the MC particles.
  const LHCb::MCParticles* particles = getIfExists<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
  if (!particles) {
    return Error("No particles in " + LHCb::MCParticleLocation::Default, StatusCode::SUCCESS);
  }
  // Pick up the MC hits.
  const LHCb::MCHits* hits = getIfExists<LHCb::MCHits*>(m_mchitLocation);
  if (!hits) {
    return Error("No hits in " + m_mchitLocation, StatusCode::SUCCESS);
  }

  // Create a map from all MCParticles to MCHits
  std::map<const LHCb::MCParticle *, std::vector<const LHCb::MCHit*> > mchitmap;
  for (const LHCb::MCParticle* particle : *particles) {
    mchitmap[particle] = std::vector<const LHCb::MCHit*>();
  }
  
  // First collect hits
  for (const LHCb::MCHit* hit : *hits) {
    if (!hit) {
      warning() << "MCHit shouldn't be NULL!" << endmsg;
      continue;
    }
    // Skip hits not linked to a particle.
    if (!hit->mcParticle()) continue;
    const double z = hit->entry().z();
    if (z < -300 * Gaudi::Units::mm || z < 800 * Gaudi::Units::mm) {
      continue;
    }
    bool inAA(false);
    for (const auto sensor : m_det->sensors()) {
      if (!sensor->isInside(hit->midPoint())) continue;
      if (sensor->isInActiveArea(sensor->globalToLocal(hit->midPoint()))) {
        inAA = true;
        break;
      }
    }
    if (inAA) {
      mchitmap[hit->mcParticle()].push_back(hit);
    }
  }

  Tuple tuple = nTuple("VPAcceptance", "");

  for (const LHCb::MCParticle* particle : *particles) {
    const bool forward = particle->momentum().z() > 0.;
    const int ntracks = int(inverseLinker->relations(particle).size());

    // Determine origin vertex, decay vertex, and primary vertex.
    double fd = -1.;
    Gaudi::XYZPoint ov(-99999., -99999., -99999.);
    Gaudi::XYZPoint dv(-99999., -99999., -99999.);
    Gaudi::XYZPoint pv(-99999., -99999., -99999.);

    if (particle->endVertices().size() > 0) {
      dv = particle->endVertices().front()->position();
    }
    if (particle->originVertex() != 0x0) {
      ov = particle->originVertex()->position();
      if (particle->endVertices().size() > 0) {
        fd = (dv - ov).R();
      } else {
        fd = 99999.;
      }
    }
    if (particle->primaryVertex() != 0x0) {
      pv = particle->primaryVertex()->position();
    }

    Gaudi::XYZPoint firsthit(99999., 99999., 99999.);
    if (!forward) firsthit = Gaudi::XYZPoint(99999.,99999.,-99999.);
    int nmcparts(-999);
    int nhits(0);
    int nhitsleft(0);
    int nhitsright(0);
    if (ntracks > 0) {
      const LHCb::Track* track = inverseLinker->relations(particle).begin()->to();
      nmcparts = int(directLinker->relations(track).size());
      
      for (const LHCb::LHCbID& lhcbID : track->lhcbIDs()) {
        if (!track->isOnTrack(lhcbID)) continue;
        if (!lhcbID.isVP()) continue;
        const DeVPSensor* sensor = m_det->sensorOfChannel(lhcbID.vpID());
        ++nhits;
        if (sensor->isLeft()) {
          ++nhitsleft;
        } else if (sensor->isRight()) {
          ++nhitsright;
        }
            
        Gaudi::XYZPoint currenthit(sensor->channelToPoint(lhcbID.vpID(), false));
        if (forward) {
          if (firsthit.z() > currenthit.z()) firsthit = currenthit;
        } else {
          if (firsthit.z() < currenthit.z()) firsthit = currenthit;
        }
      }
    } // ntracks > 0

    tuple->column("p", particle->p());
    tuple->column("pt", particle->pt());
    tuple->column("pid", particle->particleID().pid());
    tuple->column("threecharge", particle->particleID().threeCharge());
    tuple->column("pvx", pv.x());
    tuple->column("pvy", pv.y());
    tuple->column("pvz", pv.z());
    tuple->column("ovx", ov.x());
    tuple->column("ovy", ov.y());
    tuple->column("ovz", ov.z());
    tuple->column("dvx", dv.x());
    tuple->column("dvy", dv.y());
    tuple->column("dvz", dv.z());
    tuple->column("fhx", firsthit.x());
    tuple->column("fhy", firsthit.y());
    tuple->column("fhz", firsthit.z());
    tuple->column("fhr", firsthit.Rho());
    tuple->column("fhphi", firsthit.phi());
    tuple->column("fd", fd);
    tuple->column("eta", particle->momentum().eta());
    tuple->column("phi", particle->momentum().phi());
    tuple->column("forward", forward);
    tuple->column("frompv", int(particle->originVertex()->isPrimary()));
    tuple->column("nendvertex", int(particle->endVertices().size()));
    tuple->column("ntracks", ntracks);
    tuple->column("nhits", nhits);
    tuple->column("nhitsleft", nhitsleft);
    tuple->column("nhitsright", nhitsright);
    tuple->column("nmchits", int(mchitmap.find(particle)->second.size()));
    tuple->column("nmcparts", nmcparts);
    tuple->column("mcmothertype", mcMotherType(*particle));
    tuple->column("mcvertextype", mcVertexType(*particle));

    tuple->write();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Walk up the decay tree of a particle and determine its origin vertex.
//=============================================================================
const LHCb::MCVertex* VPTrackEff::findMCOriginVertex(const LHCb::MCParticle& particle, 
                                                     const double decaylengthtolerance = 1.e-3) {

  const LHCb::MCVertex* ov = particle.originVertex();
  if (!ov) return ov;
  const LHCb::MCParticle* mother = ov->mother();
  if (mother && mother != &particle) {
    const LHCb::MCVertex* mov = mother->originVertex();
    if (!mov) return ov;
    const double d = (mov->position() - ov->position()).R();
    if (mov == ov || d < decaylengthtolerance) {
      ov = findMCOriginVertex(*mother, decaylengthtolerance);
    }
  }
  return ov;

}

//=============================================================================
// Return the origin vertex type of the mother particle.
//=============================================================================
int VPTrackEff::mcMotherType(const LHCb::MCParticle& particle) {

  const LHCb::MCVertex& vertex = findMCOriginVertex(particle);

  int rc(-1);
  if (vertex.isPrimary()) rc = 0;
  else if (vertex.mother()) {
    const LHCb::MCParticle* mother = vertex.mother();
    if (mother->particleID().hasBottom()) rc = 3;
    else if (mother->particleID().hasCharm()) rc = 2;
    else if (mother->particleID().hasStrange()) rc = 1;
    else rc = 4;
  }
  return rc ;

}

//=============================================================================
// Return the origin vertex type of a particle.
//=============================================================================
int VPTrackEff::mcVertexType(const LHCb::MCParticle& particle) {

  const LHCb::MCVertex &vertex = findMCOriginVertex(particle);
  return vertex.type();

}
