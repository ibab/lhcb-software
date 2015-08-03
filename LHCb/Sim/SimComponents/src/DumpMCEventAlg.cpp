// $Id: DumpMCEventAlg.cpp,v 1.2 2007-01-12 10:01:00 cattanem Exp $
// Include files 
// Tools
#include "Kernel/ICheckTool.h"

// Event Model
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "DumpMCEventAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpMCEventAlg
//
// 2004-07-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(DumpMCEventAlg)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpMCEventAlg::DumpMCEventAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_checker(0)
{
  declareProperty( "NumberOfObjectsToPrint", m_numObjects = 5 );
}
//=============================================================================
// Destructor
//=============================================================================
DumpMCEventAlg::~DumpMCEventAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpMCEventAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_checker = tool<ICheckTool>( "CheckMCEventTool" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpMCEventAlg::execute() {

  LHCb::MCHeader* evt = 
    get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default );
  info() << "++++++++++++++++++++++++++++++++++++++++++++++++++++" 
         << std::endl 
         << *evt << endmsg;
  
  StatusCode sc = m_checker->check();
  if( sc.isFailure() ) return sc;
  info() << "MCVertex/MCParticle tree structure is OK" << endmsg;
  
  LHCb::MCVertices* verts = 
    get<LHCb::MCVertices>( LHCb::MCVertexLocation::Default );
  info() << "There are " << verts->size() << " Vertices:" << endmsg;
  
  if( 0 < m_numObjects && msgLevel( MSG::DEBUG ) ) {
    unsigned int count = 0;
    LHCb::MCVertices::iterator iVert;
    for( iVert = verts->begin(); iVert != verts->end(); iVert++ ) {
      if( !msgLevel(MSG::VERBOSE) && m_numObjects < ++count ) break;
      debug() << "MCVertex " << (*iVert)->key() << ":" << std::endl
              << (*iVert)->fillStream(debug().stream()) << endmsg;
    }
  }
  
  LHCb::MCParticles* parts = 
    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  info() << "There are " << parts->size() << " Particles:" << endmsg;

  if( 0 < m_numObjects && msgLevel( MSG::DEBUG ) ) {
    unsigned int count = 0;
    LHCb::MCParticles::iterator iPart;
    for( iPart = parts->begin(); iPart != parts->end(); iPart++ ) {
      if( !msgLevel(MSG::VERBOSE) && m_numObjects < ++count ) break;
      debug() << "MCParticle " << (*iPart)->key() << ":" << std::endl
              << (*iPart)->fillStream(debug().stream()) << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
