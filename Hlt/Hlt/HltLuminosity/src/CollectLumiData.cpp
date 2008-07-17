// $Id: CollectLumiData.cpp,v 1.1.1.1 2008-07-17 08:50:25 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/IOnOffline.h"
// from LHCB
#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"

// local
#include "CollectLumiData.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CollectLumiData
//
// 2008-07-11 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CollectLumiData );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CollectLumiData::CollectLumiData( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "RZVeloContainer",      m_RZVeloContainerName =    "Hlt/Track/RZVelo");
  declareProperty( "PV2DContainer",        m_PV2DContainerName   =    "Hlt/Vertex/PV2D");

  declareProperty( "OutputContainer",      m_OutputContainerName =    "Hlt/LumiSummary");

}
//=============================================================================
// Destructor
//=============================================================================
CollectLumiData::~CollectLumiData() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CollectLumiData::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  info() << "RZVeloContainer     " << m_RZVeloContainerName << endmsg;
  info() << "PV2DContainer       " << m_PV2DContainerName   << endmsg;

  info() << "OutputContainer     " << m_OutputContainerName << endmsg;
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CollectLumiData::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_printing_verbose = msgLevel( MSG::VERBOSE );
  m_printing_debug   = msgLevel( MSG::DEBUG );
  m_printing_info    = msgLevel( MSG::INFO );

  // ------------------------------------------
  // create output container on the TES
  m_HltLumiSummarys = new LHCb::HltLumiSummarys();
  // locate them in the TES
  put(m_HltLumiSummarys, m_OutputContainerName); 

  // ------------------------------------------
  // load the track objects
  int n_RZVelo =  0;
  if ( !exist<LHCb::Tracks>(m_RZVeloContainerName) ){
    if (m_printing_info) 
      info() << m_RZVeloContainerName << " not found" << endmsg ;
  } else {
    // get the container
    m_RZVelo = get<LHCb::Tracks>(m_RZVeloContainerName);
  
    if ( !m_RZVelo ) { 
      err() << "Could not find location " 
	    <<  m_RZVeloContainerName << endreq;
      return StatusCode::FAILURE ;
    }
    n_RZVelo =  m_RZVelo->size() ;
    if (m_printing_verbose) 
      verbose() << "found " << n_RZVelo << " RZVelo tracks." << endreq ;
  }
  m_nRZVelo = (short) n_RZVelo;
  debug() << "There are " << n_RZVelo << " tracks in " << m_RZVeloContainerName <<  endreq ;

  // ------------------------------------------
  // load the vertex objects
  int n_PV2D =  0;
  if ( !exist<LHCb::RecVertices>(m_PV2DContainerName)){
    if (m_printing_info) 
      info() << m_PV2DContainerName << " not found" << endmsg ;
  } else {  
    m_PV2D = get<LHCb::RecVertices>(m_PV2DContainerName);
    if ( !m_PV2D ) { 
      err() << "Could not find location " 
            <<  m_PV2DContainerName << endreq;
      return StatusCode::FAILURE ;
    }
    n_PV2D =  m_PV2D->size() ;
    if (m_printing_verbose) 
      verbose() << "found " << n_PV2D << " PV2D vertices." << endreq ;
  }
  m_nPV2D = (short) n_PV2D;
  debug() << "There are " << n_PV2D << " vertices in " << m_PV2DContainerName <<  endreq ;

  // ------------------------------------------
  // fill the output container
  collect();

  // ------------------------------------------
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CollectLumiData::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================
//  Collect
//=============================================================================
void CollectLumiData::collect() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Collect" << endmsg;

  // initialize output class
  LHCb::HltLumiSummary* hltLS= new LHCb::HltLumiSummary();

  // add tracks
  hltLS->addInfo(LHCb::HltLumiSummary::RZVelo, m_nRZVelo);
  
  // add vertices
  hltLS->addInfo(LHCb::HltLumiSummary::PV2D, m_nPV2D);

  m_HltLumiSummarys->insert( hltLS );

}
