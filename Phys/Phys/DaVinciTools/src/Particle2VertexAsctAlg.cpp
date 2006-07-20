// $Id: Particle2VertexAsctAlg.cpp,v 1.8 2006-07-20 13:19:21 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from BOOST
#include "boost/lexical_cast.hpp"

// from DaVinciKernel
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

  declareProperty( "InputLocation", m_inputData );
  declareProperty( "OutputLocation", m_outputTableLocation );
//   declareProperty( "UseSignificance", m_useSignificance=true );
//   declareProperty( "MaxToBeAssociated", m_max=-1 );
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

//   m_asctTool->setProperty("UseSignificance", 
//                           boost::lexical_cast<std::string>(m_useSignificance));
//   m_asctTool->setProperty("MaxToBeAssociated", 
//                           boost::lexical_cast<std::string>(m_max) );

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Particle2VertexAsctAlg::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::Particle::ConstVector particles;
  
  StatusCode sc = makeConstVector<LHCb::Particle>(particles, m_inputData);

  if ( sc.isFailure() ) {
    fatal() << "Could not retrieve all particles from TES" << endmsg;
    return sc;
  }
  
  LHCb::RecVertex::ConstVector vertices;
  sc = makeConstVector<LHCb::RecVertex>(vertices, 
                                         LHCb::VertexLocation::Primary);

  if ( sc.isFailure() ) {
        fatal() << "Could not retrieve vertices from " 
                << LHCb::VertexLocation::Primary << endmsg;
        return sc;
  }

  IParticle2VertexAsct::TablePV* table = 
    new IParticle2VertexAsct::TablePV(m_asctTool->table(particles.begin(), 
                                                        particles.end(),
                                                        vertices.begin(), 
                                                        vertices.end(),
                                                        m_ipTool) );

  debug() << "Putting Patricle2Vertex relations table in " 
          << m_outputTableLocation << endmsg;

  if (0==table) {
    verbose() << "Something failed in table making" << endmsg;
  } else {
    verbose() << "Table OK" << endmsg;
  }
  
  
  
  put( table, m_outputTableLocation);

  return StatusCode::SUCCESS;

}
//=============================================================================
template <typename T>
StatusCode 
Particle2VertexAsctAlg::makeConstVector(typename T::ConstVector& things, 
                                        const std::vector<std::string>& locations)
{
  StatusCode sc = StatusCode::SUCCESS;
  for (std::vector<std::string>::const_iterator i= locations.begin(); 
       i!=locations.end(); ++i) {
    StatusCode sc = makeConstVector<T>(things, (*i)+"/Particles");
    if ( sc.isFailure() ) continue;
  }
  return sc;
  
}
//=============================================================================
template <typename T>
StatusCode 
Particle2VertexAsctAlg::makeConstVector(typename T::ConstVector& things, 
                                        const std::string& location)
{
  verbose() << "Getting data from " << location << endmsg;
  typename T::Container*  pThings = get<typename T::Container>(location);

  if (pThings) {
    verbose() << "Found "<< pThings->size() << " things"<< endmsg;
    things.insert( things.begin(), pThings->begin(), pThings->end() );
  } else {
    error() <<"Could not retrieve Things from " << location << endmsg;
    return StatusCode::FAILURE;
  }
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
