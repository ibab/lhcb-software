// Include files 

#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// local
#include "ExampleAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ExampleAlg
//
// 2006-01-22 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ExampleAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExampleAlg::ExampleAlg( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_myCond1(NULL), m_myCond2(NULL), m_myAlignment(NULL),
    m_cond1(0.0), m_cond2()
{

}
//=============================================================================
// Destructor
//=============================================================================
ExampleAlg::~ExampleAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ExampleAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  try {
    registerCondition("Conditions/Environment/MyDet/Cond1",
                      m_myCond1, &ExampleAlg::i_cacheCondition1Data);
    registerCondition("Conditions/ReadoutConf/MyDet/Cond2",
                      m_myCond2, &ExampleAlg::i_cacheCondition2Data);
    registerCondition("Conditions/Alignment/MyDet/Alignment",
                      m_myAlignment, &ExampleAlg::i_cacheAlignmentData);
  }
  catch (GaudiException &e){
    fatal() << e << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ExampleAlg::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Cond1
//=========================================================================
StatusCode ExampleAlg::i_cacheCondition1Data() {
  m_cond1 = m_myCond1->param<double>("value");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Cond2
//=========================================================================
StatusCode ExampleAlg::i_cacheCondition2Data() {
  m_cond2 = m_myCond2->paramVect<int>("list");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Extract data from Alignment
//=========================================================================
StatusCode ExampleAlg::i_cacheAlignmentData() {
  // ...
  return StatusCode::SUCCESS;
}

//=============================================================================
