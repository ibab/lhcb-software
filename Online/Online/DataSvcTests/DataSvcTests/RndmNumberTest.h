// $Header: /local/gaudicvs/GaudiExamples/src/RandomNumber/RndmNumberTest.h,v 1.2 2001/11/22 09:02:16 mato Exp $	//
#ifndef TESTS_RNDMNUMBERTEST_H
#define TESTS_RNDMNUMBERTEST_H

// Framework include files
#include "DataSvcTests/UnitTest.h"
#include "GaudiKernel/RndmGenerators.h"

class IRndmGenSvc;
template <class T> class SmartIF;

namespace Tests {

  /** @class RndmNumberTest 
      A small algorithm class using the random number service

      @author M.Frank
      @date 1999
  */
  class RndmNumberTest : public UnitTest     {
    IRndmGenSvc* m_pRN;
  public:
    /// Constructor: A constructor of this form must be provided.
    RndmNumberTest(const std::string& name, bool dbg); 
    /// Standard Destructor
    virtual ~RndmNumberTest();
    /// Shutdown the test...
    virtual void shutdown();
    /// Shoot individual numbers
    StatusCode testShootNumbers(IRndmGen* gen, long count);
    /// Shoot into array
    StatusCode testShootArray(IRndmGen* gen, long count);
    /// Get random number generator
    StatusCode testGetRandomizer(long expected, 
				 const IRndmGen::Param& par,
				 SmartIF<IRndmGen>& gen);
    /// Run simple suite...
    StatusCode runSimple ( const IRndmGen::Param& par, long count);
    /// Run full suite...
    void test (long count);
  };
}
#endif // TESTS_RNDMNUMBERTEST_H
