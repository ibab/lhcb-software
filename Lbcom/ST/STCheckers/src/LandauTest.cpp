// Random Numbers
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// local
#include "LandauTest.h"

DECLARE_ALGORITHM_FACTORY( LandauTest )

///_________________________________________________
/// Test of Landau generator

LandauTest::LandauTest(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiHistoAlg(name, pSvcLocator)
{
  /// constructor
}

LandauTest::~LandauTest()
{
  /// destructor
}

StatusCode LandauTest::initialize()
{ 
  // Initialize 
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Initialize the Landau function
  sc = randSvc()->generator(Rndm::Landau(0.226,1.), m_landauDist.pRef());  

  // Sample the Landau funtion 10M times and plot it
  for (int i=0; i<1e7 ; ++i ){
     plot(90+(5*m_landauDist->shoot()),"Landau",0.,200.,2000);
  }

  return sc;
}

StatusCode LandauTest::execute()
{
  return StatusCode::SUCCESS;
}
