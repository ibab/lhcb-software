// $Id: $

// local
#include "RecProcessingTimeMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecProcessingTimeMoni
//
// 2010-07-15 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecProcessingTimeMoni );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecProcessingTimeMoni::RecProcessingTimeMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "Algorithms",   m_algNames );
  declareProperty( "MaxEventTime", m_maxTime = 10000  );
}

//=============================================================================
// Destructor
//=============================================================================
RecProcessingTimeMoni::~RecProcessingTimeMoni() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecProcessingTimeMoni::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if ( sc.isFailure() ) return sc;  

  // are we properly configured
  if ( m_algNames.empty() ) { sc = Warning( "No algorithms to time !"); }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RecProcessingTimeMoni::execute() 
{

  // Loop over algorithms to include in the timing and add them up
  double time = 0;
  for ( AlgorithmNames::const_iterator name = m_algNames.begin();
        name != m_algNames.end(); ++name )
  {
    time += chronoSvc()->chronoDelta((*name)+":execute",IChronoStatSvc::ELAPSED)/1000;
  }
  
  plot1D( time, "overallTime", "Overall Event Processing Time (ms)", 0, m_maxTime, 100 );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
