// $Id: OTEffCalculator.cpp,v 1.9 2006-05-10 16:09:45 cattanem Exp $

// Gaudi files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// MCEvent
#include "Event/MCOTDeposit.h"

/// GSL
#include "gsl/gsl_math.h"

// OTSimulation
#include "OTEffCalculator.h"

/** @file OTEffCalculator.cpp 
 *
 *  Implementation of OTEffCalculator tool
 *  
 *  @author M. Needham
 *  @date   19-09-2000
 */

using namespace LHCb;

// Declaration of the tool Factory
DECLARE_TOOL_FACTORY( OTEffCalculator );

OTEffCalculator::OTEffCalculator(const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_genEff(0),
  m_cellRadius(2.5*Gaudi::Units::mm) 
{ 
  // constructor
  // jobOptions
  declareProperty("etaZero", m_etaZero = 0.99); // Test Beam 2000 & 2005
  
  // TestBeam 2000 Ar/CF4/Co2: 70/15/10
  //declareProperty("rho", m_rho = 1.47/mm);
  
  // Test Beam 2005 Ar/Co2 : 70/30
  declareProperty("rho", m_rho = 3.333/Gaudi::Units::mm); 

  declareInterface<IOTEffCalculator>(this);
}

StatusCode OTEffCalculator::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) return Error( "Failed to initialize base class", sc );
  
  // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  sc = serviceLocator()->service( "RndmGenSvc", randSvc, true );
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve random number service",sc);
  }

  // get interface to generator
  sc = randSvc->generator(Rndm::Flat(0.,1.),m_genEff.pRef()); 
  if( sc.isFailure() ) {
    return Error ("Failed to generate random number distribution",sc);
  }
  randSvc->release();

  return StatusCode::SUCCESS;  
}

OTEffCalculator::~OTEffCalculator()
{
  //destructor
  // release interface
  //  m_genEff->release();
}

StatusCode OTEffCalculator::calculate(MCOTDeposit* aDeposit, bool& iAccept)
{
  // get a random number
  double testVal = m_genEff->shoot();
  if (testVal < effParamFunc(aDeposit->driftDistance()) ) {
    iAccept = true;
  } else {
    iAccept = false;
  } //  test

  return StatusCode::SUCCESS;
}

double OTEffCalculator::effParamFunc(const double driftDistance)
{
  // efficiency function parameterization
  
  // calculate the track path Length in the cell
  double pathLength2 = gsl_pow_2(m_cellRadius) - gsl_pow_2(driftDistance);

  // eff - return val
  double eff = 0.;

  if (pathLength2 > 0.0) {
    eff = m_etaZero*(1.0-exp(-2.0*m_rho*sqrt(pathLength2)));
  } else {
    // efficiency 0 outside cell
  }

  return eff;
}
