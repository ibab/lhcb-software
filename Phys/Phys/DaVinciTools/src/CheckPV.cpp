// $Id: CheckPV.cpp,v 1.1 2004-11-18 07:56:51 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "DaVinciTools/IPVLocator.h"

// local
#include "CheckPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckPV
//
// 2004-11-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CheckPV>          s_factory ;
const        IAlgFactory& CheckPVFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckPV::CheckPV( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_PVLocator()
{

}
//=============================================================================
// Destructor
//=============================================================================
CheckPV::~CheckPV() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckPV::initialize() {

  debug() << "==> Initialize" << endmsg;

  m_PVLocator = tool<IPVLocator>("PVLocator");
  if( !m_PVLocator ){
    err() << " Unable to retrieve PV Locator tool" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckPV::execute() {

  debug() << "==> Execute" << endmsg;

  std::string m_PVContainer = m_PVLocator->getPVLocation() ;
  
  verbose() << "Getting PV from " << m_PVContainer << endreq ;  
  Vertices* PV = get<Vertices>(m_PVContainer);

  if ( !PV ) {
    err() << "Could not find primary vertex location " 
          <<  m_PVContainer << endreq;
    return StatusCode::FAILURE ;
  }
  if ( PV->empty() ) {
    warning() << "PV container is empty " << endreq;
    setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  } else {
    debug() << "There are " << PV->size() << " primary vertices." << endreq ;
    setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckPV::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
