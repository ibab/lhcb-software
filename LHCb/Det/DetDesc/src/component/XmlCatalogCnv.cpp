// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlCatalogCnv.cpp,v 1.6 2001-11-20 15:22:24 sponce Exp $

// include files
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
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

#include "DetDesc/IXmlSvc.h"
#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/CLIDIsotope.h"
#include "DetDesc/CLIDElement.h"
#include "DetDesc/CLIDMixture.h"
#include "DetDesc/XmlCnvException.h"

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
StatusCode XmlCatalogCnv::i_createObj (DOM_Element element,
                                       DataObject*& refpObject) {
  // creates an object for the node found
  refpObject = new DataObject();
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlCatalogCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject) {
  MsgStream log( msgSvc(), "XmlCatalogCnv" );
  // We will create an address for this element
  IOpaqueAddress* xmlAddr = 0;
  // the registry entry corresponding to the current object
  IRegistry* searchedDir = m_objRcpt->registry();
  // gets the element name
  std::string tagName (dom2Std (childElement.getNodeName()));
  std::string entryName;
  // first get the CLID and checks a converter exists for this type
  CLID clsID;
  if ("isotope" == tagName) {
    clsID = CLID_Isotope;
  } else if ("element" == tagName) {
    clsID = CLID_Element;
  } else if ("material" == tagName) {
    clsID = CLID_Mixture;
  } else {
    char *clidAttribute = childElement.getAttribute("classID").transcode();
    clsID = (CLID) atol(clidAttribute);
    delete [] clidAttribute;
  }
  log << MSG::VERBOSE << "tag is " << tagName << ", clsID is "
      << clsID << endreq;
  checkConverterExistence(clsID);
  StatusCode sc;
  // take care whether it is a reference or not
  if ("ref" == tagName.substr(tagName.length()-3,3)) {
    // gets the reference value and the position of the '#' in it
    std::string referenceValue = dom2Std (childElement.getAttribute ("href"));
    unsigned int poundPosition = referenceValue.find_last_of('#');
    // builds an entryName
    entryName = "/" + referenceValue.substr(poundPosition + 1);
    // gets the directory where the xmlFile is located
    unsigned int dPos  = m_objRcpt->par()[0].find_last_of('/');
    std::string locDir = m_objRcpt->par()[0].substr( 0, dPos + 1 );
    // builds the location of the file
    std::string location = referenceValue.substr(0, poundPosition);
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = m_objRcpt->par()[0];
    } else {
      location = locDir + location;
    }
    // builds an XmlAdress
    const std::string par[2] = {location, entryName};
    sc = addressCreator()->createAddress (XML_StorageType,
                                          clsID,
                                          par,
                                          0,
                                          xmlAddr);
  } else {
    // builds an entryName
    entryName = std::string("/") +
      dom2Std (childElement.getAttribute ("name"));
    // Then builds an XmlAdress
    const std::string par[2] = {m_objRcpt->par()[0], entryName};
    sc = addressCreator()->createAddress (XML_StorageType,
                                          clsID,
                                          par,
                                          0,
                                          xmlAddr);
  }
  if (sc.isSuccess()) {
    log << MSG::VERBOSE << " XML address : objectName = "
        << xmlAddr->par()[1]
        << ", dbName = " << xmlAddr->par()[0] << endreq; 
    // Now we have a new entry name and a corresponding xml address,
    // just add the new entry to the current registry entry
    IDataProviderSvc* dsvc = dataProvider();
    IDataManagerSvc* mgr = 0;
    StatusCode status =
      dsvc->queryInterface(IID_IDataManagerSvc,(void**)&mgr);
    if ( status.isSuccess() ) {
      status = mgr->registerAddress(searchedDir, entryName, xmlAddr);
      mgr->release();        
    }
    if ( !status.isSuccess() )   {
      log << MSG::FATAL << " File " << __FILE__ << " line "
          << __LINE__ << endreq;
      log << MSG::FATAL << " XML address:" << endreq;
      log << MSG::FATAL << " entryName:" << entryName
          << " objectName: " << xmlAddr->par()[1]
          << " dbName:" << xmlAddr->par()[0] << endreq;
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
        << "File " << m_objRcpt->par()[0]
        << " class ID "
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
