// $Id: OTSmearer.cpp,v 1.2 2004-09-10 13:14:23 cattanem Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IDataProviderSvc.h"

//CLHEP
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

// OTEvent
#include "Event/MCHit.h"

//OTSimulation
#include "OTSmearer.h"

/** @file OTSmearer.cpp 
 *
 *  Implementation of OTSmearer tool.
 *  
 *  @author M. Needham
 *  @date   21/10/2000
 */

static ToolFactory<OTSmearer> s_factory;
const IToolFactory& OTSmearerFactory = s_factory;


OTSmearer::OTSmearer(const std::string& type, 
                     const std::string& name, 
                     const IInterface* parent) : 
  AlgTool( type, name, parent ),
  m_magFieldSvc(0)
{
  declareInterface<IOTSmearer>(this);
}

OTSmearer::~OTSmearer()
{
  //destructor
}

StatusCode OTSmearer::finalize()
{
  // release services and tools requested at initialization
  if( 0 != m_magFieldSvc ) {
    m_magFieldSvc->release();
    m_magFieldSvc = 0;
  }
  return StatusCode::SUCCESS;
}

StatusCode OTSmearer::initialize() 
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
  sc = randSvc->generator(Rndm::Gauss(0.,1.0),m_genDist.pRef()); 
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to generate random number distribution" 
        << endreq;
    return sc;
  }
  randSvc->release();

  // retrieve pointer to magnetic field service
  sc = serviceLocator()->service("MagneticFieldSvc", m_magFieldSvc, true);
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to retrieve magnetic field service" << endreq;
    return sc;
  }

  // Loading OT Geometry from XML
  IDataProviderSvc* detSvc; 
  sc = serviceLocator()->service( "DetectorDataSvc", detSvc, true );
  if( sc.isFailure() ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to retrieve magnetic field service" << endreq;
  }
  SmartDataPtr<DeOTDetector> tracker( detSvc, "/dd/Structure/LHCb/OT" );
  if ( !tracker ) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Unable to retrieve Tracker detector element"
        << " from xml." << endreq;
    return StatusCode::FAILURE;
  }
  m_tracker = tracker;
  detSvc->release();
  

  return StatusCode::SUCCESS;  
}


StatusCode OTSmearer::smear(MCOTDeposit* aDeposit)
{
  // retrieve MC info
  MCHit* aMCHit = aDeposit->mcHit();

  // average entrance and exit to get point in cell 
  const HepPoint3D& entrancePoint = aMCHit->entry();
  const HepPoint3D& exitPoint = aMCHit->exit();
  HepPoint3D aPoint = 0.5*(entrancePoint+exitPoint);

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


double OTSmearer::resolution(HepPoint3D& aPoint)
{
  // return sigma (error on drift distance) for this point 
  HepVector3D bField;
  m_magFieldSvc->fieldVector( aPoint, bField );
  return sigmaParamFunc(bField.y());
}
