// $Id: XmlTabulatedPropertyCnv.cpp,v 1.7 2009-04-17 12:25:18 cattanem Exp $

// Include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/TabulatedProperty.h"
#include "XmlTools/IXmlSvc.h"
#include "DetDescCnv/XmlCnvException.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "XmlTabulatedPropertyCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlTabulatedPropertyCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlTabulatedPropertyCnv::XmlTabulatedPropertyCnv (ISvcLocator* svcs) :
  XmlGenericCnv (svcs, classID()),
  m_xunit (1),
  m_yunit (1){
  nameString = xercesc::XMLString::transcode("name");
  typeString = xercesc::XMLString::transcode("type");
  xaxisString = xercesc::XMLString::transcode("xaxis");
  yaxisString = xercesc::XMLString::transcode("yaxis");
  xunitString = xercesc::XMLString::transcode("xunit");
  yunitString = xercesc::XMLString::transcode("yunit");
  entryString = xercesc::XMLString::transcode("entry");
  xString = xercesc::XMLString::transcode("x");
  yString = xercesc::XMLString::transcode("y");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlTabulatedPropertyCnv::~XmlTabulatedPropertyCnv () {
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&typeString);
  xercesc::XMLString::release((XMLCh**)&xaxisString);
  xercesc::XMLString::release((XMLCh**)&yaxisString);
  xercesc::XMLString::release((XMLCh**)&xunitString);
  xercesc::XMLString::release((XMLCh**)&yunitString);
  xercesc::XMLString::release((XMLCh**)&entryString);
  xercesc::XMLString::release((XMLCh**)&xString);
  xercesc::XMLString::release((XMLCh**)&yString);
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlTabulatedPropertyCnv::i_createObj (xercesc::DOMElement* element,
                                                 DataObject*& refpObject) {
  m_xunit   = 1    ;
  m_yunit   = 1    ;

  // creates an object for the node found
  std::string elementName = dom2Std (element->getAttribute(nameString));
  TabulatedProperty* dataObj = new TabulatedProperty (elementName);
  refpObject = dataObj;

  /// get all attributes
  dataObj->table().clear()   ;
  const std::string Type = dom2Std (element->getAttribute (typeString));
  dataObj->setType(Type) ;
  const std::string xAxis= dom2Std (element->getAttribute (xaxisString));
  dataObj->setXAxis(xAxis) ;
  const std::string yAxis= dom2Std (element->getAttribute (yaxisString));
  dataObj->setYAxis(yAxis) ;
  std::string xUnitAttribute = dom2Std (element->getAttribute (xunitString));
  if (xUnitAttribute.empty()) {
    m_xunit = 1.0;
  } else {
    m_xunit = xmlSvc()->eval(xUnitAttribute, false);
  }
  std::string yUnitAttribute = dom2Std (element->getAttribute (yunitString));
  if (yUnitAttribute.empty()) {
    m_yunit = 1.0;
  } else {
    m_yunit = xmlSvc()->eval(yUnitAttribute, false);
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode
XmlTabulatedPropertyCnv::i_fillObj (xercesc::DOMElement* childElement,
                                    DataObject* refpObject,
                                    IOpaqueAddress* /*address*/) {
  MsgStream log(msgSvc(), "TabulatedPropertyCnv" );

  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(entryString, tagName)) {
    std::string xAttribute = dom2Std (childElement->getAttribute (xString));
    if (xAttribute.empty()) {
      throw XmlCnvException
        ("XmlTabulatedPropertyCnv: 'x' attribute for tag='entry'"
         " could not be omitted! ");
    }
    const double vx = xmlSvc()->eval(xAttribute ,false) * m_xunit;
    std::string yAttribute = dom2Std (childElement->getAttribute (yString));
    if (yAttribute.empty()) {
      throw XmlCnvException
        ("XmlTabulatedPropertyCnv: 'y' attribute for tag='entry'"
         " could not be omitted! ");
    }
    const double vy = xmlSvc()->eval(yAttribute ,false) * m_yunit;
    dataObj->table().push_back(TabulatedProperty::Entry(vx, vy));
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to tabulated property : "
        << tagNameString << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Fill an object with a new text child
// -----------------------------------------------------------------------
StatusCode
XmlTabulatedPropertyCnv::i_fillObj (xercesc::DOMText* childText,
                                    DataObject* refpObject,
                                    IOpaqueAddress* /*address*/) {
  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // gets the text
  const XMLCh* text = childText->getData();
  char *textValue = xercesc::XMLString::transcode(text);

  // parses it
  std::string x ;
  std::string y ;
  std::istringstream cstr (textValue);
  while (cstr >> x >> y) {
    const double vx = xmlSvc()->eval (x, false) * m_xunit;
    const double vy = xmlSvc()->eval (y, false) * m_yunit;
    /// add a new entry to the table
    dataObj->table().push_back (TabulatedProperty::Entry (vx, vy));
    x.erase();
    y.erase();
  }
  xercesc::XMLString::release(&textValue);
  if (!x.empty() || !y.empty()) {
    throw XmlCnvException
      ("XmlTabulatedPropertyCnv: Corrupted Data #x does not match #y! ");
  }
  // returns
  return StatusCode::SUCCESS;
}  // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlTabulatedPropertyCnv::i_processObj (DataObject* refpObject,
                                                 IOpaqueAddress* /*address*/) {
  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // sorts the table
  std::sort (dataObj->table().begin(), dataObj->table().end());
  m_xunit = 1;
  m_yunit = 1;
  // returns
  return StatusCode::SUCCESS;
} // end i_processObj

