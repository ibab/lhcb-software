// $Id: OTEffCalculator.cpp,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $

// Gaudi files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// OTSimulation
#include "OTSimulation/OTEffCalculator.h"

/** @file OTEffCalculator.cpp 
 *
 *  Implementation of OTEffCalculator tool
 *  
 *  @author M. Needham
 *  @date   19-09-2000
 */

static ToolFactory<OTEffCalculator> s_factory;
const IToolFactory& OTEffCalculatorFactory = s_factory;


OTEffCalculator::OTEffCalculator(const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent) : 
  AlgTool( type, name, parent ),
  m_genEff(0),
  m_cellRadius(2.5*mm) 
{ 
  // constructor
  // jobOptions
  declareProperty("etaZero", m_etaZero = 0.99); 
  declareProperty("rho", m_rho = 1.47/mm);

  declareInterface<IOTEffCalculator>(this);
}

StatusCode OTEffCalculator::initialize() 
{
  // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  StatusCode sc = serviceLocator()->service( "RndmGenSvc", randSvc, true );
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to retrieve random number service" << endreq;
    return sc;
  }

  // get interface to generator
  sc = randSvc->generator(Rndm::Flat(0.,1.),m_genEff.pRef()); 
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to generate random number distribution" 
        << endreq;
    return sc;
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
  }
  else{
    iAccept = false;
  } //  test

  return StatusCode::SUCCESS;
}

double OTEffCalculator::effParamFunc(const double driftDistance)
{
  // efficiency function parameterization
  
  // calculate the track path Length in the cell
  double pathLength2 = pow(m_cellRadius,2.) - pow(driftDistance,2.);

  // eff - return val
  double eff = 0.;

  if (pathLength2 > 0.0){
    eff = m_etaZero*(1.0-exp(-2.0*m_rho*sqrt(pathLength2)));
  }
  else{
    // efficiency 0 outside cell
  }

  return eff;
}
