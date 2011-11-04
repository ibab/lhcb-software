
// Framework include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Example related include files
#include "DataSvcTests/RndmNumberTest.h"

using namespace Tests;

RndmNumberTest::RndmNumberTest(const std::string& nam, bool dbg) 
: UnitTest(nam, dbg)
{
  m_pRN  = 0;
  // Create object instance
  StatusCode status = getService("RndmGenSvc", m_pRN);
  if ( !status.isSuccess() )    {
    m_nerr++;
    std::cout << "Service: RndmGenSvc has NO IID_IRndmGenSvc. " << std::endl;
  }
  else if ( m_debug )   {
    std::cout << "Service: RndmGenSvc has IID_IRndmGenSvc." << std::endl;    
  }
  if ( m_debug )    {
    std::cout << "+===============================================================+" << std::endl;
    std::cout << "|             RndmNumberTest initialized                        |" << std::endl;
    std::cout << "+===============================================================+" << std::endl;
  }
}

// Standard destructor
RndmNumberTest::~RndmNumberTest()   {
}

//======================================================================
// Test Generator access
//======================================================================
StatusCode RndmNumberTest::testGetRandomizer(long expected, 
                                             const IRndmGen::Param& par,
                                             SmartIF<IRndmGen>& gen)    {
  StatusCode status = m_pRN->generator( par, gen.pRef() );
  if ( check(status, expected) )      {
    std::cout << name() << " FAILED to get randomizer of type " 
              << System::typeinfoName(typeid(par)) << std::endl;
  }
  return status;
}

StatusCode RndmNumberTest::testShootArray(IRndmGen* gen, long count)  {
  double sum = 0.0;
  std::vector<double> numbers;
  StatusCode status = gen->shootArray(numbers, count);
  if ( check(status, StatusCode::SUCCESS) )      {
    std::cout << name() << " FAILED to shoot array of type " 
              << System::typeinfoName(typeid(*gen)) << std::endl;
  }
  for ( unsigned int i = 0; i < numbers.size(); i++ )
    sum += numbers[i];

  if ( sum == 0.0 && count > 0 )   {
    std::cout << name() << "FAILED: I shot " << count 
              << " Random numbers, but the sum is ZERO!" << std::endl;
    return StatusCode::FAILURE;
  }
  return status;
}

StatusCode RndmNumberTest::testShootNumbers(IRndmGen* gen, long count)  {
  double sum = 0.0;
  for ( long int i = 0; i < count; i++ )    {
    sum += gen->shoot();
  }
  if ( sum == 0.0 && count > 0 )   {
    std::cout << name() << "FAILED: I shot " << count 
              << " Random numbers, but the sum is ZERO!" << std::endl;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void RndmNumberTest::shutdown()   {
  UnitTest::finalize();
  UnitTest::shutdown();
}

// Run simple test suite
StatusCode RndmNumberTest::runSimple ( const IRndmGen::Param& par, long /* count */ )   {
  // Get random number generator:
  SmartIF<IRndmGen> gen;
  StatusCode status = testGetRandomizer(StatusCode::SUCCESS, par, gen);
  if ( status.isSuccess() )  {
    testShootNumbers(gen, 5000);
    testShootArray(gen, 5000);
  }
  else    {
    std::cout << name() << "Simple test with " 
              << System::typeinfoName(typeid(par)) 
              << " FAILED...." 
              << std::endl;
  }
  return status;
}

// Run simple test suite
void RndmNumberTest::test (long count)    {
  std::vector<double> pdf;
  pdf.push_back(1.0);
  pdf.push_back(2.0);
  pdf.push_back(3.0);
  pdf.push_back(4.0);
  pdf.push_back(5.0);
  pdf.push_back(6.0);
  runSimple( Rndm::Gauss(5,1),          count);
  runSimple( Rndm::Exponential(1),      count);
  runSimple( Rndm::Chi2(5),             count);
  runSimple( Rndm::Landau(5,1), count);
  runSimple( Rndm::BreitWigner(91,2.7), count);
  runSimple( Rndm::BreitWignerCutOff(91,2.7,1000), count);
  runSimple( Rndm::StudentT(1), count);
  runSimple( Rndm::Gamma(1,2), count);
  runSimple( Rndm::Poisson(1), count);
  runSimple( Rndm::Binomial(1000,0.1), count);
  runSimple( Rndm::Poisson(1), count);
  runSimple( Rndm::Flat(0,1), count);
  runSimple( Rndm::DefinedPdf(pdf, true), count);
  shutdown();
}
