// $Id: $

// FTDet
#include "FTDet/DeFTFibreModule.h"

/** @file DeFTFibreModule.cpp
 *
 *  Implementation of class : DeFTFibreModule
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTFibreModule::DeFTFibreModule( const std::string& name ) :
  DetectorElement ( name )
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTFibreModule::~DeFTFibreModule(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTFibreModule::clID () const {
  return DeFTFibreModule::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTFibreModule::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Initialize DeFTFibreModule" << endmsg;

  return StatusCode::SUCCESS;
}

