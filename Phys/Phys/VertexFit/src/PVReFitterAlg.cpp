// $Id: PVReFitterAlg.cpp,v 1.18 2010-06-21 12:32:11 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
//#include "GaudiKernel/ObjectVector.h"
// LHCb
#include <Event/RecVertex.h>
#include <Event/Particle.h>
#include <Event/Track.h>
#include "Kernel/IPVReFitter.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IOnOffline.h"
#include "TrackInterfaces/IPVOfflineTool.h"
//
#include "Kernel/DaVinciStringUtils.h"
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
  m_pvOfflineTool(0),
  m_pvReFitter(0),
  m_onOfflineTool(0),
  m_pvOfflinetoolType("PVOfflineTool"),
  m_pvReFitterType("AdaptivePVReFitter"),
  m_useIPVOfflineTool(false),
  m_useIPVReFitter(true),
  m_particleInputLocation(""),
  m_PVInputLocation(""),
  m_particle2VertexRelationsOutputLocation(""),
  m_vertexOutputLocation(""),
  m_outputLocation("")

{

  declareProperty("IPVOfflineTool", m_pvOfflinetoolType);
  declareProperty("IPVReFitter",    m_pvReFitterType);
  declareProperty("UseIPVOfflineTool", m_useIPVOfflineTool);
  declareProperty("UseIPVReFitter",    m_useIPVReFitter);
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  
}
//=============================================================================
// Destructor
//=============================================================================
PVReFitterAlg::~PVReFitterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVReFitterAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by DVAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if (m_useIPVOfflineTool) {    
    m_pvOfflineTool = tool<IPVOfflineTool> (m_pvOfflinetoolType, this);
    if (0==m_pvOfflineTool) return StatusCode::FAILURE;
  }

  if (m_useIPVReFitter) {
    m_pvReFitter = tool<IPVReFitter>(m_pvReFitterType, this);
    if (0==m_pvReFitter) return StatusCode::FAILURE;
  }

  if (!m_useIPVOfflineTool && ! m_useIPVReFitter) {
    return Error("At least one of UseIPVOfflineTool and UseIPVReFitter must be true!", StatusCode::FAILURE);
  }

  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool", this);

  if (0==m_onOfflineTool) {
    return Error("Could not get OnOfflineTool");
  }
  
  if (m_PVInputLocation=="") {
    m_PVInputLocation=m_onOfflineTool->primaryVertexLocation();
  }
  

  m_outputLocation = m_particleInputLocation;
  
  removeEnding(m_outputLocation, "/Particles");

  std::string instanceName = this->name();
  
  DaVinci::StringUtils::expandLocation(m_outputLocation,
                                       m_onOfflineTool->trunkOnTES());

  m_vertexOutputLocation = m_outputLocation + "/"+instanceName+"_PVs";
  m_particle2VertexRelationsOutputLocation = m_outputLocation + "/"+instanceName+"_P2PV";

  return sc;
  
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVReFitterAlg::execute() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::Particle::Container>(m_particleInputLocation) &&
      !exist<LHCb::Particle::Range>(m_particleInputLocation) ) {
    return Error("No LHCb::Particle::Range found at " + 
                 m_particleInputLocation,
                 StatusCode::SUCCESS);
  }
  if (!exist<LHCb::RecVertex::Container>(m_PVInputLocation) &&
      !exist<LHCb::RecVertex::Selection>(m_PVInputLocation) ) {
    return Error("No LHCb::RecVertex::Range found at " +
                 m_PVInputLocation,
                 StatusCode::SUCCESS);
  }

  const LHCb::Particle::Range particles = 
    get<LHCb::Particle::Range>(m_particleInputLocation);

  if (particles.empty()) {
    return Error("No LHCb::Particles in LHCb::Particle::Range " +
                 m_particleInputLocation,
                 StatusCode::SUCCESS);
  }

  const LHCb::RecVertex::Range vertices = 
    get<LHCb::RecVertex::Range>(m_PVInputLocation);

  if (vertices.empty()) {
    return Error("No LHCb::RecVertices in LHCb::Particle::Range "+
                 m_PVInputLocation,
                 StatusCode::SUCCESS);
  }
  
  LHCb::RecVertex::Container* vertexContainer = 
    new  LHCb::RecVertex::Container();

  verbose() << "Storing re-fitted vertices in " 
            << m_vertexOutputLocation << endmsg;

  put(vertexContainer, m_vertexOutputLocation);  

  Particle2Vertex::Table* newTable = new Particle2Vertex::Table();

  verbose() << "Storing Particle->Refitted Vtx relations table in "
            << m_particle2VertexRelationsOutputLocation << endmsg;

  put( newTable, m_particle2VertexRelationsOutputLocation);

  verbose() << "Loop over " << particles.size() << " particles" << endmsg;

  for ( LHCb::Particle::Range::const_iterator itP = particles.begin();
        itP != particles.end(); 
        ++itP) {
    
    for (LHCb::RecVertex::Range::const_iterator itPV = vertices.begin();
         itPV != vertices.end();
         ++itPV) {
      
      verbose() << "Re-fitting vertex for particle\n" 
                << *(*itP) << endmsg;
      LHCb::RecVertex* refittedVertex = refitVertex(*itPV, *itP);
      if (0!=refittedVertex) {
        verbose() << "Storing vertex with key " << refittedVertex->key()
                  << " into container slot with key " << (*itPV)->key() 
                  << endmsg;
        vertexContainer->insert(refittedVertex);
        newTable->relate(*itP, refittedVertex);
        verbose() << "Re-fitted vertex " << *(*itPV) << "\n as\n"
                  << *refittedVertex << endmsg;
      }
    }
  }
  
  setFilterPassed(true);

  if (exist<LHCb::RecVertex::Container>(m_vertexOutputLocation) )
  {  
    const LHCb::RecVertex::Container* vertices = 
      get<LHCb::RecVertex::Container>(m_vertexOutputLocation);
    verbose() << "CHECK: stored " 
              << vertices->size()
              << " re-fitted vertices in " 
              << m_vertexOutputLocation << endmsg;
  } else {
    return Error("No re-fitted vertices at "+
                 m_vertexOutputLocation,
                 StatusCode::SUCCESS);
  }


  if (exist<Particle2Vertex::Table>(m_particle2VertexRelationsOutputLocation) )
  {  
    verbose() << "CHECK: table is at " 
              << m_particle2VertexRelationsOutputLocation << endmsg;
  } else {
    return Error("No LHCb::Particle->LHCb::RecVertex table found at "+
                 m_particle2VertexRelationsOutputLocation,
                 StatusCode::SUCCESS);
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::RecVertex* PVReFitterAlg::refitVertex(const LHCb::RecVertex* v,
                                            const LHCb::Particle* p  ) const
{

  LHCb::RecVertex* reFittedVertex = 
    m_useIPVOfflineTool ? new LHCb::RecVertex() : new LHCb::RecVertex(*v) ;
  StatusCode sc(StatusCode::SUCCESS);

  if (m_useIPVOfflineTool) {
    LHCb::Track::ConstVector tracks;
    getTracks(p, tracks);
    sc = m_pvOfflineTool->reDoSinglePV(v->position(), 
                                       tracks, 
                                       *reFittedVertex );
  }

  if (m_useIPVReFitter && sc==StatusCode::SUCCESS) {
    sc = m_pvReFitter->remove(p, reFittedVertex);
  }
  
  return (sc==StatusCode::SUCCESS) ? reFittedVertex : 0;
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
void PVReFitterAlg::removeEnding(std::string& a, const std::string& ending)
{
  std::string::size_type pos = a.rfind(ending);
  if ( pos != std::string::npos ) {
    std::cout << "found " << ending << " at " << pos << std::endl;
    a = std::string(a, 0, pos);
    
  }
}
//=============================================================================
