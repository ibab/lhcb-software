// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlTabulatedPropertyCnv.cpp,v 1.7 2001-11-20 15:22:25 sponce Exp $

// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/IXmlSvc.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "XmlTabulatedPropertyCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlTabulatedPropertyCnv> s_FactoryXmlTabulatedPropertyCnv;
const ICnvFactory& XmlTabulatedPropertyCnvFactory =
  s_FactoryXmlTabulatedPropertyCnv;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlTabulatedPropertyCnv::XmlTabulatedPropertyCnv (ISvcLocator* svcs) :
  XmlGenericCnv (svcs, classID()),
  m_xunit (1),
  m_yunit (1){
};


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlTabulatedPropertyCnv::i_createObj (DOM_Element element,
                                                 DataObject*& refpObject) {
  m_xunit   = 1    ;
  m_yunit   = 1    ;

  // creates an object for the node found
  std::string elementName = dom2Std (element.getAttribute("name"));
  TabulatedProperty* dataObj = new TabulatedProperty (elementName);
  refpObject = dataObj;

  /// get all attributes           
  dataObj->table().clear()   ;
  const std::string Type = dom2Std (element.getAttribute ("type"));
  dataObj->setType(Type) ;
  const std::string xAxis= dom2Std (element.getAttribute ("xaxis"));
  dataObj->setXAxis(xAxis) ;
  const std::string yAxis= dom2Std (element.getAttribute ("yaxis"));
  dataObj->setYAxis(yAxis) ;
  std::string xUnitAttribute = dom2Std (element.getAttribute ("xunit"));
  if (xUnitAttribute.empty()) {
    m_xunit = 1.0;
  } else {
    m_xunit = xmlSvc()->eval(xUnitAttribute, false);
  }
  std::string yUnitAttribute = dom2Std (element.getAttribute ("yunit"));
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
StatusCode XmlTabulatedPropertyCnv::i_fillObj (DOM_Element childElement,
                                               DataObject* refpObject) {
  MsgStream log(msgSvc(), "TabulatedPropertyCnv" );
  
  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if ("entry" == tagName) {
    std::string xAttribute = dom2Std (childElement.getAttribute ("x"));
    if (xAttribute.empty()) {
      throw XmlCnvException
        ("XmlTabulatedPropertyCnv: 'x' attribute for tag='entry'"
         " could bot be omitted! ");
    }
    const double vx = xmlSvc()->eval(xAttribute ,false) * m_xunit;
    std::string yAttribute = dom2Std (childElement.getAttribute ("y"));
    if (yAttribute.empty()) {
      throw XmlCnvException
        ("XmlTabulatedPropertyCnv: 'y' attribute for tag='entry'"
         " could bot be omitted! ");
    }
    const double vy = xmlSvc()->eval(yAttribute ,false) * m_yunit;
    dataObj->table().push_back(TabulatedProperty::Entry(vx, vy));
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to tabulated property : "
        << tagName << endreq;
  }
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Fill an object with a new text child
// -----------------------------------------------------------------------
StatusCode XmlTabulatedPropertyCnv::i_fillObj (DOM_Text childText,
                                               DataObject* refpObject) {
  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // gets the text
  DOMString text = childText.getData();
  char *textValue = text.transcode();

  // parses it
  std::string x ;
  std::string y ;
  std::istrstream cstr (textValue, text.length());
  while (cstr >> x >> y) {
    const double vx = xmlSvc()->eval (x, false) * m_xunit;
    const double vy = xmlSvc()->eval (y, false) * m_yunit;
    /// add a new entry to the table 
    dataObj->table().push_back (TabulatedProperty::Entry (vx, vy));
    x.erase();
    y.erase();
  }
  if (!x.empty() || !y.empty()) {
    throw XmlCnvException
      ("XmlTabulatedPropertyCnv: Currupted Data #x does not match #y! ");
  }
  // returns
  return StatusCode::SUCCESS;
}  // end i_fillObj


// -----------------------------------------------------------------------
// Process an object
// -----------------------------------------------------------------------
StatusCode XmlTabulatedPropertyCnv::i_processObj (DataObject* refpObject) {
  // gets the object
  TabulatedProperty* dataObj = dynamic_cast<TabulatedProperty*> (refpObject);
  // sorts the table
  std::sort (dataObj->table().begin(), dataObj->table().end());
  m_xunit = 1;
  m_yunit = 1; 
  // returns
  return StatusCode::SUCCESS;
} // end i_processObj

