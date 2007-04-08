// $Id: OTEffCalculator.cpp,v 1.11 2007-04-08 16:54:51 janos Exp $

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
#include "gsl/gsl_sf_exp.h"

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
  if ( sc.isFailure() ) return Error( "Failed to initialize OTEffCalculator", sc );
  
  // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  sc = serviceLocator()->service( "RndmGenSvc", randSvc, true );
  if ( sc.isFailure() ) {
    return Error ("Failed to retrieve random number service",sc);
  }

  // get interface to generator
  sc = randSvc->generator(Rndm::Flat(0.,1.),m_genEff.pRef()); 
  if ( sc.isFailure() ) {
    return Error ("Failed to generate random number distribution",sc);
  }
  randSvc->release();

  return StatusCode::SUCCESS;  
}

OTEffCalculator::~OTEffCalculator()
{
  //destructor
}

double OTEffCalculator::effParamFunc(const double driftDistance)
{
  // efficiency function parameterization
  // calculate the track path Length in the cell
  double pathLength2 = gsl_pow_2(m_cellRadius) - gsl_pow_2(driftDistance);

  // if pathLength2 > 0.0 return eff parm else 0 (driftdistance > cell radius)
  return (pathLength2 > 0.0?m_etaZero*(1.0-gsl_sf_exp(-2.0*m_rho*std::sqrt(pathLength2))):0.0);
}

void OTEffCalculator::calculate(MCOTDeposit* aDeposit, bool& accept)
{
  // get a random number
  double testVal = m_genEff->shoot();
  // to accpet or not to accept
  accept = (testVal < effParamFunc(aDeposit->driftDistance()));
}
