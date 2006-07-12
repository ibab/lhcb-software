// $Id: MultiDBTest.cpp,v 1.9 2006-07-12 18:18:13 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "DetDesc/Condition.h"

// local
#include "MultiDBTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MultiDBTest
//
// 2005-05-12 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MultiDBTest );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MultiDBTest::MultiDBTest( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{}
//=============================================================================
// Destructor
//=============================================================================
MultiDBTest::~MultiDBTest() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MultiDBTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  try {

    info() << "*** register conditions ***" << endreq;
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond1",m_cond1);
    registerCondition<MultiDBTest>("/dd/multiDBTest/Cond2",m_cond2);

  }
  catch (GaudiException){
    return StatusCode::FAILURE;
  }
  return runUpdate();
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MultiDBTest::execute() {

  info() << "*************** execute(): process new event ***************" << endmsg;
  
  info() << "Cond1: " << m_cond1->validSince() << " -> " << m_cond1->validTill() << endmsg;
  info() << "       DB = " << m_cond1->paramAsString("Database") << endmsg;
  
  info() << "Cond2: " << m_cond2->validSince() << " -> " << m_cond2->validTill() << endmsg;
  info() << "       DB = " << m_cond2->paramAsString("Database") << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MultiDBTest::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
