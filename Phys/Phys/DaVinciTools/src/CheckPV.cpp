// $Id: CheckPV.cpp,v 1.8 2006-03-06 13:59:49 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/IOnOffline.h"
#include "Event/Vertex.h"
// local

#include "CheckPV.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckPV
//
// 2004-11-18 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CheckPV>          s_factory ;
const        IAlgFactory& CheckPVFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckPV::CheckPV( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_OnOfflineTool()
  , m_nEvent(0)
  , m_nPV(0)
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_OnOfflineTool = tool<IOnOffline>("OnOfflineTool", this);
  if( !m_OnOfflineTool ){
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

  std::string m_PVContainer = m_OnOfflineTool->getPVLocation() ;
  int n = 0 ;
  bool ok = 0 ;
  
  verbose() << "Getting PV from " << m_PVContainer << endreq ;  
  if ( !exist<Vertices>(m_PVContainer)){
    info() << m_PVContainer << " not found" << endmsg ;
    ok = (m_minPV<=0) ; // Ok if no PV required
  } else {  
    Vertices* PV = get<Vertices>(m_PVContainer);
    if ( !PV ) { 
      err() << "Could not find primary vertex location " 
            <<  m_PVContainer << endreq;
      return StatusCode::FAILURE ;
    }
    n =  PV->size() ;
    verbose() << "There are " << n << " primary vertices." << endreq ;
    m_nPV = m_nPV+n ;
    ++m_nEvent ;
    
    ok = ( n >= m_minPV );      // more than m_minPV
    if ( m_maxPV >= 0 ){              // some maximum?
      ok = (ok && ( n <= m_maxPV ));  // less than m_maxPV
    }
  }
  
  setFilterPassed(ok);
  if (ok) debug() << "Event accepted because there are " << n << " primary vertices." << endreq ;
  else    debug() << "Event rejected because there are " << n << " primary vertices." << endreq ; 
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CheckPV::finalize() {

  debug() << "==> Finalize" << endmsg;
  double fpv = (double)m_nPV/(double)m_nEvent ;
  info() << "Seen " << m_nPV << " PVs in " << m_nEvent 
         << " events. Average = " <<  fpv << endreq ;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
