// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlGenericCnv.cpp,v 1.3 2001-11-20 15:22:24 sponce Exp $

// Include files
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/XmlCnvException.h"

#include <util/XMLUni.hpp>
#include <util/XMLString.hpp>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/IXmlSvc.h"
#include "DetDesc/XmlCnvAttrList.h"


// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlGenericCnv::XmlGenericCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (XML_StorageType, clid, svc),
  m_objRcpt (0), 
  m_xmlSvc (0) {
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::initialize() {
  // Initializes the grand father
  StatusCode status = Converter::initialize();

  // Locates the Detector Data Service 
  IDataProviderSvc* dp;
  serviceLocator()->getService ("DetectorDataSvc",
                                IID_IDataProviderSvc,
                                (IInterface*&)dp);
  setDataProvider (dp);
  
  // Locate the Xml Conversion Service
  serviceLocator()->getService ("XmlCnvSvc",
                                IID_IXmlSvc,
                                (IInterface*&)m_xmlSvc);

  // returns
  return status;
}


// -----------------------------------------------------------------------
// Finalize the converter
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::finalize() {
  // RIP dear grand father!
  return Converter::finalize();
}


// -----------------------------------------------------------------------
// Create the transient representation of an object.
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::createObj (IOpaqueAddress* pAddress,
                                     DataObject*&    refpObject)  {
   // creates a msg stream for debug purposes
   MsgStream log( msgSvc(), "XmlGenericCnv" );
   
   // retrieves the address for the object we have to create
   GenericAddress* addr;
   if (0 != pAddress) {
     try {
       addr = dynamic_cast<GenericAddress*> (pAddress);
     } catch( ... ) {
       return StatusCode::FAILURE;
     }
   } else {
     return StatusCode::FAILURE;
   }

   // stores the address. Needed ? - FIXME
   m_objRcpt = addr;

   // displays the address for debug purposes  
   log << MSG::DEBUG << "Address : dbname = " << addr->par()[0]
       << ", ObjectName = " << addr->par()[1] << endreq;

   // parses the xml file and retrieves a DOM document
   DOM_Document document = xmlSvc()->parse(addr->par()[0].c_str());
   if (document.isNull()) {
     log << MSG::FATAL
         << "XmlParser failed, can't convert "
         << addr->par()[1] << "!" << endreq;
     return StatusCode::FAILURE;
   }

   // checks version of the file
   // first find the "DDDB" or "materials" element
   DOM_NodeList list = document.getChildNodes();
   DOM_Element mainNode;
   unsigned int index;
   for (index = 0;
        index < list.getLength() && mainNode.isNull();
        index++) {
     if (list.item(index).getNodeType() == DOM_Node::ELEMENT_NODE) {
       DOM_Node childNode = list.item(index);
       DOM_Element childElement = (DOM_Element &) childNode;
       log << MSG::DEBUG << "found element "
           << dom2Std (childElement.getNodeName())
           << " at top level of xml file." << endreq;
       if (childElement.getNodeName().equals("DDDB") ||
           childElement.getNodeName().equals("materials")) {
         mainNode = childElement;
       }
     }
   }
   // check it exists
   if (mainNode.isNull()) {
     log << MSG::FATAL << addr->par()[1]
         << " has no DDDB element at the beginning of the file."
         << endreq;
     return StatusCode::FAILURE;
   } else {
     // for DDDB, checks the version attribute
     if (mainNode.getNodeName().equals("DDDB")) {
       std::string versionAttribute =
         dom2Std (mainNode.getAttribute ("version"));
       log << MSG::DEBUG
           << "Detector Description Markup Language Version "
           << versionAttribute << endreq;
       if (versionAttribute != "3.3" ) {
         StatusCode sc;
         sc.setCode(WRONG_DTD_VERSION);
         std::string msg = "DDDB DTD Version 3.3 required, ";
         msg += "please update your DTD and XML data files.";
         throw XmlCnvException(msg.c_str(), sc);
       }
     }
   }
   // deals with parameter definitions
   // get the parameters
   DOMString parameterTag ("parameter");
   DOM_NodeList parameterList = mainNode.getElementsByTagName(parameterTag);
   unsigned int k;
   for (k = 0; k < parameterList.getLength(); k++) {
     DOM_Node parameterNode = parameterList.item(k);
     DOM_Element parameter = (DOM_Element &) parameterNode;
     std::string name = dom2Std (parameter.getAttribute ("name"));
     std::string value = dom2Std (parameter.getAttribute ("value"));
     xmlSvc()->addParameter(name, value);
     log << MSG::DEBUG
         << "Added DDDB Parameter " << name << " = " << value << endreq;
   }
   
   // retrieve the name of the object we want to create. Removes the leading
   // '/' if needed
   std::string objectName = addr->par()[1];
   unsigned int slashPosition = objectName.find_last_of('/');
   if (std::string::npos != slashPosition) {
     objectName= objectName.substr(slashPosition + 1);
   }
  
   // finds the corresponding node in the DOM tree
   DOMString domObjectName(objectName.c_str());
   DOM_Element element = document.getElementById(domObjectName);
   if (element.isNull()){
     log << MSG::FATAL
         << objectName << " : "
         << "No such object in file " << addr->par()[0]
         << endreq;
     return StatusCode::FAILURE;
   }
   
   try {
     // deal with the node found itself
     return internalCreateObj (element, refpObject);
   } catch (GaudiException e) {
     log << MSG::FATAL << "An exception went out of the conversion process : ";
     e.printOut (log);
     log << endreq;
     return StatusCode::FAILURE;
   }
   
} // end createObj


// -----------------------------------------------------------------------
// create the object from a DOM Element
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::internalCreateObj (DOM_Element element,
                                             DataObject*& refpObject) {
  // creates a msg stream for debug purposes
  MsgStream log( msgSvc(), "XmlGenericCnv" );

  // creates an object for the node found
  StatusCode sc = i_createObj (element, refpObject);
  if (sc.isFailure()) {
    return sc;
  }

  // fills the object with its children
  // gets the children
  DOM_NodeList childList = element.getChildNodes();
  // scans them
  unsigned int i;
  for (i = 0; i < childList.getLength(); i++) {
    if (childList.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
      // gets the current child
      DOM_Node childNode = childList.item(i);
      DOM_Element childElement = (DOM_Element &) childNode;
      // calls fill_obj on it
      StatusCode sc2 = i_fillObj(childElement, refpObject);
      if (sc2.isFailure()) {
        log << MSG::ERROR << "unable to fill "
            << dom2Std (element.getNodeName())
            << " with its child "
            << dom2Std (childElement.getNodeName())
            << endreq;
      }
    } else if (childList.item(i).getNodeType() == DOM_Node::TEXT_NODE) {
      // gets the current child
      DOM_Node childNode = childList.item(i);
      DOM_Text textNode = (DOM_Text &) childNode;
      // calls fill_obj on it
      StatusCode sc2 = i_fillObj(textNode, refpObject);
      if (sc2.isFailure()) {
        log << MSG::ERROR << "unable to fill "
            << dom2Std(element.getNodeName())
            << " with text node containing : \""
            << dom2Std (textNode.getData())
            << endreq;
      }     
    }
  }
  
  // ends up the object construction
  return i_processObj(refpObject);
} // end internalCreateObj


// -----------------------------------------------------------------------
// Update the transient object from the other representation.
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::updateObj (IOpaqueAddress* /*pAddress*/,
                                     DataObject*     /*refpObject*/) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Convert the transient object to the requested representation.
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::createRep (DataObject*      /*pObject*/,
                                     IOpaqueAddress*& /*refpAddress*/) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Update the converted representation of a transient object.
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::updateRep (IOpaqueAddress* /*pAddress*/,
                                     DataObject*     /*pObject*/) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_createObj (DOM_Element childElement,
                                       DataObject*& refpObject) {
  return StatusCode::FAILURE;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (DOM_Element childElement,
                                     DataObject* refpObject) {
  return StatusCode::SUCCESS;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new text child
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (DOM_Text childText,
                                     DataObject* refpObject) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Process an object already created an filled
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_processObj(DataObject* refpObject) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// build a standard string from a DOMString
// -----------------------------------------------------------------------
const std::string XmlGenericCnv::dom2Std (DOMString domString) {
  char *cString = domString.transcode();
  std::string stdString;
  if (cString) {
    stdString = cString;
    delete [] (cString);
  }
  return stdString;
}
 
