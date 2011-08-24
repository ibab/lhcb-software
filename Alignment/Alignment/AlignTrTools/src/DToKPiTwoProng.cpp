
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"

#include "Event/TwoProngVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackVertexer.h"

#include "Event/Track.h"
#include "Event/Particle.h"

#include "DToKPiTwoProng.h"

#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "TrackInterfaces/ITrackFitter.h"


//#include "LoKi/ParticleProperties.h"

DECLARE_ALGORITHM_FACTORY( DToKPiTwoProng );


DToKPiTwoProng::DToKPiTwoProng(const std::string& name, ISvcLocator* pSvc):
  GaudiTupleAlg(name,pSvc)
{

  declareProperty( "ParticleLocation", m_particleLocation = "/Event/Strip02/SeqD2HH/Phys/SelD2HH/Particles" );
  declareProperty( "VertexLocation", m_vertexLocation = "Rec/Vertex/DKPi" ) ;
  declareProperty( "DaughterTrackLocation", m_trackOutputLocation = "Rec/Track/DKPiDaughters" ) ;
  declareProperty( "dM", m_deltaMass = 24 * Gaudi::Units::MeV );
  declareProperty( "resonanceName", m_resonanceName = "D0" );

}


DToKPiTwoProng::~DToKPiTwoProng() {}

StatusCode DToKPiTwoProng::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize();
  if (sc.isFailure()){
    return Warning("Failed to init base class", StatusCode::FAILURE);
  } 

  m_vertexer = tool<ITrackVertexer>( "TrackVertexer" );
  m_trackExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator");

  m_trackPreFit = tool<ITrackFitter>("TrackMasterFitter"," preFit", this);
  m_trackFit = tool<ITrackFitter>("TrackMasterFitter", "fit", this);
  
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

  const LHCb::ParticleProperty* prop2 = propertysvc->find("pi+");
  m_pionMass = prop2->mass();

  const LHCb::ParticleProperty* prop3 = propertysvc->find("K+");
  m_kaonMass = prop3->mass();
  
  propertysvc->release();  
  
  /*
  using namespace LoKi::Particles;
  double dMass = massFromName(m_resonanceName);
  m_minMass = dMass - m_deltaMass;
  m_maxMass = dMass + m_deltaMass;
  m_pionMass = massFromName("pi+");
  m_kaonMass = massFromName("K+");
  */

  return StatusCode::SUCCESS;
}


StatusCode DToKPiTwoProng::finalize(){
  return GaudiTupleAlg::finalize();
}

StatusCode DToKPiTwoProng::execute()
{
  
  // Create the output container 
  typedef KeyedContainer<LHCb::TwoProngVertex, Containers::HashMap> TwoProngVertices;
  TwoProngVertices* vertContainer = new TwoProngVertices() ;
  put(vertContainer, m_vertexLocation);
  
  LHCb::Track::Selection* daughterCont = new LHCb::Track::Selection() ;
  put(daughterCont, m_trackOutputLocation);
  
  //std::cout << " Going to Loop " << std::endl;
  
  // get the input particles 
  const LHCb::Particle::Container* particles = get<LHCb::Particle::Container>(m_particleLocation);
  for( LHCb::Particle::Container::const_iterator iterP = particles->begin(); iterP != particles->end(); ++iterP ){
    
    const SmartRefVector<LHCb::Particle> daughters = (*iterP)->daughters();
    LHCb::Track* track1 = track(daughters.front());
    LHCb::Track* track2 = track(daughters.back());

    m_trackPreFit->fit(*track1); 
    m_trackFit->fit(*track1);

    m_trackPreFit->fit(*track2); 
    m_trackFit->fit(*track2);

    double z = (*iterP)->endVertex()->position().z();

    // make the vertex, make first particle the kaon
    LHCb::TwoProngVertex* tvertex; 
    if (PIDK(daughters[0]) > PIDK(daughters[1])){
     tvertex = refittedMass(track1, track2, z);          
    }
   else {
     tvertex = refittedMass(track2, track1, z); 
   }
  
   double m = tvertex->mass(m_kaonMass, m_pionMass);
   
   //Tuple tuple = nTuple("testTuple");
   //tuple << Tuples::Column("m", m); 
   //tuple->write(); 

   //   std::cout << " m" << m << std::endl;   
   plot(m , "m" , 1000. , 2000, 200);
   if (m > m_minMass && m < m_maxMass){
     vertContainer->insert(tvertex);
     daughterCont->insert(track1);
     daughterCont->insert(track2);
   }
   else {
     delete tvertex;
   }
 } // loop candidates

 return StatusCode::SUCCESS;

} // the end of the Algorihtm


LHCb::Track* DToKPiTwoProng::track(const LHCb::Particle* part) const{

  const LHCb::ProtoParticle* proto = part->proto();
  if (!proto || proto->charge() == 0) return 0;
  return const_cast<LHCb::Track*>(proto->track()) ;
}

double DToKPiTwoProng::PIDK(const LHCb::Particle* part) const{

  const LHCb::ProtoParticle* proto = part->proto();
  return proto == 0 ? -9999 : proto->info(LHCb::ProtoParticle::CombDLLk,-1000);
}

LHCb::TwoProngVertex* DToKPiTwoProng::refittedMass(LHCb::Track* track1, 
						   LHCb::Track* track2,
                                                   const double zVert ) const{
  
  LHCb::State* state1 = track1->stateAt(LHCb::State::ClosestToBeam);
  m_trackExtrapolator->propagate(*state1, zVert);
  LHCb::State* state2 =track2->stateAt(LHCb::State::ClosestToBeam);
  m_trackExtrapolator->propagate(*state2, zVert);

  LHCb::TwoProngVertex* vertex =  m_vertexer->fit(*state1,*state2) ;
  vertex->addToTracks( track1 ) ;
  vertex->addToTracks( track2 ) ;

  return vertex;
 
}





