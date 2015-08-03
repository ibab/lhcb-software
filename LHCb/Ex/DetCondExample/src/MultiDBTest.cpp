// Include files 

#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// local
#include "MultiDBTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MultiDBTest
//
// 2005-05-12 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MultiDBTest )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultiDBTest::MultiDBTest( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_cond1(NULL),m_cond2(NULL),m_align1(NULL)
{}
//=============================================================================
// Destructor
//=============================================================================
MultiDBTest::~MultiDBTest() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MultiDBTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  try {

    info() << "*** register conditions ***" << endmsg;
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond1",m_cond1);
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond2",m_cond2);
    registerCondition<MultiDBTest>("/dd/multiDBTest/Align1",m_align1);

  }
  catch (GaudiException){
    return StatusCode::FAILURE;
  }
  return runUpdate();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MultiDBTest::execute() {

  info() << "*************** execute(): process new event ***************" << endmsg;
  
  info() << "Cond1: " << m_cond1->validSince() << " -> " << m_cond1->validTill() << endmsg;
  info() << "       DB = " << m_cond1->paramAsString("Database") << endmsg;
  
  info() << "Cond2: " << m_cond2->validSince() << " -> " << m_cond2->validTill() << endmsg;
  info() << "       DB = " << m_cond2->paramAsString("Database") << endmsg;

  info() << "Align1: " << m_align1->validSince() << " -> " << m_align1->validTill() << endmsg;
  info() << m_align1->offNominalMatrix() << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
