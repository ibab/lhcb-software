// $Id: TestCacheInjection.h,v 1.1 2005-06-23 15:30:54 marcocle Exp $
#ifndef TESTCACHEINJECTION_H 
#define TESTCACHEINJECTION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class ICondDBAccessSvc;
class IUpdateManagerSvc;

/** @class TestCacheInjection TestCacheInjection.h
 *  
 *  Algorithm that tests the injection of condition object into CondDBAccessSvc's cache.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-20
 */
class TestCacheInjection : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestCacheInjection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestCacheInjection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  StatusCode i_injectData();

  /// Handle to the CondDBAccessSvc
  ICondDBAccessSvc* m_dbAccSvc;

  size_t m_evtCount;

  IUpdateManagerSvc *m_ums;

  virtual StatusCode null_op() { return StatusCode::SUCCESS; }

};
#endif // TESTCACHEINJECTION_H
