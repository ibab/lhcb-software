// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlSurfaceCnv.cpp,v 1.7 2002-01-22 14:26:22 sponce Exp $

// Include files
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/IXmlSvc.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/XmlCnvException.h"

#include "XmlSurfaceCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlSurfaceCnv>          s_FactoryXmlSurfaceCnv;
const ICnvFactory& XmlSurfaceCnvFactory = s_FactoryXmlSurfaceCnv;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlSurfaceCnv::XmlSurfaceCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, classID()) {
};


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlSurfaceCnv::i_createObj (DOM_Element element,
                                       DataObject*& refpObject) {
  MsgStream log (msgSvc(), "XmlSurfaceCnv");
  
  std::string elementName = dom2Std (element.getAttribute ("name"));
  Surface* dataObj= new Surface (elementName);
  refpObject = dataObj;
  
  // model 
  {
    const std::string value = dom2Std (element.getAttribute ("model"));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) { 
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "model"     << "'" 
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endreq; 
    }
    dataObj->setModel (i_value);
  }
  // finish
  {
    const std::string value = dom2Std (element.getAttribute ("finish"));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) { 
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "finish"    << "'" 
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endreq; 
    }           
    dataObj->setFinish (i_value); 
  }
  // type 
  {
    const std::string value = dom2Std (element.getAttribute ("type"));
    const double v_value = xmlSvc()->eval(value, false);
    const unsigned int i_value = (unsigned int) v_value; 
    if (i_value != v_value) {
      log << MSG::WARNING 
          << " Surface("         << elementName << ")"  
          << " parameter '"      << "type"      << "'"  
          << "("                 << value       << ")"
          << "="                 << v_value 
          << " is truncated to=" << i_value     << endreq; 
    }
    dataObj->setType (i_value); 
  }
  // value 
  {
    const std::string value = dom2Std (element.getAttribute ("value"));
    const double v_value = xmlSvc()->eval(value, false);
    dataObj->setValue (v_value);
  }
  // first volume  
  {
    const std::string value = dom2Std (element.getAttribute ("volfirst"));
    dataObj->setFirstVol (value);
  }
  // second volume  
  { 
    const std::string value = dom2Std (element.getAttribute ("volsecond"));
    dataObj->setSecondVol (value); 
  }
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlSurfaceCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject,
                                     IOpaqueAddress* /*address*/) {
  MsgStream log(msgSvc(), "XmlSurfaceCnv" );
  
  // gets the object
  Surface* dataObj = dynamic_cast<Surface*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if( "tabprops" == tagName) {
    log << MSG::VERBOSE << "looking at tabprops" << endreq;
    // if we have a tabprops element, adds it to the current object
    const std::string address =
      dom2Std (childElement.getAttribute ("address"));
    long linkID = dataObj->linkMgr()->addLink(address, 0);
    SmartRef<TabulatedProperty> ref(dataObj, linkID);
    dataObj->tabulatedProperties().push_back(ref); 
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to surface : "
        << tagName << endreq;
  }
  return StatusCode::SUCCESS;
} // end i_fillObj


