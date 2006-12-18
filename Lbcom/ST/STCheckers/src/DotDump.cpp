//
// This File contains the definition of the DotDump -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

#include <iostream>

// Gaudi
#include "GaudiKernel/AlgFactory.h"


#include "DotDump.h"

// xml geometry
#include "DetDesc/DetectorElement.h"

DECLARE_ALGORITHM_FACTORY( DotDump );

//--------------------------------------------------------------------
//
//  DotDump : Check digitization procedure for the outer tracker
//
//--------------------------------------------------------------------

DotDump::DotDump(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructer
  declareProperty( "path", m_path ="/dd/Structure/LHCb/TT" );
}

DotDump::~DotDump(){
  // destructer
}

StatusCode DotDump::initialize(){

  // initialize  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  } 

  DetectorElement* topElement = getDet<DetectorElement>(m_path);
  addToDot("digraph "+topElement->name()+" {");
  addToDot(topElement->name());
  children(m_path, topElement);  
  addToDot("}");

  for (std::vector<std::string>::iterator iString = m_dStrings.begin(); iString != m_dStrings.end(); ++iString){

    std::cout << *iString << std::endl;

  }

  return StatusCode::SUCCESS;

}

void DotDump::children(std::string path, DetectorElement* parent){

  IDetectorElement::IDEContainer::const_iterator iChild;
  for (iChild = parent->childBegin(); parent->childEnd() != iChild; ++iChild) {
    std::string newPath = path + "/" + (*iChild)->name();
    std::string rel = path + " -> " + (*iChild)->name();
    addToDot(rel);
    DetectorElement* tElem = dynamic_cast<DetectorElement*>(*iChild);
    children(newPath, tElem);
  }
}

StatusCode DotDump::execute(){

  // execute once per event
  return StatusCode::SUCCESS;
}

void DotDump::addToDot(const std::string aString){
  m_dStrings.push_back(aString);
}


