// $Id: XmlDummyDECnv.cpp,v 1.3 2007-01-23 07:54:54 cattanem Exp $
// Include files 

#include <iostream>

// local
#include "XmlDummyDECnv.h"

//-----------------------------------------------------------------------------
// Implementation file for class : XmlDummyDECnv
//
// 2005-04-11 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

DECLARE_CONVERTER_FACTORY( XmlDummyDECnv )

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
XmlDummyDECnv::~XmlDummyDECnv() {}

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



