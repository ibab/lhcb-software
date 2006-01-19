// $Id: XmlDummyDECnv.cpp,v 1.2 2006-01-19 18:32:11 marcocle Exp $
// Include files 

#include "GaudiKernel/CnvFactory.h"

#include <iostream>

// local
#include "XmlDummyDECnv.h"

//-----------------------------------------------------------------------------
// Implementation file for class : XmlDummyDECnv
//
// 2005-04-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlDummyDECnv> dummyde_factory;
const ICnvFactory& XmlDummyDECnvFactory = dummyde_factory;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
XmlDummyDECnv::XmlDummyDECnv(ISvcLocator* svc):
    XmlUserDetElemCnv<DummyDetectorElement>(svc) {

  //std::cout << "XmlDummyDECnv: Hello world!" << std::endl;
}

//=============================================================================
// Destructor
//=============================================================================
XmlDummyDECnv::~XmlDummyDECnv() {}; 

//=============================================================================
// Specific Fill
//=============================================================================
StatusCode XmlDummyDECnv::i_fillSpecificObj(xercesc::DOMElement* childElement,
                                            DummyDetectorElement* dataObj,
                                            IOpaqueAddress* address){
  // Nothing special, just fill a generic DE
  return XmlUserDetElemCnv<DummyDetectorElement>::i_fillSpecificObj(childElement, dynamic_cast<DetectorElement*>(dataObj),
                                                                    address);
}
//=============================================================================



