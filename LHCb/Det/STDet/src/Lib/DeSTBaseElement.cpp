
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

  StatusCode sc = DetectorElement::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
    return StatusCode::FAILURE; 
  }
 
  // cache trajectories
  sc = registerCondition(this,this->geometry(),&DeSTBaseElement::cachePoint,true);
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to register conditions" << endmsg;
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
  const Gaudi::XYZPoint lPoint(x,y,z);
  Gaudi::XYZPoint gPoint = toGlobal(lPoint);
  return gPoint;
}



