// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDescCnv/src/Lib/XmlGenericCnv.cpp,v 1.2 2003-04-24 09:15:33 sponce Exp $

// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDescCnv/XmlCnvException.h"

#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "XmlTools/IXmlSvc.h"

// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlGenericCnv::XmlGenericCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (XML_StorageType, clid, svc),
  m_xmlSvc (0) {
  DDDBString = xercesc::XMLString::transcode("DDDB");
  materialsString = xercesc::XMLString::transcode("materials");
  versionString = xercesc::XMLString::transcode("version");
  DTD_VersionString = xercesc::XMLString::transcode("DTD_Version");
  macroString = xercesc::XMLString::transcode("macro");
  nameString = xercesc::XMLString::transcode("name");
  valueString = xercesc::XMLString::transcode("value");
  parameterString = xercesc::XMLString::transcode("parameter");
  detelemString = xercesc::XMLString::transcode("detelem");
  conditionString = xercesc::XMLString::transcode("condition");
  classIDString = xercesc::XMLString::transcode("classID");
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::initialize() {
  // Initializes the grand father
  StatusCode status = Converter::initialize();

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
  // release XmlCnvSvc
  m_xmlSvc->release();
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
   xercesc::DOMDocument* document = 0;
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
   if (0 == document) {
     log << MSG::FATAL
         << "XmlParser failed, can't convert "
         << addr->par()[1] << "!" << endreq;
     return StatusCode::FAILURE;
   }

   // checks version of the file
   // first find the "DDDB" or "materials" element
   xercesc::DOMNodeList* list = document->getChildNodes();
   xercesc::DOMElement* mainNode = 0;
   unsigned int index;
   for (index = 0;
        index < list->getLength() && 0 == mainNode;
        index++) {
     if (list->item(index)->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
       xercesc::DOMNode* childNode = list->item(index);
       xercesc::DOMElement* childElement = (xercesc::DOMElement*) childNode;
       log << MSG::VERBOSE << "found element "
           << dom2Std (childElement->getNodeName())
           << " at top level of xml file." << endreq;
       if (0 == xercesc::XMLString::compareString(childElement->getNodeName(),
                                         DDDBString) ||
           0 == xercesc::XMLString::compareString(childElement->getNodeName(),
                                         materialsString)) {
         mainNode = childElement;
       }
     }
   }
   // check it exists
   if (0 == mainNode) {
     log << MSG::FATAL << addr->par()[1]
         << " has no DDDB element at the beginning of the file."
         << endreq;
     return StatusCode::FAILURE;
   } else {
     // checks the version attribute
     std::string versionAttribute;
     std::string defaultMajorVersion;
     std::string defaultMinorVersion;
     if (0 == xercesc::XMLString::compareString
         (mainNode->getNodeName(), DDDBString)) {
       versionAttribute = dom2Std (mainNode->getAttribute (versionString));
       defaultMajorVersion = "3";
       defaultMinorVersion = "3";
     } else {
       versionAttribute = dom2Std (mainNode->getAttribute (DTD_VersionString));
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
   xercesc::DOMNodeList* macroList =
     mainNode->getElementsByTagName(macroString);
   unsigned int k;
   for (k = 0; k < macroList->getLength(); k++) {
     xercesc::DOMNode* macroNode = macroList->item(k);
     xercesc::DOMElement* macro = (xercesc::DOMElement*) macroNode;
     std::string name = dom2Std (macro->getAttribute (nameString));
     std::string value = dom2Std (macro->getAttribute (valueString));
     xmlSvc()->addParameter(name, value);
     log << MSG::DEBUG
         << "Added DDDB Macro " << name << " = " << value << endreq;
   }
   // deals with old parameter definitions
   // get the parameters
   xercesc::DOMNodeList* parameterList =
     mainNode->getElementsByTagName(parameterString);
   unsigned int kk;
   for (kk = 0; kk < parameterList->getLength(); kk++) {
     xercesc::DOMNode* parameterNode = parameterList->item(kk);
     xercesc::DOMElement* parameter = (xercesc::DOMElement*) parameterNode;
     std::string name = dom2Std (parameter->getAttribute (nameString));
     std::string value = dom2Std (parameter->getAttribute (valueString));
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
   xercesc::DOMElement* element =
     document->getElementById
     (xercesc::XMLString::transcode(objectName.c_str()));
   if (0 == element){
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
StatusCode XmlGenericCnv::internalCreateObj (xercesc::DOMElement* element,
                                             DataObject*& refpObject,
                                             IOpaqueAddress* address) {
  // creates a msg stream for debug purposes
  MsgStream log( msgSvc(), "XmlGenericCnv" );

  // In case we are dealing with xml elements that can be extended (detelem
  // or condition to be short), the clid of the element needs to be checked
  // here and the proper converter loaded if we are not in the right one
  XmlGenericCnv* converter = this;
  const XMLCh* tagName = element->getNodeName();
  if (0 == xercesc::XMLString::compareString(detelemString, tagName) ||
      0 == xercesc::XMLString::compareString(conditionString, tagName)) {
    const XMLCh *clidAttribute = element->getAttribute(classIDString);
    const CLID clsID = xercesc::XMLString::parseInt(clidAttribute);
    if (clsID != this->objType()) {
      IConverter* conv = this->conversionSvc()->converter(clsID);
      if (0 == conv) {
        if (this->xmlSvc()->allowGenericCnv()) {
          log << MSG::DEBUG;
        } else {
          log << MSG::ERROR;
        }
        log << "No proper converter found for classID " << clsID
            << ", the default converter for " << dom2Std(tagName)
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
  xercesc::DOMNodeList* childList = element->getChildNodes();
  // scans them
  unsigned int i;
  for (i = 0; i < childList->getLength(); i++) {
    if (childList->item(i)->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
      // gets the current child
      xercesc::DOMNode* childNode = childList->item(i);
      xercesc::DOMElement* childElement = (xercesc::DOMElement*) childNode;
      // calls fill_obj on it
      StatusCode sc2 = converter->i_fillObj(childElement, refpObject, address);
      if (sc2.isFailure()) {
        log << MSG::ERROR << "unable to fill "
            << dom2Std (element->getNodeName())
            << " with its child "
            << dom2Std (childElement->getNodeName())
            << endreq;
      }
    } else if (childList->item(i)->getNodeType() ==
               xercesc::DOMNode::TEXT_NODE) {
      // gets the current child
      xercesc::DOMNode* childNode = childList->item(i);
      xercesc::DOMText* textNode = (xercesc::DOMText*) childNode;
      // calls fill_obj on it
      StatusCode sc2 = converter->i_fillObj(textNode, refpObject, address);
      if (sc2.isFailure()) {
        log << MSG::ERROR << "unable to fill "
            << dom2Std(element->getNodeName())
            << " with text node containing : \""
            << dom2Std (textNode->getData())
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
StatusCode XmlGenericCnv::i_createObj (xercesc::DOMElement* /*childElement*/,
                                       DataObject*& /*refpObject*/) {
  return StatusCode::FAILURE;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (xercesc::DOMElement* /*childElement*/,
                                     DataObject* /*refpObject*/,
                                     IOpaqueAddress* /*address*/) {
  return StatusCode::SUCCESS;
}

  
// -----------------------------------------------------------------------
// Fill an object with a new text child
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::i_fillObj (xercesc::DOMText* /*childText*/,
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
const std::string XmlGenericCnv::dom2Std (const XMLCh* domString) {
  char *cString = xercesc::XMLString::transcode(domString);
  std::string stdString;
  if (cString) {
    stdString = cString;
    delete [] (cString);
  }
  return stdString;
}
 
