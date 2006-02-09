//$Id: XmlTestAlgorithm.cpp,v 1.8 2006-02-09 08:29:12 marcocle Exp $

#include "XmlTestAlgorithm.h"
#include "DetDesc/Condition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/// Instantiation of a static factory to create instances of this algorithm
DECLARE_ALGORITHM_FACTORY( XmlTestAlgorithm );

//----------------------------------------------------------------------------

/// Constructor
XmlTestAlgorithm::XmlTestAlgorithm ( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiAlgorithm( name, pSvcLocator )
{}

//----------------------------------------------------------------------------

/// Process one event.
StatusCode XmlTestAlgorithm::execute() {

  //StatusCode sc;
  info() << "*************** execute(): process new event ***************" << endmsg;

  // Retrieve the LHCb detector element
  info() << "Retrieve the LHCb detector" << endmsg;
  DetectorElement *lhcb = getDet<DetectorElement>( "/dd/Structure/LHCb" );
  if( 0 != lhcb ) {
    info() << "Successfully retrieved the LHCb detector" << endmsg;
  } else {
    error() << "Could not retrieve the LHCb detector" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve the Ecal detector element
  info() << "Retrieve the Ecal detector" << endmsg;
  DetectorElement *ecal = getDet<DetectorElement>( "/dd/Structure/LHCb/Ecal" );
  if( 0 != ecal ) {
    info() << "Successfully retrieved the Ecal detector" << endmsg;
  } else {
    error() << "Could not retrieve the Ecal detector" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve alignment for the Ecal detector
  info() << "Retrieve alignment for the Ecal detector" << endmsg;
  Condition* alEcal = ecal->condition("Alignment");
  if( 0 != alEcal ) {
    info() << "Ecal alignment data has "
           << alEcal->paramNames().size() << " parameters" << std::endl
           << alEcal->printParams() << endmsg;
  } else {
    error() << "Could not retrieve Ecal alignment" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve calibration for the Ecal detector
  info() << "Retrieve calibration for the Ecal detector" << endmsg;
  Condition* caEcal = ecal->condition("Calibration");
  if( 0 != caEcal ) {
    info() << "Ecal calibration data has "
           << caEcal->paramNames().size() << " parameters" << std::endl
           << caEcal->printParams() << endmsg;
  } else {
    error() << "Could not retrieve Ecal calibration" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve readout for the Ecal detector
  info() << "Retrieve readout for the Ecal detector" << endmsg;
  Condition* roEcal = ecal->condition("Readout");
  if( 0 != roEcal ) {
    info() << "Ecal readout data has "
           << roEcal->paramNames().size() << " parameters" << std::endl
           << roEcal->printParams() << endmsg;
  } else {
    error() << "Could not retrieve Ecal readout" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve slow control for the Ecal detector
  info() << "Retrieve slow control for the Ecal detector" << endmsg;
  Condition* scEcal = ecal->condition("SlowControl");
  if( 0 != scEcal ) {
    info() << "Ecal slow control data has "
           << scEcal->paramNames().size() << " parameters" << std::endl
           << scEcal->printParams() << endmsg;
  } else {
    error() << "Could not retrieve Ecal slow control" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve fast control for the Ecal detector
  info() << "Retrieve fast control for the Ecal detector" << endmsg;
  Condition* fcEcal = ecal->condition("FastControl");
  if( 0 != fcEcal ) {
    info() << "Ecal fast control data has "
           << fcEcal->paramNames().size() << " parameters" << std::endl
           << fcEcal->printParams() << endmsg;
  } else {
    error() << "Could not retrieve Ecal fast control" << endmsg;
    return StatusCode::FAILURE;
  }

  Condition* testCond = getDet<Condition>("/dd/Conditions/TestCondition");
  if( 0 != testCond ) {
    info() << "TestCondition has "
           << testCond->paramNames().size() << " parameters" << std::endl
           << testCond->printParams() << endmsg;
  } else {
    error() << "Could not retrieve TestCondition" << endmsg;
    return StatusCode::FAILURE;
  }
  

  info() << " ===== test XML string generation =====" << endmsg;
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

