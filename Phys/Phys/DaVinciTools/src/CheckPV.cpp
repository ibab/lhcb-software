// $Id: CheckPV.cpp,v 1.2 2004-11-18 09:07:25 pkoppenb Exp $
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
  declareProperty( "MinPVs", m_minPV = 1  );
  declareProperty( "MaxPVs", m_maxPV = -1 ); // -1 -> No maximum defined

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
  if ( m_minPV > 0 && m_maxPV > 0 ){
    info() << "will select events with between " << m_minPV << " and " << m_maxPV 
           << " reconstructed PVs" << endreq ;
  } else if ( m_minPV > 0 ){
    info() << "will select events with " << m_minPV 
           << " or more reconstructed PVs" << endreq ;
   } else if ( m_minPV == 0 ){
    info() << "will select events with no reconstructed PVs" << endreq ;
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

  if ( !PV ) { err() << "Could not find primary vertex location " 
                     <<  m_PVContainer << endreq;
    return StatusCode::FAILURE ;
  }
  int n =  PV->size() ;
  debug() << "There are " << n << " primary vertices." << endreq ;

  bool ok = ( n >= m_minPV );      // more than m_minPV
  if ( m_maxPV >= 0 ){              // some maximum?
    ok = (ok && ( n <= m_maxPV ));  // less than m_maxPV
  }

  setFilterPassed(ok);
  if (ok) info() << "Event accepted because there are " << n << " primary vertices." << endreq ;
  else    info() << "Event rejected because there are " << n << " primary vertices." << endreq ; 
  
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
