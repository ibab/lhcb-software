// $Id: ResetPVLocation.cpp,v 1.1 2004-10-27 13:49:59 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from Event
#include "Event/Vertex.h" 

// local
#include "ResetPVLocation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ResetPVLocation
//
// 2004-10-27 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<ResetPVLocation>          s_factory ;
const        IAlgFactory& ResetPVLocationFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResetPVLocation::ResetPVLocation( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_PVLocator()
{
    declareProperty( "PVLocation", m_PVLocation = VertexLocation::Primary);
    declareProperty( "ResetToOffline", m_reset = false );
}
//=============================================================================
// Destructor
//=============================================================================
ResetPVLocation::~ResetPVLocation() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ResetPVLocation::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  sc = toolSvc()->retrieveTool( "PVLocator", m_PVLocator);
  if(sc.isFailure()){
    err() << " Unable to retrieve PV Locator tool" << endreq;
    return sc;
  }
  if ( m_reset && ( m_PVLocation != VertexLocation::Primary)){
    err() << "You cannot reset PV to offline PV and to " << 
      VertexLocation::Primary << " at the same time" << endreq;
    return  StatusCode::FAILURE;
  }
  if ( m_reset ) m_PVLocation = VertexLocation::Primary ; // already the case but anyway
  info() << "This algorithm will reset PV to " << m_PVLocation << endreq ;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode ResetPVLocation::execute() {

  debug() << "==> Execute" << endmsg;
  StatusCode sc = m_PVLocator->setPVLocation( m_PVLocation );
  if ( sc.isFailure() ){
    warning() << "PVLocator is unhappy" << endmsg ;
  } ;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ResetPVLocation::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
