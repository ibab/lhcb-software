// $Id: $

// FTDet
#include "FTDet/DeFTFibreMat.h"

/** @file DeFTFibreMat.cpp
 *
 *  Implementation of class : DeFTFibreMat
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTFibreMat::DeFTFibreMat( const std::string& name ) :
  DetectorElement ( name )
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTFibreMat::~DeFTFibreMat(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTFibreMat::clID () const {
  return DeFTFibreMat::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTFibreMat::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Initialize DeFTFibreMat" << endmsg;

  return StatusCode::SUCCESS;
}

