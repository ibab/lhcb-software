// $Id: OTrtRelation.cpp,v 1.4 2004-12-10 08:09:13 jnardull Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IService.h"

// CLHEP
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// OTSimulation
#include "OTrtRelation.h"

// Event
#include "Event/MCHit.h"

/** @file OTrtRelation.cpp 
 *
 *  Implementation of OTrtRelation tool.
 *  
 *  @author M. Needham
 *  @date   21/10/2000
 */

static ToolFactory<OTrtRelation> s_factory;
const IToolFactory& OTrtRelationFactory = s_factory;

OTrtRelation::OTrtRelation(const std::string& type, 
                           const std::string& name, 
                           const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_magFieldSvc(0)
{
  declareInterface<IOTrtRelation>(this); 
}

StatusCode OTrtRelation::finalize()
{
  // Release all services and tools accessed at initialization
  if( 0 != m_magFieldSvc ) {
    m_magFieldSvc->release();
    m_magFieldSvc = 0;
  }
  return GaudiTool::finalize(); 
}

StatusCode OTrtRelation::initialize() 
{

  StatusCode sc = GaudiTool::initialize();
  // retrieve pointer to magnetic field service
  sc = serviceLocator()->service( "MagneticFieldSvc", 
                                             m_magFieldSvc, true ); 
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve magnetic field service",sc);
  }

  // Loading OT Geometry from XML
  IDataProviderSvc* detSvc; 
  sc = serviceLocator()->service( "DetectorDataSvc", detSvc, true );
  if( sc.isFailure() ) {
    return Error ("Failed to retrieve DetectorDataSvc",sc);
  }

  DeOTDetector* tracker = getDet<DeOTDetector>(  DeOTDetectorLocation::Default );
  m_tracker = tracker;
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
  MCHit* aMCHit = aDeposit->mcHit();
  const double driftDist = aDeposit->driftDistance();
  
  // average entrance and exit to get point in cell 
  const HepPoint3D& entrancePoint = aMCHit->entry();
  const HepPoint3D& exitPoint = aMCHit->exit();
  HepPoint3D aPoint = 0.5 * (entrancePoint + exitPoint);

  // add drift time
  double time = driftTime(driftDist, aPoint) ;

  // add propagation delay
  time += m_tracker->propagationTime(aDeposit->channel(),aPoint.x(),aPoint.y());

  // store the time in deposit
  aDeposit->addTime(time);

  return StatusCode::SUCCESS;
}

double OTrtRelation::driftTime(const double driftDist, const HepPoint3D& aPoint)
{
  // r-t relation with correction for the magnetic field

  // get magnetic field
  HepVector3D bField;
  m_magFieldSvc->fieldVector( aPoint, bField );

  return m_tracker->driftTime(driftDist, bField.y());  
}

double OTrtRelation::driftDistance( const double driftTime, 
                                    const HepPoint3D& aPoint )
{
  // inverse r-t relation with correction for the magnetic field
  HepVector3D bField;
  m_magFieldSvc->fieldVector( aPoint, bField );
  return m_tracker->driftDistance(driftTime, bField.y());
}
