// $Id: CollectLumiData.cpp,v 1.8 2010-05-23 19:36:57 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCB
#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

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
  declareProperty( "PV3DContainer",        m_PV3DContainerName   =    "Hlt/Vertex/PV3D");

  declareProperty( "OutputContainer",      m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );

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
  info() << "PV3DContainer       " << m_PV3DContainerName   << endmsg;

  info() << "OutputContainer     " << m_OutputContainerName << endmsg;


  // ------------------------------------------
  int m_iPV3D = LHCb::LumiCounters::counterKeyToType("PV3D");
  if ( m_iPV3D == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounters not found: " << "PV3D" <<  endmsg;
  } else {
    info() << "LumiCounters::PV3D key value: " << m_iPV3D << endmsg;
  }

  int m_iRZVelo = LHCb::LumiCounters::counterKeyToType("RZVelo");
  if ( m_iRZVelo == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounters not found: " << "RZVelo" <<  endmsg;
  } else {
    info() << "LumiCounters::RZVelo key value: " << m_iRZVelo << endmsg;
  }
  // ------------------------------------------

 
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
  m_HltLumiSummary = new LHCb::HltLumiSummary();
  // locate them in the TES
  put(m_HltLumiSummary, m_OutputContainerName); 

  // ------------------------------------------
  // load the track objects
  int n_RZVelo =  0;
  const LHCb::Tracks* __m_RZVelo = getIfExists<LHCb::Tracks>(m_RZVeloContainerName);
  if ( NULL == __m_RZVelo)
  {
    if (m_printing_info) 
      info() << m_RZVeloContainerName << " not found" << endmsg ;
  } 
  else 
  {
    // get the container
    m_RZVelo = __m_RZVelo;//get<LHCb::Tracks>(m_RZVeloContainerName);
    
    n_RZVelo =  m_RZVelo->size() ;
    if (m_printing_verbose) 
      verbose() << "found " << n_RZVelo << " RZVelo tracks." << endmsg ;
  }
  m_nRZVelo = n_RZVelo;
  if ( m_printing_debug ) debug() << "There are " << n_RZVelo << " tracks in " << m_RZVeloContainerName <<  endmsg ;

  // ------------------------------------------
  // load the vertex objects
  int n_PV3D =  0;
  const LHCb::RecVertices* __m_PV3D = get<LHCb::RecVertices>(m_PV3DContainerName);
  if ( NULL == __m_PV3D)
  {
    if (m_printing_info) 
      info() << m_PV3DContainerName << " not found" << endmsg ;
  } 
  else 
  {  
    m_PV3D = __m_PV3D;
    if ( !m_PV3D ) 
    {
      
      err() << "Could not find location " 
            <<  m_PV3DContainerName << endmsg;
      return StatusCode::FAILURE ;
    }
    n_PV3D =  m_PV3D->size() ;
    if (m_printing_verbose) 
      verbose() << "found " << n_PV3D << " PV3D vertices." << endmsg ;
  }
  m_nPV3D = n_PV3D;
  if ( m_printing_debug ) debug() << "There are " << n_PV3D << " vertices in " << m_PV3DContainerName <<  endmsg ;

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
  LHCb::HltLumiSummary* hltLS=m_HltLumiSummary;

  // add tracks
  hltLS->addInfo( m_iRZVelo, m_nRZVelo);
  
  // add vertices
  hltLS->addInfo( m_iPV3D, m_nPV3D);

}
