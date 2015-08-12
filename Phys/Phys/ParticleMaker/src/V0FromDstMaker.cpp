//-----------------------------------------------------------------------------
/** @file V0FromDstMaker.cpp
 *
 * Algorithm to build particles from the Brunel V0 list.
 *
 * @author Wouter Hulsbergen
 * @date 2012-01-01
 */
//-----------------------------------------------------------------------------

// from DaVinci
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Particle.h"
#include "Event/TwoProngVertex.h"
#include <boost/foreach.hpp>
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"

class V0FromDstMaker : public GaudiAlgorithm 
{
  
public:
  
  /// Standard constructor
  V0FromDstMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~V0FromDstMaker( ); ///< Destructor

  StatusCode initialize() ;
  StatusCode execute() ;

private:

  LHCb::Particle* build( const LHCb::TwoProngVertex& vertex,
                         const LHCb::ParticleID& pid,
                         const LHCb::Particle::Range& posDaughters,
                         const LHCb::Particle::Range& negDaughters ) const ;
  LHCb::Particle* build( const LHCb::TwoProngVertex& vertex,
                         const LHCb::ParticleID& pid,
                         const LHCb::Particle& daughterA,
                         const LHCb::Particle& daughterB ) const ;

private:

  std::string m_longPionLocation ;
  std::string m_downstreamPionLocation ;
  std::string m_longProtonLocation ;
  std::string m_downstreamProtonLocation ;
  std::string m_ksLLOutputLocation ;
  std::string m_ksDDOutputLocation ;
  std::string m_lambdaLLOutputLocation ;
  std::string m_lambdaDDOutputLocation ;
  std::string m_v0InputLocation ;
  const LHCb::ParticleProperty* m_ksProperty ;
  const LHCb::ParticleProperty* m_lambdaProperty ;
};

//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
V0FromDstMaker::V0FromDstMaker( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm (  name , pSvcLocator )
{

  // ProtoParticle filters to use for each type
  declareProperty( "LongPionLocation", m_longPionLocation = "Phys/StdAllNoPIDsPions/Particles") ;
  declareProperty( "DownstreamPionLocation", m_downstreamPionLocation = "Phys/StdNoPIDsDownPions/Particles") ;
  declareProperty( "LongProtonLocation", m_longProtonLocation = "Phys/StdAllNoPIDsProtons/Particles") ;
  declareProperty( "DownstreamProtonLocation", m_downstreamProtonLocation = "Phys/StdNoPIDsDownProtons/Particles") ;
  declareProperty( "KsLLOutputLocation", m_ksLLOutputLocation = "Phys/StdKs2PiPiLL" ) ;
  declareProperty( "KsDDOutputLocation", m_ksDDOutputLocation = "Phys/StdKs2PiPiDD" ) ;
  declareProperty( "LambdaLLOutputLocation", m_lambdaLLOutputLocation = "Phys/StdLambda2PPiLL" ) ;
  declareProperty( "LambdaDDOutputLocation", m_lambdaDDOutputLocation = "Phys/StdLambda2PPiDD" ) ;
  declareProperty( "V0InputLocation", m_v0InputLocation = "Rec/Vertex/V0" ) ;
}

V0FromDstMaker::~V0FromDstMaker( ) { }

StatusCode V0FromDstMaker::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  LHCb::IParticlePropertySvc* propertysvc =
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  m_ksProperty     = propertysvc->find( "KS0" ) ;
  m_lambdaProperty = propertysvc->find( "Lambda0" ) ;
  return sc ;
}

//=============================================================================
// Main execution
//=============================================================================

LHCb::Particle* V0FromDstMaker::build(const LHCb::TwoProngVertex& vertex,
                                      const LHCb::ParticleID& pid,
                                      const LHCb::Particle& daughterA,
                                      const LHCb::Particle& daughterB) const
{
  LHCb::Particle* v0 = new LHCb::Particle(pid) ;
  v0->addToDaughters( &daughterA ) ;
  v0->addToDaughters( &daughterB ) ;
  v0->setReferencePoint( vertex.position() ) ;
  double massA =  daughterA.momentum().M() ;
  double massB =  daughterB.momentum().M() ;

  v0->setMomentum( vertex.momentum(massA,massB) ) ;
  v0->setMeasuredMass( v0->momentum().M() ) ;
  Gaudi::SymMatrix7x7 cov = vertex.covMatrix7x7(massA,massB) ;
  v0->setMomCovMatrix( cov.Sub<Gaudi::SymMatrix4x4>(3,3) ) ;
  v0->setPosCovMatrix( cov.Sub<Gaudi::SymMatrix3x3>(0,0) ) ;
  v0->setPosMomCovMatrix( cov.Sub<Gaudi::Matrix4x3>(3,0) ) ;

  // now create the vertex object
  LHCb::Vertex* particlevertex = new LHCb::Vertex(vertex.position()) ;
  particlevertex->addToOutgoingParticles(&daughterA) ;
  particlevertex->addToOutgoingParticles(&daughterB) ;
  particlevertex->setChi2AndDoF( vertex.chi2(), vertex.nDoF() ) ;
  particlevertex->setCovMatrix( v0->posCovMatrix() ) ;

  v0->setEndVertex( particlevertex ) ;

  return v0 ;
}

LHCb::Particle* V0FromDstMaker::build( const LHCb::TwoProngVertex& vertex,
                                       const LHCb::ParticleID& pid,
                                       const LHCb::Particle::Range& particlesA,
                                       const LHCb::Particle::Range& particlesB) const
{
  // locate patricles associated to this V0 in input lists
  LHCb::Particle* v0(0) ;
  const LHCb::Particle* dauA(0), *dauB(0) ;
  const LHCb::Track* track ;
  BOOST_FOREACH( const LHCb::Particle* dau, particlesA) {
    if( dau->proto() && (track = dau->proto()->track()) &&
        track == vertex.trackA() ) dauA = dau ;
  }
  BOOST_FOREACH( const LHCb::Particle* dau, particlesB) {
    if( dau->proto() && (track = dau->proto()->track()) &&
        track == vertex.trackB() ) dauB = dau ;
  }
  if(dauA && dauB) v0 = build( vertex, pid, *dauA, *dauB) ;
  //info() << "Did we find the pions? " << pionA << " " << pionB << endreq ;
  return v0 ;
}


StatusCode V0FromDstMaker::execute()
{
  // get the input list with TwoProngVertices
  const LHCb::TwoProngVertex::Range vertices = get<LHCb::TwoProngVertex::Range>(m_v0InputLocation) ;

  // get the pions and protons, long and downstream
  const LHCb::Particle::Range longpions = get<LHCb::Particle::Range>(m_longPionLocation) ;
  const LHCb::Particle::Range downstreampions = get<LHCb::Particle::Range>(m_downstreamPionLocation) ;
  const LHCb::Particle::Range longprotons = get<LHCb::Particle::Range>(m_longProtonLocation) ;
  const LHCb::Particle::Range downstreamprotons = get<LHCb::Particle::Range>(m_downstreamProtonLocation) ;

  // create the Ks output lists
  LHCb::Particle::Container* ksllparticles = new LHCb::Particle::Container() ;
  LHCb::Vertex::Container* ksllvertices = new LHCb::Vertex::Container() ;
  LHCb::Particle::Container* ksddparticles = new LHCb::Particle::Container() ;
  LHCb::Vertex::Container* ksddvertices = new LHCb::Vertex::Container() ;

  // create the Lambda output lists
  LHCb::Particle::Container* lambdallparticles = new LHCb::Particle::Container() ;
  LHCb::Vertex::Container* lambdallvertices = new LHCb::Vertex::Container() ;
  LHCb::Particle::Container* lambdaddparticles = new LHCb::Particle::Container() ;
  LHCb::Vertex::Container* lambdaddvertices = new LHCb::Vertex::Container() ;

  put( ksllparticles,  m_ksLLOutputLocation + "/Particles" ) ;
  put( ksllvertices,  m_ksLLOutputLocation + "/Vertices" ) ;
  put( ksddparticles,  m_ksDDOutputLocation + "/Particles" ) ;
  put( ksddvertices,  m_ksDDOutputLocation + "/Vertices" ) ;

  put( lambdallparticles,  m_lambdaLLOutputLocation + "/Particles" ) ;
  put( lambdallvertices,  m_lambdaLLOutputLocation + "/Vertices" ) ;
  put( lambdaddparticles,  m_lambdaDDOutputLocation + "/Particles" ) ;
  put( lambdaddvertices,  m_lambdaDDOutputLocation + "/Vertices" ) ;

  // horrible logic, but I don't know how to do this otherwise, at
  // least, not at this hour of the day
  BOOST_FOREACH( const LHCb::TwoProngVertex* vertex, vertices) {
    assert( vertex->trackA()->firstState().qOverP()>0) ;
    assert( vertex->trackB()->firstState().qOverP()<0) ;
    // just make sure they are unique
    std::set<LHCb::ParticleID> pids( vertex->compatiblePIDs().begin(), vertex->compatiblePIDs().end() ) ;
    BOOST_FOREACH( LHCb::ParticleID pid, pids) {
      if( pid == m_ksProperty->pdgID() ) {
        LHCb::Particle* p = build( *vertex,pid,longpions,longpions ) ;
        if( p ) {
          ksllparticles->insert( p ) ;
          ksllvertices->insert( const_cast<LHCb::Vertex*>(p->endVertex()) ) ;
        } else {
          p = build( *vertex,pid,downstreampions,downstreampions ) ;
          if( p ) {
            ksddparticles->insert( p ) ;
            ksddvertices->insert( const_cast<LHCb::Vertex*>(p->endVertex()) ) ;
          }
        }
      } else if( std::abs(pid.pid()) == std::abs(m_lambdaProperty->pdgID().pid()) ) {
        const LHCb::Particle::Range* rangeA = &longprotons ;
        const LHCb::Particle::Range* rangeB = &longpions ;
        if( pid.pid()<0) std::swap( rangeA, rangeB) ;
        LHCb::Particle* p = build( *vertex,pid,*rangeA,*rangeB) ;
        if( p ) {
          lambdallparticles->insert( p ) ;
          lambdallvertices->insert( const_cast<LHCb::Vertex*>(p->endVertex()) ) ;
        } else {
          const LHCb::Particle::Range* rangeA = &downstreamprotons ;
          const LHCb::Particle::Range* rangeB = &downstreampions ;
          if( pid.pid()<0) std::swap( rangeA, rangeB) ;
          p = build( *vertex,pid,*rangeA,*rangeB) ;
          if( p ) {
            lambdaddparticles->insert( p ) ;
            lambdaddvertices->insert( const_cast<LHCb::Vertex*>(p->endVertex()) ) ;
          }
        }
      }
    }
  }

  //info() << "Number of input vertices/output: " << vertices.size() << " " << outputparticles->size() << endreq ;

  return StatusCode::SUCCESS ;
}

// Declaration of the Tool Factory
DECLARE_ALGORITHM_FACTORY( V0FromDstMaker )
