//$Id: XmlTestAlgorithm.cpp,v 1.1 2001-12-17 20:13:25 andreav Exp $

#include "XmlTestAlgorithm.h"
#include "DetDesc/Condition.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IAlignment.h"
#include "DetDesc/ICalibration.h"
#include "DetDesc/IReadOut.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/IFastControl.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<XmlTestAlgorithm> Factory;
const IAlgFactory& XmlTestAlgorithmFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
XmlTestAlgorithm::XmlTestAlgorithm ( const std::string& name, 
				     ISvcLocator* pSvcLocator )
  : Algorithm( name, pSvcLocator )
{
}

//----------------------------------------------------------------------------

/// Initialize the algorithm. 
StatusCode XmlTestAlgorithm::initialize() {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Process one event.
StatusCode XmlTestAlgorithm::execute() {

  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO
      << "*************** execute(): process new event ***************"
      << endreq;

  // Retrieve the LHCb detector element
  log << MSG::INFO << "Retrieve the LHCb detector" << endreq;
  SmartDataPtr<DetectorElement> lhcb ( detSvc(), "/dd/Structure/LHCb" );
  if( 0 != lhcb ) {
    log << MSG::INFO << "Successfully retrieved the LHCb detector" << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve the LHCb detector" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the LHCb detector" << endreq;
  sc = detSvc()->updateObject( (DataObject*)lhcb );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }

  // Retrieve the Ecal detector element
  log << MSG::INFO << "Retrieve the Ecal detector" << endreq;
  SmartDataPtr<DetectorElement> ecal ( detSvc(), "/dd/Structure/LHCb/Ecal" );
  if( 0 != ecal ) {
    log << MSG::INFO << "Successfully retrieved the Ecal detector" << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve the Ecal detector" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Ecal detector" << endreq;
  sc = detSvc()->updateObject( (DataObject*)ecal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }

  // Retrieve alignment for the Ecal detector
  log << MSG::INFO << "Retrieve alignment for the Ecal detector" << endreq;
  Condition* alEcal = ecal->alignment()->condition();
  if( 0 != alEcal ) {
    log << MSG::INFO << "Before update, Ecal alignment data has " 
	<< alEcal->params().size() << " parameters and "
	<< alEcal->paramVectors().size() << " parameter vectors" << std::endl
	<< alEcal->printParams() << alEcal->printParamVectors() << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal alignment" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Condition" << endreq;
  sc = detSvc()->updateObject( (DataObject*)alEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }
  log << MSG::INFO << "After update, Ecal alignment data has " 
      << alEcal->params().size() << " parameters and "
      << alEcal->paramVectors().size() << " parameter vectors" << std::endl
      << alEcal->printParams() << alEcal->printParamVectors() << endreq;

  // Retrieve calibration for the Ecal detector
  log << MSG::INFO << "Retrieve calibration for the Ecal detector" << endreq;
  Condition* caEcal = ecal->calibration()->condition();
  if( 0 != caEcal ) {
    log << MSG::INFO << "Before update, Ecal calibration data has " 
	<< caEcal->params().size() << " parameters and "
	<< caEcal->paramVectors().size() << " parameter vectors" << std::endl
	<< caEcal->printParams() << caEcal->printParamVectors() << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal calibration" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Condition" << endreq;
  sc = detSvc()->updateObject( (DataObject*)caEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }
  log << MSG::INFO << "After update, Ecal calibration data has " 
      << caEcal->params().size() << " parameters and "
      << caEcal->paramVectors().size() << " parameter vectors" << std::endl
      << caEcal->printParams() << caEcal->printParamVectors() << endreq;

  // Retrieve readout for the Ecal detector
  log << MSG::INFO << "Retrieve readout for the Ecal detector" << endreq;
  Condition* roEcal = ecal->readOut()->condition();
  if( 0 != roEcal ) {
    log << MSG::INFO << "Before update, Ecal readout data has " 
	<< roEcal->params().size() << " parameters and "
	<< roEcal->paramVectors().size() << " parameter vectors" << std::endl
	<< roEcal->printParams() << roEcal->printParamVectors() << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal readout" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Condition" << endreq;
  sc = detSvc()->updateObject( (DataObject*)roEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }
  log << MSG::INFO << "After update, Ecal readout data has " 
      << roEcal->params().size() << " parameters and "
      << roEcal->paramVectors().size() << " parameter vectors" << std::endl
      << roEcal->printParams() << roEcal->printParamVectors() << endreq;

  // Retrieve slow control for the Ecal detector
  log << MSG::INFO << "Retrieve slow control for the Ecal detector" << endreq;
  Condition* scEcal = ecal->slowControl()->condition();
  if( 0 != scEcal ) {
    log << MSG::INFO << "Before update, Ecal slow control data has " 
	<< scEcal->params().size() << " parameters and "
	<< scEcal->paramVectors().size() << " parameter vectors" << std::endl
	<< scEcal->printParams() << scEcal->printParamVectors() << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal slow control" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Condition" << endreq;
  sc = detSvc()->updateObject( (DataObject*)scEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }
  log << MSG::INFO << "After update, Ecal slow control data has " 
      << scEcal->params().size() << " parameters and "
      << scEcal->paramVectors().size() << " parameter vectors" << std::endl
      << scEcal->printParams() << scEcal->printParamVectors() << endreq;
  
  // Retrieve fast control for the Ecal detector
  log << MSG::INFO << "Retrieve fast control for the Ecal detector" << endreq;
  Condition* fcEcal = ecal->fastControl()->condition();
  if( 0 != fcEcal ) {
    log << MSG::INFO << "Before update, Ecal fast control data has " 
	<< fcEcal->params().size() << " parameters and "
	<< fcEcal->paramVectors().size() << " parameter vectors" << std::endl
	<< fcEcal->printParams() << fcEcal->printParamVectors() << endreq;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal fast control" << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "Now update the Condition" << endreq;
  sc = detSvc()->updateObject( (DataObject*)fcEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endreq;
  }
  log << MSG::INFO << "After update, Ecal fast control data has " 
      << fcEcal->params().size() << " parameters and "
      << fcEcal->paramVectors().size() << " parameter vectors" << std::endl
      << fcEcal->printParams() << fcEcal->printParamVectors() << endreq;
  
  // Event processing completed
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

/// Finalize the algorithm. 
StatusCode XmlTestAlgorithm::finalize( ) {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

