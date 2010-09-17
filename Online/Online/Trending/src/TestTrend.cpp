// $Id: TestTrend.cpp,v 1.3 2010-09-17 09:23:13 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TestTrend.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestTrend
//
// 2010-06-03 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestTrend )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestTrend::TestTrend( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  
}
//=============================================================================
// Destructor
//=============================================================================
TestTrend::~TestTrend() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestTrend::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_trend = tool<ITrendingTool>( "TrendingTool", "MyTrend", this );

  std::vector<std::string> tags;
  tags.push_back( "Var_01" );
  tags.push_back( "Var_02" );
  tags.push_back( "Var_03" );
  tags.push_back( "Var_04" );
  tags.push_back( "Var_05" );
  tags.push_back( "Var_06" );
  tags.push_back( "Var_07" );
  tags.push_back( "Var_08" );
  tags.push_back( "Var_09" );
  tags.push_back( "Var_10" );
  tags.push_back( "Var_11" );
  tags.push_back( "Var_12" );
  tags.push_back( "Var_13" );
  tags.push_back( "Var_14" );
  tags.push_back( "Var_15" );
  tags.push_back( "Var_16" );
  tags.push_back( "Var_17" );
  tags.push_back( "Var_18" );
  tags.push_back( "Var_19" );
  tags.push_back( "Var_20" );
  tags.push_back( "Var_21" );
  tags.push_back( "Var_22" );
  tags.push_back( "Var_23" );
  tags.push_back( "Var_24" );
  tags.push_back( "Var_25" );
  tags.push_back( "Var_26" );
  tags.push_back( "Var_27" );
  tags.push_back( "Var_28" );
  tags.push_back( "Var_29" );
  tags.push_back( "Var_30" );
  tags.push_back( "Var_31" );
  tags.push_back( "Var_32" );
  tags.push_back( "Var_33" );
  tags.push_back( "Var_34" );
  tags.push_back( "Var_35" );
  tags.push_back( "Var_36" );
  tags.push_back( "Var_37" );
  tags.push_back( "Var_38" );
  tags.push_back( "Var_39" );
  tags.push_back( "Var_40" );
  tags.push_back( "Var_41" );
  
  int status = m_trend->openWrite( "MyTrend.first", tags );
  info() << "openWrite returned " << status << endmsg;
  if ( !status ) return StatusCode::FAILURE;

  std::vector<float> threshold( 41, 0.5f );
  status = m_trend->setThresholds( threshold );
  if ( !status ) return StatusCode::FAILURE;
  
  status = m_trend->setThreshold( "Var_21", 0.2f );
  if ( !status ) return StatusCode::FAILURE;

  m_event = 0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestTrend::execute() {

  debug() << "==> Execute" << endmsg;

  m_event++;
  std::vector<float> data;
  for ( unsigned int kk=0 ; 41 > kk ; ++kk ) {
    data.push_back( ( 0.1f + 0.001f * kk ) * float( m_event ) );
  }
  int time = 1200000000 + m_event;
  int status = m_trend->write( data, time );
  if ( !status) return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestTrend::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_trend->closeFile();
  
  int status = m_trend->openRead( "MyTrend.first" );
  if ( !status) return StatusCode::FAILURE;
  
  std::vector<std::string> tags;
  status = m_trend->tags( tags );
  if ( !status) return StatusCode::FAILURE;

  info() << "=== Returned " << tags.size() << " tags" << endmsg;
  for ( std::vector<std::string>::const_iterator itS = tags.begin(); tags.end() != itS; ++itS ) {
    info() << "Tag " << *itS << endmsg;
  }
  
  status = m_trend->select( 1200000700, 1200000800, "Var_14" );
  if ( !status) return StatusCode::FAILURE;
  int nbVal = 0;
  int time;
  float value;
  while ( m_trend->nextValue( time, value ) ) {
    nbVal++;
    info() << "At '" <<  m_trend->timeString( time ) << "' value " << value << endmsg;
  }
  info() << "Retrieved " << nbVal << " values for tag." << endmsg;

  status = m_trend->select( 1200095600, 1200095800 );
  if ( !status) return StatusCode::FAILURE;
  nbVal = 0;
  std::vector<float> values;
  while ( m_trend->nextEvent( time, values ) ) {
    nbVal++;
    info() << "At '" <<  m_trend->timeString( time ) << "' values[19] " << values[19] << " values[20] " << values[20] << endmsg;
  }
  info() << "Retrieved " << nbVal << " values for tag." << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
