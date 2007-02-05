// $Id: OTSmearer.cpp,v 1.11 2007-02-05 09:46:40 cattanem Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// MathCore
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

//OTSimulation
#include "OTSmearer.h"

/** @file OTSmearer.cpp 
 *
 *  Implementation of OTSmearer tool.
 *  
 *  @author M. Needham
 *  @date   21/10/2000
 */

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( OTSmearer );

OTSmearer::OTSmearer(const std::string& type, 
                     const std::string& name, 
                     const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_magFieldSvc(0)
{
  declareInterface<IOTSmearer>(this);
}

OTSmearer::~OTSmearer()
{
  //destructor
}

StatusCode OTSmearer::initialize() 
{
 StatusCode sc = GaudiTool::initialize();
 if ( sc.isFailure() ) return Error( "Failed to initialize OTSmearer", sc );

 // retrieve pointer to random number service
  IRndmGenSvc* randSvc = 0;
  sc = serviceLocator()->service( "RndmGenSvc", randSvc, true ); 
  if ( sc.isFailure() ) {
    return Error ("Failed to retrieve random number service",sc);
  }  

  // get interface to generator
  sc = randSvc->generator(Rndm::Gauss(0.,1.0),m_genDist.pRef()); 
  if ( sc.isFailure() ) {
    return Error ("Failed to generate random number distribution",sc);
  }
  randSvc->release();

  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

  // Loading OT Geometry from XML
  IDataProviderSvc* detSvc; 
  sc = serviceLocator()->service( "DetectorDataSvc", detSvc, true );
  if ( sc.isFailure() ) {
    return Error ("Failed to retrieve Detector data svc",sc);
  }
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  detSvc->release();
 
  return StatusCode::SUCCESS;  
}


StatusCode OTSmearer::smear(MCOTDeposit* aDeposit)
{
  // retrieve MC info
  const MCHit* aMCHit = aDeposit->mcHit();

  // average entrance and exit to get point in cell 
  Gaudi::XYZPoint aPoint = aMCHit->midPoint();

  // get sigma (error on drift distance) for this point 
  double driftDistError = resolution(aPoint);

  // set error on drift distance
  //aDigit->setDriftDistanceErr(driftDistError);

  // get a random number (from Gaussian)
  double smearVal = m_genDist->shoot();
 
  // smear
  double driftDist = aDeposit->driftDistance();
  driftDist += smearVal*driftDistError;

  // update digitization 
  aDeposit->setDriftDistance(driftDist);

  return StatusCode::SUCCESS;
}


double OTSmearer::sigmaParamFunc(const double By)
{
  // Calculate sigma of smear 
  return m_tracker->resolution(By);
}


double OTSmearer::resolution()
{
  // return sigma (without magnetic field correction)
  return m_tracker->resolution();
}


double OTSmearer::resolution(Gaudi::XYZPoint& aPoint)
{
  // return sigma (error on drift distance) for this point 
  Gaudi::XYZVector bField;
  m_magFieldSvc->fieldVector( aPoint, bField );
  return sigmaParamFunc(bField.y());
}
