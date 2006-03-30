// $Id: OTrtRelation.cpp,v 1.8 2006-03-30 21:50:19 janos Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// MathCore
#include "Kernel/Vector3DTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/SystemOfUnits.h"

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// OTSimulation
#include "OTrtRelation.h"

/** @file OTrtRelation.cpp 
 *
 *  Implementation of OTrtRelation tool.
 *  
 *  @author M. Needham
 *  @date   21/10/2000
 */

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( OTrtRelation );

OTrtRelation::OTrtRelation(const std::string& type, 
                           const std::string& name, 
                           const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_magFieldSvc(0)
{
  declareInterface<IOTrtRelation>(this); 
}

StatusCode OTrtRelation::initialize() 
{

  StatusCode sc = GaudiTool::initialize();
  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  
  // Loading OT Geometry from XML
  IDataProviderSvc* detSvc; 
  sc = serviceLocator()->service( "DetectorDataSvc", detSvc, true );
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve DetectorDataSvc",sc);
  }

  m_tracker = getDet<DeOTDetector>(  DeOTDetectorLocation::Default );
  detSvc->release();

  return StatusCode::SUCCESS;
}
			     
OTrtRelation::~OTrtRelation()
{
  //destructor
}

StatusCode OTrtRelation::convertRtoT(MCOTDeposit* aDeposit)
{
  // r-t relation
  // retrieve MC info
  const MCHit* aMCHit = aDeposit->mcHit();
  const double driftDist = aDeposit->driftDistance();
  
  // average entrance and exit to get point in cell 
  const Gaudi::XYZPoint& entryPoint = aMCHit->entry();
  const Gaudi::XYZPoint& exitPoint = aMCHit->exit();
  Gaudi::XYZPoint aPoint = entryPoint + 0.5 * (exitPoint - entryPoint);

  // add drift time
  double time = driftTime(driftDist, aPoint) ;

  // add propagation delay
  time += m_tracker->propagationTime(aDeposit->channel(),aPoint.x(),aPoint.y());

  // store the time in deposit
  aDeposit->addTime(time);

  return StatusCode::SUCCESS;
}

double OTrtRelation::driftTime(const double driftDist, const Gaudi::XYZPoint& aPoint)
{
  // r-t relation with correction for the magnetic field

  // get magnetic field
  Gaudi::XYZVector bField;
  m_magFieldSvc->fieldVector( aPoint, bField );

  return m_tracker->driftTime(driftDist, bField.y());  
}

double OTrtRelation::driftDistance( const double driftTime, 
                                    const Gaudi::XYZPoint& aPoint )
{
  // inverse r-t relation with correction for the magnetic field
  Gaudi::XYZVector bField;
  m_magFieldSvc->fieldVector( aPoint, bField );
  return m_tracker->driftDistance(driftTime, bField.y());
}
