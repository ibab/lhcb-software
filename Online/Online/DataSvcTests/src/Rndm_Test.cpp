#include "GaudiKernel/SmartIF.h"
#include "DataSvcTests/RndmNumberTest.h"

namespace Tests {
  /** Definition of a small class to test the random number generator service.
      
  @author M.Frank
  @date   11/2/99
  
  */
  class Rndm_Test1   : public RndmNumberTest   {
  public:
    /// Standard constructor
    Rndm_Test1(bool dbg) : RndmNumberTest("Rndm_Test1", dbg) {}
    /// Standard destructor
    virtual ~Rndm_Test1()    {}
    /// action methid
    virtual void run();
  };
}

using namespace Tests;


void Rndm_Test1::run() {
  RndmNumberTest::test(5000);
}

int rndmsvc_entry(int argc, char** /* argv */)    {
  bool dbg = argc > 1;
  { 
    Rndm_Test1 tst(dbg);
    tst.run();
  }
  return 0x0;
}
