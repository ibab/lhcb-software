
#include "STDet/DeSTBaseElement.h"

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

  return StatusCode::SUCCESS;
}


