// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDescCnv/src/component/XmlCatalogCnv.cpp,v 1.1.1.1 2003-04-23 13:59:46 sponce Exp $

// include files
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include <dom/DOMString.hpp>
#include <dom/DOM_Node.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_NamedNodeMap.hpp>

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
static CnvFactory<XmlCatalogCnv> s_factory;
const ICnvFactory& XmlCatalogCnvFactory = s_factory;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlCatalogCnv::XmlCatalogCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_Catalog) {
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlCatalogCnv::i_createObj (DOM_Element /*element*/,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  refpObject = new DataObject();
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// get the CLID of a given element
// -----------------------------------------------------------------------
CLID XmlCatalogCnv::getCLID (DOM_Element element) {
  // gets the element name
  std::string tagName (dom2Std (element.getNodeName()));
  // compare the tagName to a set of known tags
  if (("isotope" == tagName) || ("isotoperef" == tagName)) {
    return CLID_Isotope;
  } else if (("element" == tagName) || ("elementref" == tagName)) {
    return CLID_Element;
  } else if (("material" == tagName) || ("materialref" == tagName)) {
    return CLID_Mixture;
  } else if (("catalog" == tagName) || ("catalogref" == tagName)) {
    return CLID_Catalog;
  } else if (("tabproperty" == tagName) || ("tabpropertyref" == tagName)) {
    return CLID_TabulatedProperty;
  } else if (("surface" == tagName) || ("surfaceref" == tagName)) {
    return CLID_Surface;
  } else if (("logvol" == tagName) || ("logvolref" == tagName)) {
    return CLID_LVolume;
  } else if (("detelem" == tagName) || ("detelemref" == tagName)) {
    return CLID_DetectorElement;
  } else if (("condition" == tagName) || ("conditionref" == tagName)) {
    return CLID_Condition;
  } else {
    // the tag is unknown : return -1
    return CLID (-1);
  }
}

// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlCatalogCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* /*refpObject*/,
                                     IOpaqueAddress* address) {
  MsgStream log( msgSvc(), "XmlCatalogCnv" );
  // the registry entry corresponding to the current object
  IRegistry* searchedDir = address->registry();
  // gets the element name
  std::string tagName (dom2Std (childElement.getNodeName()));
  // first get the CLID and checks a converter exists for this type
  CLID clsID = getCLID (childElement);
  if (CLID (-1) == clsID) {
    // bad CLID
    MsgStream log (msgSvc(), "XmlCatalogCnv");
    log << MSG::ERROR << tagName << " : unable to find a CLID for this tag."
        << " This tag may be forbiden as child of catalog."
        << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::VERBOSE << "tag is " << tagName << ", clsID is "
      << clsID << endreq;
  checkConverterExistence(clsID);
  StatusCode sc;
  std::string entryName;
  // We will create an address for this element
  IOpaqueAddress* xmlAddr = 0;
  // take care whether it is a reference or not
  if ("ref" == tagName.substr(tagName.length()-3,3)) {
    // gets the reference value and the position of the '#' in it
    std::string referenceValue = dom2Std (childElement.getAttribute ("href"));
    // creates the address
    xmlAddr = createAddressForHref (referenceValue, clsID, address);
  } else {
    // builds an entryName
    entryName = std::string("/") + dom2Std (childElement.getAttribute ("name"));
    // Then builds an XmlAdress
    xmlAddr = createXmlAddress (address->par()[0], entryName, clsID);
  }
  if (sc.isSuccess()) {
    // Now we have a new entry name and a corresponding xml address,
    // just add the new entry to the current registry entry
    IDataProviderSvc* dsvc = dataProvider();
    IDataManagerSvc* mgr = 0;
    StatusCode status =
      dsvc->queryInterface(IID_IDataManagerSvc,(void**)&mgr);
    if ( status.isSuccess() ) {
      status = mgr->registerAddress(searchedDir, xmlAddr->par()[1], xmlAddr);
      mgr->release();        
    }
    if ( !status.isSuccess() )   {
      log << MSG::FATAL << " File " << __FILE__ << " line "
          << __LINE__ << endreq;
      log << MSG::FATAL << " XML address:" << endreq;
      log << MSG::FATAL << " location : " << xmlAddr->par()[0]
          << " entryName : " << xmlAddr->par()[1]
          << " isString : " << xmlAddr->ipar()[0] << endreq;
      xmlAddr->release();
      xmlAddr = 0;
      StatusCode stcod = ERROR_ADDING_TO_TS;
      throw XmlCnvException 
        ("Error adding registry entry to detector transient store", stcod);
    }
  }
  
  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Check the existence of a converter
// -----------------------------------------------------------------------
void XmlCatalogCnv::checkConverterExistence (const CLID& clsID) {
  // Checking the other incoming guys according to our DTD!
  bool cnvExists;
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
        << clsID << ", proper converter not found" << endreq;
    stcod.setCode (INVALID_CLASS_ID);  
    throw XmlCnvException ("Unknown class ID", stcod);
  } 
}


// -----------------------------------------------------------------------
// Accessor to the classID
// -----------------------------------------------------------------------
const CLID& XmlCatalogCnv::classID() {
  return CLID_Catalog;
}
