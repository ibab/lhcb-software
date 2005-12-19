// $Id: LandauTest.cpp,v 1.1.1.1 2005-12-19 15:44:29 mneedham Exp $
//
// This File contains the implementation of the VeloMonitor
// C++ code for 'LHCb Tracking package(s)'
//
//   Author:Matthew Needham
//   Created: 18-05-1999

//#include "TrAlgorithms/VeloMonitor.h"

// Standard C++ Library

// Gaudi
#include "GaudiKernel/AlgFactory.h"

//Random Numbers
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "LandauTest.h"

// Needed for the creation of LandauTest objects.
#include "GaudiKernel/AlgFactory.h"
static const AlgFactory<LandauTest>  s_factory;
const IAlgFactory& LandauTestFactory = s_factory;

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
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
 
  sc = randSvc()->generator(Rndm::Landau(0.226,1.),m_landauDist.pRef());  

  for (int i=0; i<1e7 ; ++i ){
     plot(90+(5*m_landauDist->shoot()),"landau",0.,200.,2000);
  } //i

  // end
 
  return StatusCode::SUCCESS;
  
}

StatusCode LandauTest::execute()
{
  /// Executes for one event.
 
  return StatusCode::SUCCESS;
}



