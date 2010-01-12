// $Id: TestConditionAlg.cpp,v 1.2 2010-01-12 19:06:27 marcocle Exp $

// Include files
#include "GaudiKernel/Map.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h" 
#include "DetDesc/Condition.h"

#include <vector>

namespace DetCondTest {

/** @class TestConditionAlg TestConditionAlg.h component/TestConditionAlg.h
 *  
 *  Simple algorithm that prints the requested conditions at every event.
 * 
 *  @author Marco CLEMENCIC
 *  @date   2008-06-27
 */
class TestConditionAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestConditionAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestConditionAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  void i_dump();

  /// Names of the conditions to print
  std::vector<std::string> m_condPaths;

  /// Flag to decide if the conditions have to be loaded also during the
  /// initialize step.
  bool m_loadAtInit;
  
  /// Container of the conditions to print
  GaudiUtils::Map<std::string,Condition*> m_conditions;
  
};

//-----------------------------------------------------------------------------
// Implementation file for class : TestConditionAlg
//
// 2008-06-27 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestConditionAlg::TestConditionAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("Conditions", m_condPaths,
      "list of paths to conditions in the detector transient store");
  declareProperty("LoadDuringInitialize", m_loadAtInit = false,
      "load the requested conditions already duting the initialization");
}
//=============================================================================
// Destructor
//=============================================================================
TestConditionAlg::~TestConditionAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestConditionAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  std::vector<std::string>::iterator path;
  for (path = m_condPaths.begin(); path != m_condPaths.end(); ++path){
    registerCondition<TestConditionAlg>(*path,m_conditions[*path],NULL);
  }
  
  if (m_loadAtInit) {
    StatusCode sc = updMgrSvc()->update(this);
    info() << "Conditions loaded at initialize" << endmsg;
    if (sc.isSuccess()){
      i_dump();
    }
    else return sc;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestConditionAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  i_dump();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestConditionAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  updMgrSvc()->unregister(this);
  m_conditions.clear();
  m_condPaths.clear();
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
//  Print the conditions
//=============================================================================
void TestConditionAlg::i_dump() 
{
  info() << "Requested Conditions:\n";
  GaudiUtils::Map<std::string,Condition*>::iterator it;
  for (it = m_conditions.begin(); it != m_conditions.end(); ++it) {
    info() << "--- " << it->first << "\n" << *(it->second) << "\n";
  }
  info() << endmsg;  
}

//=============================================================================

}

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( DetCondTest, TestConditionAlg );
