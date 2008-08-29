// $Id: LumiPutResult.cpp,v 1.1 2008-08-29 07:52:45 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "HltBase/ANNSvc.h"
#include "Event/HltLumiResult.h"
#include "boost/foreach.hpp"

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

  m_items = svc<IANNSvc>("LumiANNSvc")->items("LumiCounters");
  m_size = m_items.size();
  // get arrays
  m_means = new double[m_size];            // create a fixed location for DIM to look at
  m_thresholds = new double[m_size];       // create a fixed location for DIM to look at
  m_infoKeys = new unsigned int[m_size];   // corresponding key in the info 

  int i=0;
  BOOST_FOREACH( IANNSvc::minor_value_type iKey, m_items )  {
    // declare all possible counters
    std::string name  = iKey.first;
    m_infoKeys[i] = iKey.second;
    // announce the values
    declareInfo("COUNTER_TO_RATE["+name+"_mean]", m_means[i], "mean of "+name);
    declareInfo("COUNTER_TO_RATE["+name+"_threshold]", m_thresholds[i], "fraction over threshold of "+name);
    info() << "counter " << name << " declared at " << i << " with key " << m_infoKeys[i] << endmsg;
    i++;
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
      debug() << "Key: " << key << " mean: " << mean << " threshold " << threshold << endmsg;
    }
  }

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

