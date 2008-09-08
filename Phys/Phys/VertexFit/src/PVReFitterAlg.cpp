// $Id: PVReFitterAlg.cpp,v 1.4 2008-09-08 16:09:10 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// LHCb
#include <Event/RecVertex.h>
#include <Event/Particle.h>
#include <Event/Track.h>
#include "Kernel/IOnOffline.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IRelatedPVFinder.h"
#include "TrackInterfaces/IPVOfflineTool.h"
// local
#include "PVReFitterAlg.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PVReFitterAlg
//
// 2008-06-25 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVReFitterAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVReFitterAlg::PVReFitterAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_onOfflineTool(0),
  m_pvOfflineTool(0),
  m_pvReFitter(0),
  m_lifetimeFitter(0),
  m_relatedPVFinder(0),
  m_pvOfflinetoolType("PVOfflineTool"),
  m_pvReFitterType("AdaptivePVReFitter"),
  m_lifetimeFitterType("PropertimeFitter"),
  m_relatedPVFinderType("Default"),
  m_particleInputLocation(""),
  m_particle2VertexRelationsInputLocation(""),
  m_particle2VertexRelationsOutputLocation(""),
  m_vertexOutputLocation("")

{

  declareProperty("IPVOfflineTool", m_pvOfflinetoolType);
  declareProperty("IPVReFitter",    m_pvReFitterType);
  declareProperty("ILifetimeFitter",    m_lifetimeFitterType);
  declareProperty("IRelatedPVFinder", m_relatedPVFinderType);
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("P2VRelationsInputLocation",  
                  m_particle2VertexRelationsInputLocation);
  declareProperty("P2VRelationsOutputLocation",  
                  m_particle2VertexRelationsOutputLocation);
  declareProperty("VertexOutputLocation", m_vertexOutputLocation);

}
//=============================================================================
// Destructor
//=============================================================================
PVReFitterAlg::~PVReFitterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVReFitterAlg::initialize() {

  //  StatusCode sc = DVAlgorithm::initialize(); // must be executed first

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by DVAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  //  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading PhysDesktop" << endmsg;

  //  this->desktop();

  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool", this);

  m_pvOfflineTool = tool<IPVOfflineTool> (m_pvOfflinetoolType, this);

  m_pvReFitter = tool<IPVReFitter>(m_pvReFitterType, this);

  m_lifetimeFitter = tool<ILifetimeFitter>(m_lifetimeFitterType, this);

  if (m_relatedPVFinderType != "Default") {
    verbose() << "Use non-standard IRelatedPVFinder " 
              << m_relatedPVFinderType << endmsg;
    m_relatedPVFinder = tool<IRelatedPVFinder>(m_relatedPVFinderType, this);
  } else {
    m_relatedPVFinder = m_onOfflineTool->pvRelator();
  }

  return ( m_pvOfflineTool   && 
           m_pvReFitter      && 
           m_lifetimeFitter  && 
           m_relatedPVFinder && 
           m_onOfflineTool   &&
           m_relatedPVFinder    ) 
    ? StatusCode::SUCCESS
    : StatusCode::FAILURE;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVReFitterAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::Particle::Container>(m_particleInputLocation)) {
    error() << "No LHCb::Particle::Container found at "
            <<  m_particleInputLocation << endmsg;
    return StatusCode::SUCCESS;
  }

//   if (!exist<Particle2Vertex::Table>(m_particle2VertexRelationsInputLocation) )
//   {  
//     error() << "No LHCb::Particle->LHCb::RecVertex table found at "
//             << m_particle2VertexRelationsInputLocation << endmsg;
//     return StatusCode::SUCCESS;    
//   } else {
//     m_p2VtxTable = 
//       get<Particle2Vertex::Table>(m_particle2VertexRelationsInputLocation);
//   }
  

  LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>(m_particleInputLocation);

//   Particle2Vertex::Table* table = 
//     get<Particle2Vertex::Table>(m_particle2VertexRelationsInputLocation);

  if (0==particles) {
    error() << "No LHCb::Particles in LHCb::Particle::Container "
            <<  m_particleInputLocation << endmsg;
    return StatusCode::SUCCESS;
  }
  
//   if (0==table) {
//     error() << "No LHCb::Particle->LHCb::RecVertex "
//             << m_particle2VertexRelationsInputLocation << endmsg;
//     return StatusCode::SUCCESS;    
//   }

  LHCb::RecVertex::Container* vertexContainer = 
    new LHCb::RecVertex::Container();

  verbose() << "Storing re-fitted vertices in " 
            << m_vertexOutputLocation << endmsg;

  put(vertexContainer, m_vertexOutputLocation);  

  Particle2Vertex::Table* newTable = new Particle2Vertex::Table();

  verbose() << "Storing Particle->Refitted Vtx relations table in "
            << m_particle2VertexRelationsOutputLocation << endmsg;

  put( newTable, m_particle2VertexRelationsOutputLocation);

  verbose() << "Loop over " << particles->size() << " particles" << endmsg;

  for ( LHCb::Particle::Container::const_iterator itP = particles->begin();
        itP != particles->end(); 
        ++itP) {
    const LHCb::RecVertex* bestVertex = getRelatedVertex(*itP);

    if (0!=bestVertex) {
      if( !vertexContainer->object(bestVertex->key()) ) {
        verbose() << "Found related vertex for particle\n" 
                  << *(*itP) << endmsg;
        LHCb::RecVertex* refittedVertex = refitVertex(bestVertex, *itP);
        if (0!=refittedVertex) {
          verbose() << "Storing vertex with key " << refittedVertex->key()
                   << " into container slot with key " << bestVertex->key() << endmsg;
          vertexContainer->insert(refittedVertex);
          newTable->relate(*itP, refittedVertex, 1.);
        }
      } else {
        verbose() << "Related vertex had already been refitted" << endmsg;
      }
    } else {
      warning() << "Found no related vertex for particle\n" 
                << *(*itP) << endmsg;
    }
  }

  if (exist<LHCb::RecVertex::Container>(m_vertexOutputLocation) )
  {  
    const LHCb::RecVertex::Container* vertices = 
      get<LHCb::RecVertex::Container>(m_vertexOutputLocation);
    verbose() << "CHECK: stored " 
              << vertices->size()
              << " re-fitted vertices in " 
              << m_vertexOutputLocation << endmsg;
  } else {
    error() << "No re-fitted vertices at "
            << m_vertexOutputLocation << endmsg;    
  }


  if (exist<Particle2Vertex::Table>(m_particle2VertexRelationsOutputLocation) )
  {  
    verbose() << "CHECK: table is at " 
              << m_particle2VertexRelationsOutputLocation << endmsg;
  } else {
    error() << "No LHCb::Particle->LHCb::RecVertex table found at "
            << m_particle2VertexRelationsOutputLocation << endmsg;    
  }
  
  m_p2VtxTable.clear();

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::RecVertex* PVReFitterAlg::refitVertex(const LHCb::RecVertex* v,
                                            const LHCb::Particle* p  ) const
{

  LHCb::RecVertex* reFittedVertex = new LHCb::RecVertex(v->key());  

  LHCb::Track::ConstVector tracks;
  
  getTracks(p, tracks);

  StatusCode sc = m_pvOfflineTool->reDoSinglePV(v->position(), 
                                                tracks, 
                                                *reFittedVertex );

  const double max_double = std::numeric_limits<double>::max();

  double tau(max_double); 
  double tauErr(max_double); 
  double tauChi2(max_double); 

  if (sc==StatusCode::SUCCESS) {
    sc = m_pvReFitter->remove(p, reFittedVertex);
  }
  if (sc==StatusCode::SUCCESS) {
    sc = m_lifetimeFitter->fit(*reFittedVertex, *p, tau, tauErr, tauChi2);;  
  }

  return (sc==StatusCode::SUCCESS) ? reFittedVertex : 0;
}
//=============================================================================
const LHCb::RecVertex* PVReFitterAlg::getRelatedVertex(const LHCb::Particle* p)
{
  StatusCode sc = m_relatedPVFinder->relatedPVs(p, &m_p2VtxTable);
  if (sc.isFailure()) return 0;
  return dynamic_cast<const LHCb::RecVertex*>(m_p2VtxTable.relations(p).back().to());
}
//=============================================================================
void PVReFitterAlg::getTracks(const LHCb::Particle* p,
                              LHCb::Track::ConstVector& tracks) const
{
  const LHCb::ProtoParticle*   proto  = p->proto() ;

  if( proto ) {

    const LHCb::Track* track = proto->track();
    if(track) tracks.push_back(track);

  } else {
 
    const SmartRefVector< LHCb::Particle >& Prods = p->daughters();
    SmartRefVector< LHCb::Particle >::const_iterator iProd;

    for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){

      const LHCb::Particle* daughter = *iProd;
      getTracks(daughter, tracks);
      
    }

  }
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVReFitterAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
