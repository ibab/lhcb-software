// $Id: OTrtRelation.cpp,v 1.14 2007-04-08 16:54:51 janos Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// OTDet
#include "OTDet/DeOTDetector.h"

// MathCore
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

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
  if ( sc.isFailure() ) return Error( "Failed to initialize OTrtRelation", sc );
  
  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  
  m_tracker = getDet<DeOTDetector>(  DeOTDetectorLocation::Default );

  return StatusCode::SUCCESS;
}
			     
OTrtRelation::~OTrtRelation()
{
  //destructor
}

void OTrtRelation::convertRtoT(MCOTDeposit* aDeposit)
{
  // r-t relation
  // retrieve MC info
  const MCHit* aMCHit = aDeposit->mcHit();
  const double driftDist = aDeposit->driftDistance();
  
  // average entrance and exit to get point in cell 
  Gaudi::XYZPoint aPoint = aMCHit->midPoint();

  // add drift time and propagation delay
  double time = driftTime(driftDist, aPoint) + 
    (m_tracker->propagationTime(aDeposit->channel(), aPoint.x(), aPoint.y())) ;
  
  // store the time in deposit
  aDeposit->addTime(time);
}

double OTrtRelation::driftTime(const double driftDist, const Gaudi::XYZPoint& aPoint)
{
  // r-t relation with correction for the magnetic field

  // get magnetic field
  Gaudi::XYZVector bField;
  /// fieldVector always returns success. Save to ignore
  m_magFieldSvc->fieldVector( aPoint, bField ).ignore();

  return m_tracker->driftTime(driftDist, bField.y());  
}

double OTrtRelation::driftDistance( const double driftTime, 
                                    const Gaudi::XYZPoint& aPoint )
{
  // inverse r-t relation with correction for the magnetic field
  Gaudi::XYZVector bField;
  /// fieldVector always returns success. Save to ignore
  m_magFieldSvc->fieldVector( aPoint, bField ).ignore();
  return m_tracker->driftDistance(driftTime, bField.y());
}
