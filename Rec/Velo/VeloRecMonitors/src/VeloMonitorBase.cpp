// Include files 
// -------------

// VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "VeloMonitorBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloMonitorBase
//
// 2008-08-15 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

namespace Velo 
{

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloMonitorBase::VeloMonitorBase( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
  , m_veloDet ( 0 )
  , m_debugLevel(false)
{
  setProperty ( "PropertiesPrint", true ).ignore();
}

//=============================================================================
// Destructor
//=============================================================================
VeloMonitorBase::~VeloMonitorBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloMonitorBase::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  setHistoTopDir( "Velo/" );
  
  if ( propsPrint() ) printProps();

  m_debugLevel = msgLevel( MSG::DEBUG );

  if ( m_debugLevel ) debug() << "==> Initialize" << endmsg;

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloMonitorBase::execute() {

  if ( m_debugLevel ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloMonitorBase::finalize() {

  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
} // end of Velo namespace
