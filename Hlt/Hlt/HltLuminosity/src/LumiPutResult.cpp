// $Id: LumiPutResult.cpp,v 1.2 2009-02-18 13:11:13 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/LumiCounters.h"
#include "Event/HltLumiResult.h"

// local
#include "LumiPutResult.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiPutResult
//
// 2008-08-27 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiPutResult );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiPutResult::LumiPutResult( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputContainer", m_InputContainerName = LHCb::HltLumiResultLocation::Default );
}


//=============================================================================
// Destructor
//=============================================================================
LumiPutResult::~LumiPutResult() {
  // first release DIM

  // how????

  // then release storage
  delete[] m_means;
  delete[] m_thresholds;
  delete[] m_infoKeys;
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiPutResult::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  info() << "InputContainer   " << m_InputContainerName << endmsg;

  // first count number of valid keys
  m_size = 0;
  for ( int iKey = 0; iKey != LHCb::LumiCounters::LastGlobal; iKey++ ) {
    if ( iKey != LHCb::LumiCounters::Unknown ) {
      m_size++;
    }
  }

  // get arrays
  m_means = new double[m_size];            // create a fixed location for DIM to look at
  m_thresholds = new double[m_size];       // create a fixed location for DIM to look at
  m_infoKeys = new unsigned int[m_size];   // corresponding key in the info 

  int i=0;
  for ( int iKey = 0; iKey != LHCb::LumiCounters::LastGlobal; iKey++ ) {
    if ( iKey != LHCb::LumiCounters::Unknown ) {
      // declare all possible counters
      std::string name = LHCb::LumiCounters::counterKeyToString(iKey);
      m_infoKeys[i] = iKey;
      // announce the values
      declareInfo("COUNTER_TO_RATE["+name+"_mean]", m_means[i], "mean of "+name);
      declareInfo("COUNTER_TO_RATE["+name+"_threshold]", m_thresholds[i], "fraction over threshold of "+name);
      info() << "counter " << name << " declared at " << i << " with key " << m_infoKeys[i] << endmsg;
      i++;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiPutResult::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  // get container
   LHCb::HltLumiResult* result = getOrCreate<HltLumiResult,HltLumiResult>(m_InputContainerName);

  // read the contents and publish the values which exist
  for ( unsigned int i=0; i < m_size; i++ ) {
    unsigned key = m_infoKeys[i];
    double mean = result->info(key+100, -10000.);
    if ( mean != -10000. ) { 
      m_means[i] = mean;
      double threshold = result->info(key+200, -10000.);
      m_thresholds[i] = threshold;
       if ( msgLevel(MSG::DEBUG) ) debug() << "Key: " << key << " mean: " << mean << " threshold " << threshold << endmsg;
    }
  }

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

