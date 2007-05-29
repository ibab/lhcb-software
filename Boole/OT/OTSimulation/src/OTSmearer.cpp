// $Id: OTSmearer.cpp,v 1.13 2007-05-29 15:10:56 mneedham Exp $

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
 // get interface to generator
 IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
 sc = randSvc->generator(Rndm::Gauss(0.,1.0),m_genDist.pRef()); 
 if ( sc.isFailure() ) {
   return Error ("Failed to generate random number distribution",sc);
 }
 release(randSvc);

 // retrieve pointer to magnetic field service
 m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

 // Loading OT Geometry from XML
 m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
 
 return StatusCode::SUCCESS;  
}

void OTSmearer::smearDistance(MCOTDeposit* aDeposit) const
{
  // retrieve MC info
  const MCHit* aMCHit = aDeposit->mcHit();

  // average entrance and exit to get point in cell 
  Gaudi::XYZPoint aPoint = aMCHit->midPoint();

  // get sigma (error on drift distance) for this point 
  const double driftDistError = resolution(aPoint);

  // get a random number (from Gaussian)
  const double smearVal = m_genDist->shoot();
 
  // smear
  double driftDist = aDeposit->driftDistance();
  driftDist += smearVal*driftDistError;

  // update digitization 
  aDeposit->setDriftDistance(driftDist);
}


void OTSmearer::smearTime(MCOTDeposit* aDeposit) const
{
  // retrieve MC info
  const MCHit* aMCHit = aDeposit->mcHit();

  // average entrance and exit to get point in cell 
  const Gaudi::XYZPoint& aPoint = aMCHit->midPoint();

  // get sigma (error on drift distance) for this point 
  const double driftTimeError = timeResolution(aPoint);

  // get a random number (from Gaussian)
  double smearVal = m_genDist->shoot();

  // update digitization 
  aDeposit->addTime(driftTimeError*smearVal);
}

double OTSmearer::resolution() const
{
  // return sigma (without magnetic field correction)
  return m_tracker->resolution();
}

double OTSmearer::resolution(const Gaudi::XYZPoint& aPoint) const
{
  // return sigma (error on drift distance) for this point 
  static Gaudi::XYZVector bField;
  /// fieldVector always returns success. Save to ignore
  m_magFieldSvc->fieldVector( aPoint, bField ).ignore();
  /// return sigma with magnetic field correction
  return m_tracker->resolution(bField.y());
}

double OTSmearer::timeResolution(const Gaudi::XYZPoint& aPoint) const{

  // return sigma (error on drift distance) for this point 
  static Gaudi::XYZVector bField;
  /// fieldVector always returns success. Save to ignore
  m_magFieldSvc->fieldVector( aPoint, bField ).ignore();
  /// return sigma with magnetic field correction

  const double maxDriftTime = m_tracker->maxDriftTimeFunc(bField.y() );
  return maxDriftTime*m_tracker->resolution(bField.y())/m_tracker->driftDistance(maxDriftTime,bField.y());
}
