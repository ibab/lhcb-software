// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlGenericCnv.cpp,v 1.10 2002-05-24 15:28:36 sponce Exp $

// Include files
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/XmlCnvException.h"

#include <util/XMLUni.hpp>
#include <util/XMLString.hpp>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/IXmlSvc.h"
#include "DetDesc/XmlCnvAttrList.h"


// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlGenericCnv::XmlGenericCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (XML_StorageType, clid, svc),
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
StatusCode XmlGenericCnv::createObj (IOpaqueAddress* addr,
                                     DataObject*&    refpObject)  {
   // creates a msg stream for debug purposes
   MsgStream log( msgSvc(), "XmlGenericCnv" );
   
   // maked sure the address is not null
   if (0 == addr) {
     return StatusCode::FAILURE;
   }

   // displays the address for debug purposes  
   log << MSG::DEBUG << "Address : dbname = " << addr->par()[0]
       << ", ObjectName = " << addr->par()[1]
       << ", isString = " << addr->ipar()[0] << endreq;

   // parses the xml file or the xml string and retrieves a DOM document
   DOM_Document document;
   if ( 0 == addr->ipar()[0] ) {
     document = xmlSvc()->parse(addr->par()[0].c_str());
   } else if ( 1 == addr->ipar()[0] ) {
     document = xmlSvc()->parseString(addr->par()[0].c_str());
   } else {
     log << MSG::FATAL
         << "XmlParser failed, invalid flag isString="
         << addr->ipar()[0] << "!" << endreq;
     return StatusCode::FAILURE;
   }
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
       log << MSG::VERBOSE << "found element "
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
     // checks the version attribute
     std::string versionAttribute;
     std::string defaultMajorVersion;
     std::string defaultMinorVersion;
     if (mainNode.getNodeName().equals("DDDB")) {
       versionAttribute = dom2Std (mainNode.getAttribute ("version"));
       defaultMajorVersion = "3";
       defaultMinorVersion = "3";
     } else {
       versionAttribute = dom2Std (mainNode.getAttribute ("DTD_Version"));
       defaultMajorVersion = "v5";
       defaultMinorVersion = "0";
     }
     log << MSG::DEBUG
         << "Detector Description Markup Language Version "
         << versionAttribute << endreq;
     std::string::size_type dotPos = versionAttribute.find ('.');
     std::string majorVersion;
     std::string minorVersion = "0";       
     if (dotPos == std::string::npos) {
       majorVersion = versionAttribute;
     } else {
       majorVersion = versionAttribute.substr (0, dotPos);
       minorVersion = versionAttribute.substr (dotPos + 1);
     }
     if (majorVersion != defaultMajorVersion) {
       log << MSG::ERROR << "DDDB DTD Version " << defaultMajorVersion
           << "." << defaultMinorVersion << " required. "
           << "You are currently using Version " << versionAttribute
           << ". Please update your DTD and XML data files. "
           << "If you are using the XmlDDDB package, please "
           << "get a new version of it."
           << endreq;
       return StatusCode::FAILURE;
     } else if (minorVersion != defaultMinorVersion) {
       log << MSG::WARNING << "DDDB DTD Version " << defaultMajorVersion
           << "." << defaultMinorVersion << " recommanded. "
           << "You are currently using Version " << versionAttribute
           << ". Everything should work fine but you may get some "
           << "error messages about unknown tags."
           << endreq;
     }
   }
   // deals with macro definitions
   // get the parameters
   DOMString macroTag ("macro");
   DOM_NodeList macroList = mainNode.getElementsByTagName(macroTag);
   unsigned int k;
   for (k = 0; k < macroList.getLength(); k++) {
     DOM_Node macroNode = macroList.item(k);
     DOM_Element macro = (DOM_Element &) macroNode;
     std::string name = dom2Std (macro.getAttribute ("name"));
     std::string value = dom2Std (macro.getAttribute ("value"));
     xmlSvc()->addParameter(name, value);
     log << MSG::DEBUG
         << "Added DDDB Macro " << name << " = " << value << endreq;
   }
   // deals with old parameter definitions
   // get the parameters
   DOMString parameterTag ("parameter");
   DOM_NodeList parameterList = mainNode.getElementsByTagName(parameterTag);
   unsigned int kk;
   for (kk = 0; kk < parameterList.getLength(); kk++) {
     DOM_Node parameterNode = parameterList.item(kk);
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
     return internalCreateObj (element, refpObject, addr);
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
                                             DataObject*& refpObject,
                                             IOpaqueAddress* address) {
  // creates a msg stream for debug purposes
  MsgStream log( msgSvc(), "XmlGenericCnv" );

  // In case we are dealing with xml elements that can be extended (detelem
  // or condition to be short), the clid of the element needs to be checked
  // here and the proper converter loaded if we are not in the right one
  XmlGenericCnv* converter = this;
  std::string tagName = dom2Std (element.getNodeName());
  if (("detelem" == tagName) || ("condition" == tagName)) {
    char *clidAttribute = element.getAttribute("classID").transcode();
    const CLID clsID = (CLID) atol(clidAttribute);
    delete [] clidAttribute;
    if (clsID != this->objType()) {
      IConverter* conv = this->conversionSvc()->converter(clsID);
      if (0 == conv) {
        log << MSG::WARNING
            << "No proper converter found for classID " << clsID
            << ", the default converter for " << tagName
            << " will be used. This message may be ignored in case you are "
            << "fine with the default converter (ie you don't use the "
            << "content of the specific part)." << endreq;
      } else {
        converter = dynamic_cast <XmlGenericCnv*> (conv);
        if (0 == converter) {
          log << MSG::ERROR
              << "The converter found for classID " << clsID
              << " was not a descendant of XmlGenericCnv as it should be "
              << "( was of type " << typeid (*converter).name() << "). "
              << "The default converter will be used" << endreq;
          converter = this;
        }
      }
    }
  }
  
  // creates an object for the node found
  StatusCode sc = converter->i_createObj (element, refpObject);
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
      StatusCode sc2 = converter->i_fillObj(childElement, refpObject, address);
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
      StatusCode sc2 = converter->i_fillObj(textNode, refpObject, address);
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
  return converter->i_processObj(refpObject, address);
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
StatusCode XmlGenericCnv::i_createObj (DOM_Element /*childElement*/,
                                       DataObject*& /*refpObject*/) {
  return StatusCode::FAILURE;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (DOM_Element /*childElement*/,
                                     DataObject* /*refpObject*/,
                                     IOpaqueAddress* /*address*/) {
  return StatusCode::SUCCESS;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new text child
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (DOM_Text /*childText*/,
                                     DataObject* /*refpObject*/,
                                     IOpaqueAddress* /*address*/) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Process an object already created an filled
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_processObj(DataObject* /*refpObject*/,
                                       IOpaqueAddress* /*address*/) {
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// build an address from a href
// -----------------------------------------------------------------------
IOpaqueAddress*
XmlGenericCnv::createAddressForHref (std::string href,
                                     CLID clid,
                                     IOpaqueAddress* parent) const {
  MsgStream log( msgSvc(), "XmlGenericCnv" );
  // Is it a CondDB address ?
  bool condDB = (0==href.find("conddb:/"));
  if (condDB) {
    log << MSG::VERBOSE 
        << "Href points to a conddb URL: " << href << endreq;
    // first parse the href to get entryName and path in the CondDB
    unsigned int slashPosition = href.find_first_of('/');
    unsigned int poundPosition = href.find_last_of('#');
    // builds an entryName
    std::string entryName = "/" + href.substr( poundPosition+1 );
    // gets the directory in the CondDB
    std::string path = href.substr(slashPosition, 
                                   poundPosition - slashPosition);
    log << MSG::VERBOSE 
        << "Now build a CondDB address for path=" << path
        << " and entryName=" << entryName << endreq;
    // Then build a new Address
    return createCondDBAddress (path, entryName, clid);    
  } else {
    log << MSG::VERBOSE 
        << "Href points to a a regular URL: " << href << endreq;
    // here we deal with a regular URL
    // first parse the href to get entryName and location
    unsigned int poundPosition = href.find_last_of('#');
    // builds an entryName
    std::string entryName = "/" + href.substr(poundPosition + 1);
    // builds the location of the file
    std::string location = href.substr(0, poundPosition);
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = parent->par()[0];
    } else {
      // gets the directory where the xmlFile is located
      unsigned int dPos  = parent->par()[0].find_last_of('/');
      std::string locDir = parent->par()[0].substr( 0, dPos + 1 );
      location = locDir + location;
    }
    log << MSG::VERBOSE 
        << "Now build an XML address for location=" << location
        << " and entryName=" << entryName << endreq;
    // Then build a new Address
    return createXmlAddress (location, entryName, clid);  
  }
  // we never come here
  return 0;
}

// -----------------------------------------------------------------------
// build an address from location and entryName
// -----------------------------------------------------------------------
IOpaqueAddress* XmlGenericCnv::createXmlAddress (std::string location,
                                                 std::string entryName,
                                                 CLID clid) const {
  const std::string par[2] = {location, entryName};
  const unsigned long isString = 0; // address: filename (0) or string (1)?
  const unsigned long ipar[1] = { isString };
  IOpaqueAddress* result;
  StatusCode sc = addressCreator()->createAddress (XML_StorageType,
                                                   clid,
                                                   par,
                                                   ipar,
                                                   result);
  if (!sc.isSuccess()) {
    throw XmlCnvException ("XmlGenericCnv : Unable to create Address from href",
                           sc);
  }
  MsgStream log( msgSvc(), "XmlGenericCnv" );
  log << MSG::DEBUG << "New address created : location = "
      << location << ", entry name = " 
      << entryName << " isString : " << isString << endreq;
  return result;
}

// -----------------------------------------------------------------------
// build an address from location and entryName
// -----------------------------------------------------------------------
IOpaqueAddress* XmlGenericCnv::createCondDBAddress (std::string path,
                                                    std::string entryName,
                                                    CLID clid) const {
  const std::string par[2] = {path, entryName};
  IOpaqueAddress* result;
  StatusCode sc = addressCreator()->createAddress (CONDDB_StorageType,
                                                   clid,
                                                   par,
                                                   0,
                                                   result);
  if (!sc.isSuccess()) {
    throw XmlCnvException ("XmlGenericCnv : Unable to create Address from href",
                           sc);
  }
  MsgStream log( msgSvc(), "XmlGenericCnv" );
  log << MSG::DEBUG << "New address created : path = "
      << path << ", entry name = " << entryName << endreq;
  return result;
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
 
