// $Id: Particle2VertexAsctAlg.cpp,v 1.9 2006-08-22 15:32:15 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from BOOST
#include "boost/lexical_cast.hpp"

// from DaVinciKernel
#include "Kernel/Particle2Vertex.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IParticle2VertexAsct.h"

// local
#include "Particle2VertexAsctAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2VertexAsctAlg
//
// 2006-06-07 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Particle2VertexAsctAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2VertexAsctAlg::Particle2VertexAsctAlg( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty( "InputParticles", m_inputPartLocation );
  declareProperty( "InputVertices", m_inputVertLocation );
  declareProperty( "OutputLocation", m_outputTableLocation );
  declareProperty( "IPToolName", m_ipToolName="GeomDispCalculator" );
  declareProperty( "AsscociatorToolName", 
                   m_asctToolName="Particle2VertexIPSAsct" );

}
//=============================================================================
// Destructor
//=============================================================================
Particle2VertexAsctAlg::~Particle2VertexAsctAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Particle2VertexAsctAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first  
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "===> Initialize" << endmsg;

  sc = toolSvc()->retrieveTool(m_ipToolName, m_ipTool, this);
  if(sc.isFailure()) {
    fatal() << " Unable to retrieve " << m_ipToolName << " tool" << endmsg;
  }
  
  sc = toolSvc()->retrieveTool(m_asctToolName, m_asctTool, this);

  if(sc.isFailure()) {
    fatal() << " Unable to retrieve " << m_asctToolName << " tool" << endmsg;
  }

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2VertexAsctAlg::execute() {

  using namespace LHCb;
  
  debug() << "==> Execute" << endmsg;
  verbose() << "Getting particles from " << m_inputPartLocation << endmsg;
  Particle::Container* particles = 
    get<Particle::Container>(m_inputPartLocation);

  verbose() << "Getting vertices from " << m_inputVertLocation << endmsg;
  VertexBase::Container* vertices = 
    get<VertexBase::Container>(m_inputVertLocation);
  
  if (!particles) {
    return Error("Unable to retrieve Particles from "+m_inputPartLocation);
  }
  if (!vertices) {
    return Error("Unable to retrieve Vertices from "+m_inputVertLocation);
  }

  Particle2Vertex::Table* table = new Particle2Vertex::Table();

  if (0==table) {
    return Error("Something failed in table making");
  } 

  for (Particle::Container::const_iterator iPart = particles->begin();
       iPart != particles->end(); ++iPart) {
    for (VertexBase::Container::const_iterator iVert = vertices->begin();
         iVert!=vertices->end(); ++iVert) {
      double weight = m_asctTool->weight(*iPart, *iVert, m_ipTool);
      table->relate(*iPart, *iVert, weight);
    }
  }
 
  verbose() << "Putting Patricle2Vertex relations table in " 
            << m_outputTableLocation << endmsg;


  put( table, m_outputTableLocation);

  return StatusCode::SUCCESS;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode Particle2VertexAsctAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
