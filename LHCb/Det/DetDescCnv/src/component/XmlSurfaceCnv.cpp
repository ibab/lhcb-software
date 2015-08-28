// Include files
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/TabulatedProperty.h"
#include "XmlTools/IXmlSvc.h"
#include "DetDescCnv/XmlCnvException.h"

#include "XmlSurfaceCnv.h"

// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlSurfaceCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlSurfaceCnv::XmlSurfaceCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, classID()) {
  nameString = xercesc::XMLString::transcode("name");
  modelString = xercesc::XMLString::transcode("model");
  finishString = xercesc::XMLString::transcode("finish");
  typeString = xercesc::XMLString::transcode("type");
  valueString = xercesc::XMLString::transcode("value");
  volfirstString = xercesc::XMLString::transcode("volfirst");
  volsecondString = xercesc::XMLString::transcode("volsecond");
  tabpropsString = xercesc::XMLString::transcode("tabprops");
  addressString = xercesc::XMLString::transcode("address");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlSurfaceCnv::~XmlSurfaceCnv () {
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&modelString);
  xercesc::XMLString::release((XMLCh**)&finishString);
  xercesc::XMLString::release((XMLCh**)&typeString);
  xercesc::XMLString::release((XMLCh**)&valueString);
  xercesc::XMLString::release((XMLCh**)&volfirstString);
  xercesc::XMLString::release((XMLCh**)&volsecondString);
  xercesc::XMLString::release((XMLCh**)&tabpropsString);
  xercesc::XMLString::release((XMLCh**)&addressString);
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlSurfaceCnv::i_createObj (xercesc::DOMElement* element,
                                       DataObject*& refpObject) {
  MsgStream log (msgSvc(), "XmlSurfaceCnv");
  
  std::string elementName = dom2Std (element->getAttribute (nameString));
  Surface* dataObj= new Surface (elementName);
  refpObject = dataObj;

#ifdef __INTEL_COMPILER         // Disable ICC remark
  #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif
  
  // model 
  {
    const std::string value = dom2Std (element->getAttribute (modelString));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) { 
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "model"     << "'" 
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endmsg; 
    }
    dataObj->setModel (i_value);
  }
  // finish
  {
    const std::string value = dom2Std (element->getAttribute (finishString));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) { 
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "finish"    << "'" 
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endmsg; 
    }           
    dataObj->setFinish (i_value); 
  }
  // type 
  {
    const std::string value = dom2Std (element->getAttribute (typeString));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) {
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "type"      << "'"  
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endmsg; 
    }
    dataObj->setType (i_value); 
  }
  // value 
  {
    const std::string value = dom2Std (element->getAttribute (valueString));
    const double v_value = xmlSvc()->eval(value, false);
    dataObj->setValue (v_value);
  }
  // first volume  
  {
    const std::string value = dom2Std (element->getAttribute (volfirstString));
    dataObj->setFirstVol (value);
  }
  // second volume  
  { 
    const std::string value = dom2Std (element->getAttribute (volsecondString));
    dataObj->setSecondVol (value); 
  }
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlSurfaceCnv::i_fillObj (xercesc::DOMElement* childElement,
                                     DataObject* refpObject,
                                     IOpaqueAddress* /*address*/) {
  MsgStream log(msgSvc(), "XmlSurfaceCnv" );
  
  // gets the object
  Surface* dataObj = static_cast<Surface*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  // dispatches, based on the name
  if( 0 == xercesc::XMLString::compareString(tabpropsString, tagName)) {
    if( log.level() <= MSG::DEBUG )
      log << MSG::VERBOSE << "looking at tabprops" << endmsg;
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement->getAttribute (addressString));
    long linkID = dataObj->linkMgr()->addLink(address, nullptr);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to surface : "
        << tagNameString << endmsg;
    xercesc::XMLString::release(&tagNameString);
  }
  return StatusCode::SUCCESS;
} // end i_fillObj


