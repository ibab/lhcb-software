// $Id$
#ifndef TESTFTHITS_H 
#define TESTFTHITS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class TestFTHits TestFTHits.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2012-05-15
 */
class TestFTHits : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TestFTHits( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestFTHits( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // TESTFTHITS_H
