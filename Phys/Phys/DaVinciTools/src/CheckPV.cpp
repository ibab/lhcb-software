// $Id$
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/IOnOffline.h"
#include "Event/RecVertex.h"
// local

#include "CheckPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckPV
//
// 2004-11-18 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory 
DECLARE_ALGORITHM_FACTORY( CheckPV );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckPV::CheckPV( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_onOfflineTool()
{
  declareProperty( "MinPVs", m_minPV = 1  , "Minimum nuber of PVs required");
  declareProperty( "MaxPVs", m_maxPV = -1 , "Maximum nuber of PVs required. -1 : no cut."); 
  declareProperty( "Print", m_print = false , "Print number of PVs");

}
//=============================================================================
// Destructor
//=============================================================================
CheckPV::~CheckPV() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CheckPV::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_onOfflineTool = tool<IOnOffline>("OnOfflineTool", this);
  if( !m_onOfflineTool ){
    err() << " Unable to retrieve PV Locator tool" << endreq;
    return StatusCode::FAILURE;
  }
  if ( m_minPV > 0 && m_maxPV > 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with between " << m_minPV << " and " << m_maxPV 
           << " reconstructed PVs" << endreq ;
  } else if ( m_minPV > 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with " << m_minPV 
           << " or more reconstructed PVs" << endreq ;
   } else if ( m_minPV == 0 ){
    if (msgLevel(MSG::DEBUG)) debug() << "will select events with no reconstructed PVs" << endreq ;
  }    

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckPV::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  const std::string& m_PVContainer = m_onOfflineTool->primaryVertexLocation() ;
  int n = 0 ;
  bool ok = 0 ;
  
  if (msgLevel(MSG::VERBOSE)) verbose() << "Getting PV from " << m_PVContainer << endreq ;  
  if ( !exist<LHCb::RecVertices>(m_PVContainer)){
    if (msgLevel(MSG::DEBUG)) debug() << m_PVContainer << " not found" << endmsg ;
    ok = (m_minPV<=0) ; // Ok if no PV required
  } else {  
    LHCb::RecVertices* PV = get<LHCb::RecVertices>(m_PVContainer);
    if ( !PV ) { 
      err() << "Could not find primary vertex location " 
            <<  m_PVContainer << endreq;
      return StatusCode::FAILURE ;
    }
    n =  PV->size() ;
    if (msgLevel(MSG::VERBOSE) || m_print ) info() << "There are " << n << " primary vertices." << endreq ;
    counter("PVs")+=n;
    
    ok = ( n >= m_minPV );      // more than m_minPV
    if ( m_maxPV >= 0 ){              // some maximum?
      ok = (ok && ( n <= m_maxPV ));  // less than m_maxPV
    }
  }
  counter("Events")+=ok;
  if (msgLevel(MSG::DEBUG)){
    if (ok) debug() << "Event accepted because there are " << n << " primary vertices." << endreq ;
    else    debug() << "Event rejected because there are " << n << " primary vertices." << endreq ; 
  }
  
  setFilterPassed(ok);
  return StatusCode::SUCCESS;
  
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckPV::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
