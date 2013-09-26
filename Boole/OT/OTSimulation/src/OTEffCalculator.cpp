// Gaudi files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

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
DECLARE_TOOL_FACTORY( OTEffCalculator )

OTEffCalculator::OTEffCalculator(const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_genEff( (IRndmGen*)0 ),
  m_cellRadius(2.45*Gaudi::Units::mm) 
{ 
  // constructor
  // jobOptions
  declareProperty("etaZero", m_etaZero = 0.99); // Test Beam 2000 & 2005
  
  // TestBeam 2000 Ar/CF4/Co2: 70/15/10
  //declareProperty("rho", m_rho = 1.47/mm);
  
  // Test Beam 2005 Ar/Co2 : 70/30
  //declareProperty("rho", m_rho = 3.333/Gaudi::Units::mm);

  //Effective ionization length from fit to eff. profile 
  declareProperty("rho", m_rho = 1.5/Gaudi::Units::mm);

  declareInterface<IOTEffCalculator>(this);
}

StatusCode OTEffCalculator::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize OTEffCalculator", sc );
  
  // get interface to generator
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = randSvc->generator(Rndm::Flat(0.0,1.0),m_genEff.pRef()); 
  if ( sc.isFailure() ) {
    return Error("Failed to generate random number distribution",sc);
  }
  sc = release(randSvc);
  if (sc.isFailure()) {
    return Error("Failed to release RndmGenSvc", sc);
  }

  return StatusCode::SUCCESS;  
}

OTEffCalculator::~OTEffCalculator()
{
  //destructor
}


//Function that computes the efficiency, using the 3D path through the cell, using the slope
double OTEffCalculator::effParamFunc(const double driftDistance, const double slope) const
{

  // efficiency function parameterization
  // calculate half the path Length through the cell

  double pathLength = std::sqrt((gsl_pow_2(m_cellRadius) - gsl_pow_2(driftDistance))*(gsl_pow_2(slope)+ 1));

  // exclude paths through the cell that are vertical
  if (pathLength>10){
    return m_etaZero;
  }
  

  // if pathLength2 > 0.0 return eff parm else 0 (driftdistance > cell radius)
  return (pathLength > 0.0?m_etaZero*(1.0-gsl_sf_exp(-2.0*m_rho*pathLength)):0.0);  

}

void OTEffCalculator::calculate(MCOTDeposit* aDeposit, const double slope, bool& accept) const
{
  // get a random number
  double testVal = m_genEff->shoot();
  // to accept or not to accept
  accept = (testVal < effParamFunc(aDeposit->driftDistance(), slope));
  
}


