// $Id: MultiDBTest.h,v 1.1 2005-05-12 16:34:22 marcocle Exp $
#ifndef MULTIDBTEST_H 
#define MULTIDBTEST_H 1

// forward declarations
class ICondDBAccessSvc;

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class MultiDBTest MultiDBTest.h
 *  
 *  Algorithm to test the multi-db functionality.
 *
 *  @author Marco Clemencic
 *  @date   2005-05-12
 */
class MultiDBTest : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MultiDBTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MultiDBTest( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  ICondDBAccessSvc *m_dbAccSvc1, *m_dbAccSvc2;
  
};
#endif // MULTIDBTEST_H
