// $Id: PVReFitterAlg.cpp,v 1.20 2010-08-12 11:59:21 jpalac Exp $
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
DECLARE_ALGORITHM_FACTORY( PVReFitterAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PVReFitterAlg::PVReFitterAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator ),
      m_pvOfflineTool(0),
      m_pvReFitter(0),
      m_onOfflineTool(0),
      m_pvOfflinetoolType("PVOfflineTool"),
      m_pvReFitterType("AdaptivePVReFitter"),
      m_useIPVOfflineTool(false),
      m_useIPVReFitter(true),
      m_particleInputLocation(""),
      m_particleInputLocations(),
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
  declareProperty("ParticleInputLocations",  m_particleInputLocations);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
}

//=============================================================================
// Destructor
//=============================================================================
PVReFitterAlg::~PVReFitterAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVReFitterAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if ( !m_particleInputLocations.empty() &&
       !m_particleInputLocation.empty()  )
  {
    return Error("You have set both ParticleInputLocation AND ParticleInputLocations properties");
  }

  if ( m_particleInputLocations.empty() )
  {
    if ( m_particleInputLocation.empty() )
    {
      return Error("You have to set either ParticleInputLocation OR ParticleInputLocations properties");
    }
    else
    {
      Warning("ParticleInputLocation is deprecated. Please use ParticleInputLocations list.",
              StatusCode::SUCCESS).ignore();
    }
    m_particleInputLocations.push_back(m_particleInputLocation);
  }

  if (m_useIPVOfflineTool)
  {
    m_pvOfflineTool = tool<IPVOfflineTool> (m_pvOfflinetoolType, this);
    if (!m_pvOfflineTool) return StatusCode::FAILURE;
  }

  if (m_useIPVReFitter)
  {
    m_pvReFitter = tool<IPVReFitter>(m_pvReFitterType, this);
    if (!m_pvReFitter) return StatusCode::FAILURE;
  }

  if ( !m_useIPVOfflineTool && !m_useIPVReFitter )
  {
    return Error("At least one of UseIPVOfflineTool and UseIPVReFitter must be true!", StatusCode::FAILURE);
  }

  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool", this);

  if (!m_onOfflineTool)
  {
    return Error("Could not get OnOfflineTool");
  }

  if (m_PVInputLocation.empty())
  {
    m_PVInputLocation=m_onOfflineTool->primaryVertexLocation();
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVReFitterAlg::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (!exist<LHCb::RecVertex::Range>(m_PVInputLocation) )
  {
    return StatusCode::SUCCESS;
//     return Warning("No LHCb::RecVertex::Range found at " +
//                    m_PVInputLocation,
//                    StatusCode::SUCCESS, 0);
  }

  const LHCb::RecVertex::Range vertices =
    get<LHCb::RecVertex::Range>(m_PVInputLocation);

  if (vertices.empty())
  {
    return StatusCode::SUCCESS;
//     return Warning("No LHCb::RecVertices in LHCb::Particle::Range "+
//                    m_PVInputLocation,
//                    StatusCode::SUCCESS,
//                    0);
  }

  for ( std::vector<std::string>::const_iterator iLoc = m_particleInputLocations.begin();
        iLoc != m_particleInputLocations.end(); ++iLoc )
  {
    executeForLocation(*iLoc, vertices);
  }

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

//=============================================================================

void PVReFitterAlg::executeForLocation(const std::string& particleLocation,
                                       const LHCb::RecVertex::Range& vertices) const
{

  if (!exist<LHCb::Particle::Range>(particleLocation) )
  {
    Warning("No LHCb::Particle::Range found at " +
            particleLocation,
            StatusCode::SUCCESS,
            0).ignore();
    return;
  }

  const LHCb::Particle::Range particles =
    get<LHCb::Particle::Range>(particleLocation);

  if (particles.empty())
  {
    Warning("No LHCb::Particles in LHCb::Particle::Range " +
            particleLocation,
            StatusCode::SUCCESS,
            0).ignore();
    return;
  }

  std::string outputLocation = particleLocation;

  DaVinci::StringUtils::removeEnding(outputLocation, "/Particles");

  DaVinci::StringUtils::expandLocation(outputLocation,
                                       m_onOfflineTool->trunkOnTES());

  const std::string& instanceName = this->name();
  const std::string vertexOutputLocation = outputLocation + "/"+instanceName+"_PVs";
  const std::string particle2VertexRelationsOutputLocation = outputLocation + "/"+instanceName+"_P2PV";

  LHCb::RecVertex::Container* vertexContainer =
    new LHCb::RecVertex::Container();

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "Storing re-fitted vertices in "
              << vertexOutputLocation << endmsg;
  }

  put(vertexContainer, vertexOutputLocation);

  Particle2Vertex::Table* newTable = new Particle2Vertex::Table();

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "Storing Particle->Refitted Vtx relations table in "
              << particle2VertexRelationsOutputLocation << endmsg;
  }

  put( newTable, particle2VertexRelationsOutputLocation);

  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "Loop over " << particles.size() << " particles" << endmsg;
  }

  for ( LHCb::Particle::Range::const_iterator itP = particles.begin();
        itP != particles.end(); ++itP )
  {

    for ( LHCb::RecVertex::Range::const_iterator itPV = vertices.begin();
          itPV != vertices.end(); ++itPV )
    {

      if (msgLevel(MSG::VERBOSE))
      {
        verbose() << "Re-fitting vertex for particle\n"
                  << *(*itP) << endmsg;
      }

      LHCb::RecVertex* refittedVertex = refitVertex(*itPV, *itP);

      if (refittedVertex)
      {
        if (msgLevel(MSG::VERBOSE))
        {
          verbose() << "Storing vertex with key " << refittedVertex->key()
                    << " into container slot with key " << (*itPV)->key()
                    << endmsg;
          verbose() << "Re-fitted vertex " << *(*itPV) << "\n as\n"
                    << *refittedVertex << endmsg;
        }
        vertexContainer->insert(refittedVertex);
        newTable->relate(*itP, refittedVertex);
      }

    }
  }

  if (msgLevel(MSG::VERBOSE))
  {
    // check output.
    if (exist<LHCb::RecVertex::Container>(vertexOutputLocation) )
    {
      const LHCb::RecVertex::Container* _vertices =
        get<LHCb::RecVertex::Container>(vertexOutputLocation);
      verbose() << "CHECK: stored "
                << _vertices->size()
                << " re-fitted vertices in "
                << m_vertexOutputLocation << endmsg;
    }
    else
    {
      Error("No re-fitted vertices at "+
            m_vertexOutputLocation,
            StatusCode::SUCCESS,
            0).ignore();
    }


    if (exist<Particle2Vertex::Table>(particle2VertexRelationsOutputLocation) )
    {
      verbose() << "CHECK: table is at "
                << particle2VertexRelationsOutputLocation << endmsg;
    }
    else
    {
      Error("No LHCb::Particle->LHCb::RecVertex table found at "+
            m_particle2VertexRelationsOutputLocation,
            StatusCode::SUCCESS,
            0).ignore();
    }
  }

}

//=============================================================================

LHCb::RecVertex* PVReFitterAlg::refitVertex(const LHCb::RecVertex* v,
                                            const LHCb::Particle* p  ) const
{

  if ( !v || !p ) return NULL;

  LHCb::RecVertex* reFittedVertex(NULL);

  StatusCode sc(StatusCode::SUCCESS);

  if ( m_useIPVOfflineTool )
  {
    LHCb::Track::ConstVector tracks;
    getTracks( p, tracks );
    if ( !tracks.empty() )
    {
      reFittedVertex = new LHCb::RecVertex();
      sc = m_pvOfflineTool->reDoSinglePV( v->position(),
                                          tracks,
                                          *reFittedVertex );
    }
    else
    {
      reFittedVertex = new LHCb::RecVertex(*v);
    }
  }
  else
  {
    reFittedVertex = new LHCb::RecVertex(*v);
    sc = m_pvReFitter->remove( p, reFittedVertex );
  }

  if ( sc.isSuccess() && reFittedVertex )
  {
    return reFittedVertex;
  }
  else
  {
    delete reFittedVertex;
    return NULL;
  }

}

//=============================================================================

void PVReFitterAlg::getTracks(const LHCb::Particle* p,
                              LHCb::Track::ConstVector& tracks) const
{
  const LHCb::ProtoParticle * proto = p->proto() ;
  if ( proto )
  {
    const LHCb::Track* track = proto->track();
    if ( track ) tracks.push_back(track);
  }
  else
  {
    const SmartRefVector< LHCb::Particle >& Prods = p->daughters();
    for ( SmartRefVector< LHCb::Particle >::const_iterator iProd = Prods.begin();
          iProd != Prods.end(); ++iProd )
    {
      const LHCb::Particle* daughter = *iProd;
      getTracks(daughter, tracks);
    }
  }
}

//=============================================================================
