
#include "STDet/DeSTBaseElement.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "DetDesc/IGeometryInfo.h"


/** @file DeSTBaseElement.cpp
*
*  Implementation of class :  DeSTBaseElement
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeSTBaseElement::DeSTBaseElement( const std::string& name ) :
  DetectorElement( name )
{ 
  // constructer
}


DeSTBaseElement::~DeSTBaseElement() {
  // destructer
}

StatusCode DeSTBaseElement::initialize() {

  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
 
  // cache trajectories
  try {
    msg << MSG::DEBUG << "Registering conditions" << endmsg;
    updMgrSvc()->registerCondition(this,this->geometry(),&DeSTBaseElement::cachePoint);
    msg << MSG::DEBUG << "Start first update" << endmsg;
    sc = updMgrSvc()->update(this);
  } 
  catch (DetectorElementException &e) {
   msg << MSG::ERROR << e << endmsg;
   return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeSTBaseElement::cachePoint(){
 
  m_globalCentre = toGlobal(Gaudi::XYZPoint(0,0,0));
  return StatusCode::SUCCESS;
}


Gaudi::XYZPoint DeSTBaseElement::globalPoint( const double x, const double y, 
                                         const double z) const
{
  Gaudi::XYZPoint lPoint(x,y,z);
  Gaudi::XYZPoint gPoint = toGlobal(lPoint);
  return gPoint;
}

