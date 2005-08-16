//$Id: XmlTestAlgorithm.cpp,v 1.5 2005-08-16 09:28:52 marcocle Exp $

#include "XmlTestAlgorithm.h"
#include "DetDesc/Condition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

/// Instantiation of a static factory to create instances of this algorithm
static const AlgFactory<XmlTestAlgorithm> Factory;
const IAlgFactory& XmlTestAlgorithmFactory = Factory;

//----------------------------------------------------------------------------

/// Constructor
XmlTestAlgorithm::XmlTestAlgorithm ( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name, pSvcLocator )
{}

//----------------------------------------------------------------------------

/// Process one event.
StatusCode XmlTestAlgorithm::execute() {

  //StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "*************** execute(): process new event ***************" << endmsg;

  // Retrieve the LHCb detector element
  log << MSG::INFO << "Retrieve the LHCb detector" << endmsg;
  SmartDataPtr<DetectorElement> lhcb ( detSvc(), "/dd/Structure/LHCb" );
  if( 0 != lhcb ) {
    log << MSG::INFO << "Successfully retrieved the LHCb detector" << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve the LHCb detector" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the LHCb detector" << endmsg;
  sc = detSvc()->updateObject( lhcb );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  */

  // Retrieve the Ecal detector element
  log << MSG::INFO << "Retrieve the Ecal detector" << endmsg;
  SmartDataPtr<DetectorElement> ecal ( detSvc(), "/dd/Structure/LHCb/Ecal" );
  if( 0 != ecal ) {
    log << MSG::INFO << "Successfully retrieved the Ecal detector" << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve the Ecal detector" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Ecal detector" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)ecal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  */

  // Retrieve alignment for the Ecal detector
  log << MSG::INFO << "Retrieve alignment for the Ecal detector" << endmsg;
  // Condition* alEcal = ecal->alignment()->condition();
  Condition* alEcal = ecal->condition("Alignment");
  if( 0 != alEcal ) {
    log << MSG::INFO << /* "Before update, " */ "Ecal alignment data has " 
        << alEcal->paramNames().size() << " parameters" << std::endl
        << alEcal->printParams() << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal alignment" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Condition" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)alEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  log << MSG::INFO << "After update, Ecal alignment data has " 
      << alEcal->paramNames().size() << " parameters" << std::endl
      << alEcal->printParams() << endmsg;
  */

  // Retrieve calibration for the Ecal detector
  log << MSG::INFO << "Retrieve calibration for the Ecal detector" << endmsg;
  Condition* caEcal = ecal->condition("Calibration");
  if( 0 != caEcal ) {
    log << MSG::INFO << /* "Before update, " */ "Ecal calibration data has " 
        << caEcal->paramNames().size() << " parameters" << std::endl
        << caEcal->printParams() << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal calibration" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Condition" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)caEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  log << MSG::INFO << "After update, Ecal calibration data has " 
      << caEcal->paramNames().size() << " parameters" << std::endl
      << caEcal->printParams() << endmsg;
  */

  // Retrieve readout for the Ecal detector
  log << MSG::INFO << "Retrieve readout for the Ecal detector" << endmsg;
  Condition* roEcal = ecal->condition("Readout");
  if( 0 != roEcal ) {
    log << MSG::INFO << /* "Before update, " */ "Ecal readout data has " 
        << roEcal->paramNames().size() << " parameters" << std::endl
        << roEcal->printParams() << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal readout" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Condition" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)roEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  log << MSG::INFO << "After update, Ecal readout data has " 
      << roEcal->paramNames().size() << " parameters" << std::endl
      << roEcal->printParams() << endmsg;
  */

  // Retrieve slow control for the Ecal detector
  log << MSG::INFO << "Retrieve slow control for the Ecal detector" << endmsg;
  Condition* scEcal = ecal->condition("SlowControl");
  if( 0 != scEcal ) {
    log << MSG::INFO << /* "Before update, " */ "Ecal slow control data has " 
        << scEcal->paramNames().size() << " parameters" << std::endl
        << scEcal->printParams() << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal slow control" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Condition" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)scEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  log << MSG::INFO << "After update, Ecal slow control data has " 
      << scEcal->paramNames().size() << " parameters" << std::endl
      << scEcal->printParams() << endmsg;
  */

  // Retrieve fast control for the Ecal detector
  log << MSG::INFO << "Retrieve fast control for the Ecal detector" << endmsg;
  Condition* fcEcal = ecal->condition("FastControl");
  if( 0 != fcEcal ) {
    log << MSG::INFO << /* "Before update, " */ "Ecal fast control data has " 
        << fcEcal->paramNames().size() << " parameters" << std::endl
        << fcEcal->printParams() << endmsg;
  } else {
    log << MSG::ERROR << "Could not retrieve Ecal fast control" << endmsg;
    return StatusCode::FAILURE;
  }
  /*
  log << MSG::INFO << "Now update the Condition" << endmsg;
  sc = detSvc()->updateObject( (DataObject*)fcEcal );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Can't update DataObject" << endmsg;
    return StatusCode::FAILURE;
  } else {
    log << MSG::INFO << "DataObject successfully updated" << endmsg;
  }
  log << MSG::INFO << "After update, Ecal fast control data has " 
      << fcEcal->paramNames().size() << " parameters" << std::endl
      << fcEcal->printParams() << endmsg;
  */

  log << MSG::INFO << " ===== test XML string generation =====" << endmsg;
  // Prepare a nice condition
  Condition myCond;
  myCond.addParam<int>("i",12345,"an int");
  myCond.addParam<double>("d",1.2345678901234567890,"a double");
  myCond.addParam<std::string>("s","test1","a string");
  std::vector<int> vi;
  vi.push_back(1);
  vi.push_back(2);
  vi.push_back(3);
  std::vector<double> vd;
  vd.push_back(1.1);
  vd.push_back(2.2);
  vd.push_back(3.3);
  myCond.addParam<std::vector<int> >("vi",vi,"a vector of ints");
  myCond.addParam<std::vector<double> >("vd",vd,"a vector of doubles");
  
  info() << "myCond has " << myCond.paramNames().size() << " parameters\n"
         << myCond.printParams() << endmsg;
  info() << "XML is:" << endmsg;
  info() << myCond.toXml() << endmsg;

  TabulatedProperty tabProp;
  tabProp.setType("testing");
  tabProp.setXAxis("x");
  tabProp.setYAxis("y");

  tabProp.table().push_back(TabulatedProperty::Entry(1,1));
  tabProp.table().push_back(TabulatedProperty::Entry(1.2,2.1));
  tabProp.table().push_back(TabulatedProperty::Entry(1.3,3.2));

  info() << tabProp << endmsg;

  info() << "XML is:" << endmsg;
  info() << tabProp.toXml() << endmsg;


  // Event processing completed
  return StatusCode::SUCCESS;

}

//----------------------------------------------------------------------------

