// ============================================================================
// Include files 
// ============================================================================

#include "TrackTune.h"

#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"


DECLARE_ALGORITHM_FACTORY( TrackTune )

TrackTune::TrackTune(const std::string& name, ISvcLocator* pSvc):
  GaudiTupleAlg(name,pSvc), m_minMass(0.), m_maxMass(0.)
{

  declareProperty("ParticleLocation", m_particleLocation = "/Event/Dimuon/Phys/SelDiMuonInciLoose/Particles");
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default);
  declareProperty("resonanceName", m_resonanceName = "J/psi(1S)");
  declareProperty("minPurityCut", m_minPurityCut = 0.7);
  declareProperty("selectBest", m_selectBest = true);
  declareProperty("resonance" , m_deltaMass = 110.);
}


TrackTune::~TrackTune() {}

StatusCode TrackTune::initialize() {

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  LHCb::IParticlePropertySvc* propertysvc = 
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  const LHCb::ParticleProperty* prop = propertysvc->find( m_resonanceName);
  if (prop != 0){
    m_minMass = prop->mass() - m_deltaMass; 
    m_maxMass  = prop->mass() + m_deltaMass;
  }
  else {
    return Error("Failed to find resonance", StatusCode::SUCCESS);
  } 
  propertysvc->release();  

  info() << "MinMass " << m_minMass << " MaxMass " << m_maxMass   << endmsg;

  return StatusCode::SUCCESS;

}
 
StatusCode TrackTune::execute()
{

  // output tuple
  Tuple myTuple = nTuple("Candidates");

  const LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation);
  const LHCb::Particle::Range particles = get<LHCb::Particle::Range>(m_particleLocation);

  std::vector<const LHCb::Particle* > tVec; 
  if (select(tVec,particles) == false) return StatusCode::SUCCESS ;

  for (std::vector<const LHCb::Particle* >::const_iterator iterP = tVec.begin(); iterP != tVec.end(); ++iterP ){

    bool rec = isFound(tracks,**iterP);
    myTuple <<  Tuples::Column("M", (*iterP)->measuredMass()) 
            <<  Tuples::Column("found",rec) 
	    <<  Tuples::Column("PT", (*iterP)->pt())
            <<  Tuples::Column("Candidates", particles.size());

    myTuple->write();
  }

  return StatusCode::SUCCESS ;
  //
} // the end of the Algorihtm

const LHCb::Track* TrackTune::track(const LHCb::Particle& part) const
{
  const LHCb::ProtoParticle* proto = part.proto();
  if (!proto || proto->charge() == 0) return 0;
  return proto->track() ;
}


bool TrackTune::isFound(const LHCb::Track::Range& tracks, const LHCb::Particle& part) const
{
  bool ok = true;
  const SmartRefVector<LHCb::Particle>& daughters = part.daughters();
  for (SmartRefVector<LHCb::Particle>::const_iterator iter = daughters.begin(); 
      iter != daughters.end() && ok == true ; ++iter){
     const LHCb::Track* aTrack = track(**iter);
     if (!aTrack) {
       info() << "Failed to find track " << endmsg;
     }
     const double nHits = aTrack->nLHCbIDs();
     bool matchedTrack = false;
     for (LHCb::Track::Range::const_iterator iterT = tracks.begin(); iterT != tracks.end() && matchedTrack == false; ++iterT){
       const double fracCommon = aTrack->nCommonLhcbIDs(**iterT)/double(nHits);
       plot(fracCommon, "purity", "purity",  0., 2., 100);
       if (fracCommon > m_minPurityCut) matchedTrack = true;
     } // tracks
     if (matchedTrack == false) ok = false;
  } // particles


  return ok;
}


bool TrackTune::select(std::vector<const LHCb::Particle* >& output, const LHCb::Particle::Range& input) const
{
  if (m_selectBest == true){
    double bestChi2 = 9999.; const LHCb::Particle* bestPart = 0;
    for (LHCb::Particle::Range::const_iterator iter = input.begin(); iter != input.end(); ++iter){
      if (inMassRange(**iter) == false) continue;
      const LHCb::Vertex* vert  = (*iter)->endVertex();
      if (vert->chi2PerDoF() < bestChi2){
        bestChi2= vert->chi2PerDoF();
        bestPart = *iter;
      }
    }
    if (bestPart) output.push_back(bestPart);
  }   
  else {
    for (LHCb::Particle::Range::const_iterator iter = input.begin(); iter != input.end(); ++iter){
      output.push_back(*iter);
    }
  }
  return (output.size() == 0 ? false : true ); 
}

bool TrackTune::inMassRange(const LHCb::Particle& particle) const
{
  const double m = particle.measuredMass();
  return  (m > m_minMass && m< m_maxMass);
}

