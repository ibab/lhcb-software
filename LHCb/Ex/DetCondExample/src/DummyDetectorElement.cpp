// $Id: DummyDetectorElement.cpp,v 1.2 2005-05-03 12:46:19 marcocle Exp $
// Include files 

#include <string>
#include "DetDesc/IReadOut.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/Condition.h"

#include "DetDesc/IUpdateManagerSvc.h"

// local
#include "DummyDetectorElement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DummyDetectorElement
//
// 2005-04-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyDetectorElement::DummyDetectorElement(const std::string& name):
        DetectorElement(name)
{
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyDetectorElement::DummyDetectorElement(const std::string& name,
                                           const ITime& iValidSince,
                                           const ITime& iValidTill):
        DetectorElement(name,iValidSince,iValidTill)
{
}
//=============================================================================
// Destructor
//=============================================================================
DummyDetectorElement::~DummyDetectorElement() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DummyDetectorElement::initialize(){
  MsgStream log(msgSvc(),name());
  
  log << MSG::DEBUG << " --- initialize DummyDetectorElement --- " << endmsg;
  StatusCode sc = DetectorElement::initialize();
  if ( !sc.isSuccess() ) return sc;
  
  try {
    log << MSG::DEBUG << "Registering conditions" << endmsg;
    updMgrSvc()->registerCondition(this,slowControl()->conditionName(),&DummyDetectorElement::i_updateTemperatures);
    updMgrSvc()->registerCondition(this,readOut()->conditionName(),&DummyDetectorElement::i_updateTemperatures);
    updMgrSvc()->registerCondition(this,readOut()->conditionName(),&DummyDetectorElement::i_updateChannels);
    log << MSG::DEBUG << "Start first update" << endmsg;
    sc = updMgrSvc()->update(this);
    if ( !sc.isSuccess() ) {
  	  return sc;
    }
  } catch (DetectorElementException &e) {
    log << MSG::ERROR << e << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Update methods
//=============================================================================
StatusCode DummyDetectorElement::i_updateTemperatures(){
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering i_updateTemperatures()" << endmsg;
  try {
    m_slowTemp   = slowControl()->condition()->paramAsDouble("Temperature");
    m_cratesTemp = readOut()->condition()->paramVectorAsDouble("CrateTemps");
  } catch (...) {
    log << MSG::ERROR << "i_updateTemperatures: couldn't access condition" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Update methods
//=============================================================================
StatusCode DummyDetectorElement::i_updateChannels(){
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "Entering i_updateChannels()" << endmsg;
  try {
    m_maxChannels = readOut()->condition()->paramAsInt("NChannels");
    m_activeChannels = readOut()->condition()->paramVectorAsInt("Channels");
  } catch (...) {
    log << MSG::ERROR << "i_updateChannels: couldn't access condition" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
