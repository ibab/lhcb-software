// $Id: $

// FTDet
#include "FTDet/DeFTStation.h"

/** @file DeFTStation.cpp
 *
 *  Implementation of class : DeFTStation
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTStation::DeFTStation( std::string name ) :
  DetectorElement ( std::move(name) )
{

}

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTStation::clID () const {
  return DeFTStation::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTStation::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  if(msg.level() <= MSG::DEBUG) msg << MSG::DEBUG << "==> Initialize DeFTStation" << endmsg;

  return StatusCode::SUCCESS;
}

