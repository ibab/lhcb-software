// $Id$
// Include files

#include "Event/RecVertex.h"
// local

#include "CheckPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckPV
//
// 2004-11-18 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CheckPV )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CheckPV::CheckPV( const std::string& name,
                    ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "MinPVs", m_minPV = 1  , "Minimum nuber of PVs required");
  declareProperty( "MaxPVs", m_maxPV = -1 , "Maximum nuber of PVs required. -1 : no cut.");
  declareProperty( "Print", m_print = false , "Print number of PVs");
  declareProperty( "PVLocation", m_PVContainer = LHCb::RecVertexLocation::Primary );
}

//=============================================================================
// Destructor
//=============================================================================
CheckPV::~CheckPV() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckPV::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( m_minPV > 0 && m_maxPV > 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with between " << m_minPV << " and " << m_maxPV
                                      << " reconstructed PVs" << endreq ;
  } else if ( m_minPV > 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with " << m_minPV
                                      << " or more reconstructed PVs" << endreq ;
  } else if ( m_minPV == 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with no reconstructed PVs" << endreq ;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckPV::execute() 
{
  int n = 0 ;
  bool ok = false ;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Getting PV from " << m_PVContainer << endreq ;

  // load the PVs. NULL if not available.
  const LHCb::RecVertices* PV = getIfExists<LHCb::RecVertices>(m_PVContainer);

  if ( !PV )
  {
    if (msgLevel(MSG::DEBUG)) debug() << m_PVContainer << " not found" << endmsg ;
    ok = ( m_minPV <= 0 ) ; // Ok if no PV required
  }
  else 
  {
    // count PVs
    n = PV->size() ;

    if (msgLevel(MSG::VERBOSE) || m_print ) 
      verbose() << "There are " << n << " primary vertices." << endreq ;
    
    counter("PVs") += n;

    ok = ( n >= m_minPV );      // more than m_minPV
    if ( m_maxPV >= 0 )         // some maximum?
    { 
      ok = (ok && ( n <= m_maxPV ));  // less than m_maxPV
    }
  }

  counter("Events") += ok;

  if (msgLevel(MSG::DEBUG))
  {
    if (ok) debug() << "Event accepted because there are " << n << " primary vertices." << endreq ;
    else    debug() << "Event rejected because there are " << n << " primary vertices." << endreq ;
  }

  setFilterPassed(ok);

  return StatusCode::SUCCESS;
}

//=============================================================================
