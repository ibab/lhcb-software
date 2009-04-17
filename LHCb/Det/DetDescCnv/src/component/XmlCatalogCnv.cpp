// $Id: XmlCatalogCnv.cpp,v 1.9 2009-04-17 12:25:18 cattanem Exp $

// include files
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartIF.h"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>

#include "DetDesc/CLIDCondition.h"
#include "DetDesc/CLIDDetectorElement.h"
#include "DetDesc/CLIDLVolume.h"
#include "DetDesc/CLIDSurface.h"
#include "DetDesc/CLIDIsotope.h"
#include "DetDesc/CLIDElement.h"
#include "DetDesc/CLIDMixture.h"
#include "DetDesc/CLIDTabulatedProperty.h"

#include "XmlTools/IXmlSvc.h"
#include "DetDescCnv/XmlCnvException.h"

#include "XmlCatalogCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
DECLARE_CONVERTER_FACTORY(XmlCatalogCnv)

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlCatalogCnv::XmlCatalogCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_Catalog) {
  isotopeString = xercesc::XMLString::transcode("isotope");
  materialString = xercesc::XMLString::transcode("material");
  catalogString = xercesc::XMLString::transcode("catalog");
  tabpropertyString = xercesc::XMLString::transcode("tabproperty");
  surfaceString = xercesc::XMLString::transcode("surface");
  logvolString = xercesc::XMLString::transcode("logvol");
  detelemString = xercesc::XMLString::transcode("detelem");
  elementString = xercesc::XMLString::transcode("element");
  conditionString = xercesc::XMLString::transcode("condition");

  isotoperefString = xercesc::XMLString::transcode("isotoperef");
  materialrefString = xercesc::XMLString::transcode("materialref");
  catalogrefString = xercesc::XMLString::transcode("catalogref");
  tabpropertyrefString = xercesc::XMLString::transcode("tabpropertyref");
  surfacerefString = xercesc::XMLString::transcode("surfaceref");
  logvolrefString = xercesc::XMLString::transcode("logvolref");
  detelemrefString = xercesc::XMLString::transcode("detelemref");
  elementrefString = xercesc::XMLString::transcode("elementref");
  conditionrefString = xercesc::XMLString::transcode("conditionref");

  refString = xercesc::XMLString::transcode("ref");
  hrefString = xercesc::XMLString::transcode("href");
  nameString = xercesc::XMLString::transcode("name");
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlCatalogCnv::~XmlCatalogCnv () {
  xercesc::XMLString::release((XMLCh**)&isotopeString);
  xercesc::XMLString::release((XMLCh**)&materialString);
  xercesc::XMLString::release((XMLCh**)&catalogString);
  xercesc::XMLString::release((XMLCh**)&tabpropertyString);
  xercesc::XMLString::release((XMLCh**)&surfaceString);
  xercesc::XMLString::release((XMLCh**)&logvolString);
  xercesc::XMLString::release((XMLCh**)&detelemString);
  xercesc::XMLString::release((XMLCh**)&elementString);
  xercesc::XMLString::release((XMLCh**)&conditionString);

  xercesc::XMLString::release((XMLCh**)&isotoperefString);
  xercesc::XMLString::release((XMLCh**)&materialrefString);
  xercesc::XMLString::release((XMLCh**)&catalogrefString);
  xercesc::XMLString::release((XMLCh**)&tabpropertyrefString);
  xercesc::XMLString::release((XMLCh**)&surfacerefString);
  xercesc::XMLString::release((XMLCh**)&logvolrefString);
  xercesc::XMLString::release((XMLCh**)&detelemrefString);
  xercesc::XMLString::release((XMLCh**)&elementrefString);
  xercesc::XMLString::release((XMLCh**)&conditionrefString);

  xercesc::XMLString::release((XMLCh**)&refString);
  xercesc::XMLString::release((XMLCh**)&hrefString);
  xercesc::XMLString::release((XMLCh**)&nameString);
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlCatalogCnv::i_createObj (xercesc::DOMElement* /*element*/,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  refpObject = new DataObject();
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// get the CLID of a given element
// -----------------------------------------------------------------------
CLID XmlCatalogCnv::getCLID (xercesc::DOMElement* element) {
  // gets the element name
  const XMLCh* tagName = element->getNodeName();
  // compare the tagName to a set of known tags
  if ((0 == xercesc::XMLString::compareString(isotopeString, tagName)) ||
      (0 == xercesc::XMLString::compareString(isotoperefString, tagName))) {
    return CLID_Isotope;
  } else if ((0 == xercesc::XMLString::compareString
              (elementString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (elementrefString, tagName))) {
    return CLID_Element;
  } else if ((0 == xercesc::XMLString::compareString
              (materialString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (materialrefString, tagName))) {
    return CLID_Mixture;
  } else if ((0 == xercesc::XMLString::compareString
              (catalogString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (catalogrefString, tagName))) {
    return CLID_Catalog;
  } else if ((0 == xercesc::XMLString::compareString
              (tabpropertyString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (tabpropertyrefString, tagName))) {
    return CLID_TabulatedProperty;
  } else if ((0 == xercesc::XMLString::compareString
              (surfaceString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (surfacerefString, tagName))) {
    return CLID_Surface;
  } else if ((0 == xercesc::XMLString::compareString
              (logvolString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (logvolrefString, tagName))) {
    return CLID_LVolume;
  } else if ((0 == xercesc::XMLString::compareString
              (detelemString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (detelemrefString, tagName))) {
    return CLID_DetectorElement;
  } else if ((0 == xercesc::XMLString::compareString
              (conditionString, tagName)) ||
             (0 == xercesc::XMLString::compareString
              (conditionrefString, tagName))) {
    return CLID_Condition;
  } else {


    // the tag is unknown : return -1
    return CLID (-1);
  }
}

// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlCatalogCnv::i_fillObj (xercesc::DOMElement* childElement,
                                     DataObject* /*refpObject*/,
                                     IOpaqueAddress* address) {
  MsgStream log( msgSvc(), "XmlCatalogCnv" );
  // the registry entry corresponding to the current object
  IRegistry* searchedDir = address->registry();
  // gets the element name
  const XMLCh* tagName = childElement->getNodeName();
  // first get the CLID and checks a converter exists for this type
  CLID clsID = getCLID (childElement);
  char* tagNameString = xercesc::XMLString::transcode(tagName);
  if (CLID (-1) == clsID) {
    // bad CLID
    MsgStream log (msgSvc(), "XmlCatalogCnv");
    log << MSG::ERROR << tagNameString
        << " : unable to find a CLID for this tag."
        << " This tag may be forbiden as child of catalog."
        << endmsg;
    return StatusCode::FAILURE;
  }
  log << MSG::VERBOSE << "tag is " << tagNameString
      << ", clsID is " << clsID << endmsg;
  xercesc::XMLString::release(&tagNameString);
  checkConverterExistence(clsID);
  std::string entryName;
  // We will create an address for this element
  IOpaqueAddress* xmlAddr = 0;
  // take care whether it is a reference or not
  XMLCh* tagNameEnd = new XMLCh[4];
  xercesc::XMLString::subString
    (tagNameEnd,
     tagName,
     xercesc::XMLString::stringLen(tagName) - 3,
     xercesc::XMLString::stringLen(tagName));
  
  // in order to handle properly both XML files and CondDB XML strings
  // we use always the "createAddressForHref" system
  std::string referenceValue;
  if (0 == xercesc::XMLString::compareString(refString, tagNameEnd)) {
    // that's a real *ref entry
    referenceValue = dom2Std (childElement->getAttribute (hrefString));
  } else {
    // it is an element: build a (fake) reference
    referenceValue = std::string("#") + dom2Std (childElement->getAttribute (nameString));
  }
  // creates the address
  xmlAddr = createAddressForHref (referenceValue, clsID, address);

  delete[] tagNameEnd;

  StatusCode status = StatusCode::FAILURE;
  SmartIF<IDataManagerSvc> mgr( dataProvider() );
  if ( mgr ) {
    status = mgr->registerAddress(searchedDir, xmlAddr->par()[1], xmlAddr);
  }
  if ( !status.isSuccess() )   {
    log << MSG::FATAL << " File " << __FILE__ << " line " << __LINE__ << endmsg;
    log << MSG::FATAL << " XML address:" << endmsg;
    log << MSG::FATAL << " location : " << xmlAddr->par()[0]
        << " entryName : " << xmlAddr->par()[1]
        << " isString : " << xmlAddr->ipar()[0] << endmsg;
    xmlAddr->release();
    xmlAddr = 0;
    StatusCode stcod = ERROR_ADDING_TO_TS;
    throw XmlCnvException 
      ("Error adding registry entry to detector transient store", stcod);
  }
  
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Check the existence of a converter
// -----------------------------------------------------------------------
void XmlCatalogCnv::checkConverterExistence (const CLID& clsID) {
  // Checking the other incoming guys according to our DTD!
  if ( conversionSvc() && conversionSvc()->converter(clsID) != 0 ) return;
  else {
    MsgStream log (msgSvc(), "XmlCatalogCnv");
    log << MSG::ERROR
        << "class ID "
        << clsID << ", proper converter not found" << endmsg;
    throw XmlCnvException ("Unknown class ID", StatusCode(INVALID_CLASS_ID));
    
  }
#if 0 
  bool cnvExists = false;
  ICnvManager* cnvMgr;
  
  StatusCode stcod = serviceLocator()->queryInterface (IID_ICnvManager,
                                                       (void **)&cnvMgr);
  if (stcod.isFailure()) {
    stcod.setCode (CANT_QUERY_INTERFACE);  
    throw XmlCnvException ("Query to ICnvManager interface failed", stcod);
  }

  // Check whether the converter for this storage type
  // and class ID is available
  cnvExists = cnvMgr->existsConverter(XML_StorageType, clsID);
  cnvMgr->release();
  if (!cnvExists) {
    MsgStream log (msgSvc(), "XmlCatalogCnv");
    log << MSG::ERROR
        << "class ID "
        << clsID << ", proper converter not found" << endmsg;
    stcod.setCode (INVALID_CLASS_ID);  
    throw XmlCnvException ("Unknown class ID", stcod);
  } 
#endif
}


// -----------------------------------------------------------------------
// Accessor to the classID
// -----------------------------------------------------------------------
const CLID& XmlCatalogCnv::classID() {
  return CLID_Catalog;
}
