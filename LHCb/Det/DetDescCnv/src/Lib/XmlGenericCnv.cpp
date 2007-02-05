// $Id: XmlGenericCnv.cpp,v 1.19 2007-02-05 18:55:33 marcocle Exp $

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

#include "DetDesc/ValidDataObject.h"

#include "XmlTools/IXmlSvc.h"

#include "AddressTools.h"

// -----------------------------------------------------------------------
// Standard Constructor
// ------------------------------------------------------------------------
XmlGenericCnv::XmlGenericCnv( ISvcLocator* svc, const CLID& clid) :
  Converter (XML_StorageType, clid, svc),
  m_xmlSvc (0),
  m_have_CONDDB_StorageType(false)
{
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
// Destructor
// ------------------------------------------------------------------------
XmlGenericCnv::~XmlGenericCnv() {
  xercesc::XMLString::release((XMLCh**)&DDDBString);
  xercesc::XMLString::release((XMLCh**)&materialsString);
  xercesc::XMLString::release((XMLCh**)&versionString);
  xercesc::XMLString::release((XMLCh**)&DTD_VersionString);
  xercesc::XMLString::release((XMLCh**)&macroString);
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&valueString);
  xercesc::XMLString::release((XMLCh**)&parameterString);
  xercesc::XMLString::release((XMLCh**)&detelemString);
  xercesc::XMLString::release((XMLCh**)&conditionString);
  xercesc::XMLString::release((XMLCh**)&classIDString);
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::initialize() {
  // Initializes the grand father
  StatusCode status = Converter::initialize();

  /*
  // Locate the Xml Conversion Service
  serviceLocator()->getService ("XmlCnvSvc",
  IID_IXmlSvc,
  (IInterface*&)m_xmlSvc);
  */

  // I need to check if I can create conddb addresses.
  // (sorry, I did not find a better way)
  IOpaqueAddress* result;
  const std::string par[2] = { "", "" };
  unsigned long channelId = 0;
  StatusCode sc = addressCreator()->createAddress (CONDDB_StorageType,
                                                   5,
                                                   par,
                                                   &channelId,
                                                   result);
  m_have_CONDDB_StorageType = sc.isSuccess(); // if we managed to create an address, it means we can
  if ( m_have_CONDDB_StorageType ) {
    delete result; // avoid memory leaks
  }

  // returns
  return status;
}


// -----------------------------------------------------------------------
// Finalize the converter
// -----------------------------------------------------------------------
StatusCode XmlGenericCnv::finalize() {
  // release XmlCnvSvc
  //  m_xmlSvc->release();
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
  StatusCode sc = StatusCode::FAILURE;
   
  // maked sure the address is not null
  if (0 == addr) {
    return StatusCode::FAILURE;
  }

  // parses the xml file or the xml string and retrieves a DOM document
  IOVDOMDocument* document = NULL;
  bool isAString = 1 == addr->ipar()[0];

  // displays the address for debug purposes 
  if ( isAString ) {
    log << MSG::DEBUG << "Address for string: orig. path = " << addr->par()[2]
        << ", ObjectName = " << addr->par()[1] << ", string = ";
    if ( addr->par()[0].size() > 25 ) {
      log << addr->par()[0].substr(0,22) << "...";
    } else {
      log << addr->par()[0];
    }
    log << endmsg;
  } else {
    log << MSG::DEBUG << "Address: filename = " << addr->par()[0]
        << ", ObjectName = " << addr->par()[1] << endmsg;
  }

  if ( 0 == addr->ipar()[0] ) {
    document = xmlSvc()->parse(addr->par()[0].c_str()); // this also lock the cache entry (must be released)
  } else if ( isAString ) {
    document = xmlSvc()->parseString(addr->par()[0].c_str()); // this has to be released too
  } else {
    log << MSG::FATAL << "XmlParser failed, invalid flag isString=" << addr->ipar()[0] << "!" << endreq;
    return StatusCode::FAILURE;
  }

  if (document != NULL) {
    // checks version of the file
    // first find the "DDDB" or "materials" element
    xercesc::DOMNodeList* list = document->getDOM()->getChildNodes();
    xercesc::DOMElement* mainNode = NULL;
    unsigned int index;
    for (index = 0;
         index < list->getLength() && 0 == mainNode;
         ++index) {
      if (list->item(index)->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
        xercesc::DOMNode* childNode = list->item(index);
        xercesc::DOMElement* childElement = (xercesc::DOMElement*) childNode;
        log << MSG::VERBOSE << "found element " << dom2Std (childElement->getNodeName())
            << " at top level of xml file." << endreq;
        if (0 == xercesc::XMLString::compareString(childElement->getNodeName(),DDDBString) ||
            0 == xercesc::XMLString::compareString(childElement->getNodeName(),materialsString)) {
          mainNode = childElement;
        }
      }
    }
    // check if it exists
    if (mainNode != NULL) {
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
      log << MSG::DEBUG << "Detector Description Markup Language Version " << versionAttribute << endreq;
      std::string::size_type dotPos = versionAttribute.find ('.');
      std::string majorVersion;
      std::string minorVersion = "0";
      if (dotPos == std::string::npos) {
        majorVersion = versionAttribute;
      } else {
        majorVersion = versionAttribute.substr (0, dotPos);
        minorVersion = versionAttribute.substr (dotPos + 1);
      }
       
      // I need `defaultMajorVersion'.`defaultMinorVersion'
      if (majorVersion == defaultMajorVersion) { // fine
        if (minorVersion != defaultMinorVersion) { // not perfect, just warn
          log << MSG::WARNING << "DDDB DTD Version " << defaultMajorVersion
              << "." << defaultMinorVersion << " recommanded. "
              << "You are currently using Version " << versionAttribute
              << ". Everything should work fine but you may get some "
              << "error messages about unknown tags."
              << endreq;
        }
        // deals with macro definitions
        // get the parameters
        xercesc::DOMNodeList* macroList = mainNode->getElementsByTagName(macroString);
        unsigned int k;
        for (k = 0; k < macroList->getLength(); k++) {
          xercesc::DOMNode* macroNode = macroList->item(k);
          xercesc::DOMElement* macro = (xercesc::DOMElement*) macroNode;
          std::string name = dom2Std (macro->getAttribute (nameString));
          std::string value = dom2Std (macro->getAttribute (valueString));
          xmlSvc()->addParameter(name, value);
          log << MSG::DEBUG << "Added DDDB Macro " << name << " = " << value << endreq;
        }
        // deals with old parameter definitions
        // get the parameters
        xercesc::DOMNodeList* parameterList = mainNode->getElementsByTagName(parameterString);
        unsigned int kk;
        for (kk = 0; kk < parameterList->getLength(); kk++) {
          xercesc::DOMNode* parameterNode = parameterList->item(kk);
          xercesc::DOMElement* parameter = (xercesc::DOMElement*) parameterNode;
          std::string name = dom2Std (parameter->getAttribute (nameString));
          std::string value = dom2Std (parameter->getAttribute (valueString));
          xmlSvc()->addParameter(name, value);
          log << MSG::DEBUG << "Added DDDB Parameter " << name << " = " << value << endreq;
        }
   
        // retrieve the name of the object we want to create. Removes the leading
        // '/' if needed
        std::string objectName = addr->par()[1];
        std::string::size_type slashPosition = objectName.find_last_of('/');
        if (std::string::npos != slashPosition) {
          objectName= objectName.substr(slashPosition + 1);
        }
  
        // finds the corresponding node in the DOM tree
        XMLCh* nameString = xercesc::XMLString::transcode(objectName.c_str());
        xercesc::DOMElement* element = document->getDOM()->getElementById (nameString);
        xercesc::XMLString::release(&nameString);
        if (element != NULL) {
          try {
            // deal with the node found itself
            sc = internalCreateObj (element, refpObject, addr);
            if ( sc.isSuccess() ){
              IValidity * val = dynamic_cast<IValidity *>(refpObject);
              if ( NULL != val ) { // if the created object implements IValidity, set the IOV from the returned one
                val->setValidity(document->validSince(),document->validTill());
              }
            }
          } catch (GaudiException e) {
            log << MSG::FATAL << "An exception went out of the conversion process : ";
            e.printOut (log);
            log << endmsg;
            sc =  e.code();
          }
        } else { // (element == NULL)
          log << MSG::FATAL << objectName << " : " << "No such object in file " << addr->par()[0] << endreq;
        }
      } else { // (majorVersion != defaultMajorVersion)
        // this is a problem
        log << MSG::ERROR << "DDDB DTD Version " << defaultMajorVersion
            << "." << defaultMinorVersion << " required. "
            << "You are currently using Version " << versionAttribute
            << ". Please update your DTD and XML data files. "
            << "If you are using the XmlDDDB package, please "
            << "get a new version of it."
            << endreq;
      }
    } else { // (mainNode == NULL)
      log << MSG::FATAL << addr->par()[1] << " has no DDDB element at the beginning of the file." << endreq;
    }
    // ---- release the document
    xmlSvc()->releaseDoc(document);
  } else { // (document == NULL)
    log << MSG::FATAL << "XmlParser failed, can't convert " << addr->par()[1] << "!" << endreq;
  }
  return sc;
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
StatusCode XmlGenericCnv::updateObj (IOpaqueAddress* pAddress,
                                     DataObject*     pObject) {
  DataObject* pNewObject; // create a new object and copy it to the old version

  StatusCode sc = createObj(pAddress,pNewObject);
  if (sc.isFailure()){
    MsgStream log( msgSvc(), "XmlGenericCnv" );
    log << MSG::ERROR << "Cannot create the new object to update the existing one" << endmsg;
    return sc;
  }

  // We need to do dynamic_cast because DataObject::operator= is not virtual.
  ValidDataObject* pVDO = dynamic_cast<ValidDataObject*>(pObject);
  ValidDataObject* pNewVDO = dynamic_cast<ValidDataObject*>(pNewObject);
  if ( 0 == pVDO || 0 == pNewVDO ) {
    MsgStream log( msgSvc(), "XmlGenericCnv" );
    log << MSG::ERROR
        << "Cannot update objects other than ValidDataObject: " 
        << "update() must be defined!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  // Deep copy the new Condition into the old DataObject
  pVDO->update( *pNewVDO );  

  // Delete the useless Condition
  delete pNewVDO;

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
  
  // expand environment variables in href
  std::string oldPath=href;
  if (AddressTools::hasEnvironmentVariable(href) ) {
    log << MSG::VERBOSE << "Found environment variable in path "
        << href << endmsg;
    if (!AddressTools::expandAddress(href)) {
      throw XmlCnvException("XmlGenericCnv : unable to resolve path "+oldPath,
                            StatusCode::FAILURE);
    }
    log << MSG::VERBOSE << "path expanded to " << href << std::endl;    
  }
  
  // Is it a CondDB address ?
  bool condDB = m_have_CONDDB_StorageType && (0==href.find("conddb:/"));
  if (condDB) {
    log << MSG::VERBOSE 
        << "Href points to a conddb URL: " << href << endreq;

    // the href should have the format:
    // "conddb:/path/to/folder[:channel_id][#object_name]"
    // the default values are:
    // channel_id = 0
    // object_name = "folder"

    size_t start = href.find_first_of(':')+1;
    size_t columnPos = href.find_first_of(':',start);
    size_t hashPos = href.find_first_of('#',start);
    if (columnPos > hashPos) {
      // ':' is part of the object name
      columnPos = href.npos; // let's pretend that we didn't find it
    }
    size_t pathEnd = href.size();
    
    std::string entryName,path;
    unsigned int channelId = 0;
    
    if ( hashPos != href.npos ) {
      // I do have an object_name
      pathEnd = hashPos;
      entryName = "/" + href.substr(hashPos+1);
    }
    
    if ( columnPos != href.npos ) {
      // I do have a channel id
      pathEnd = columnPos;
      std::istringstream chString(href.substr(columnPos + 1, hashPos - columnPos - 1));
      chString >> channelId;
    }
    
    path = href.substr(start, pathEnd - start);
    
    if ( entryName.empty() ) {
      // this means that I didn't find '#' or the name was not given (ex: "conddb:/path/to/folder#")
      entryName = "/" + path.substr(path.find_last_of('/')+1);
    }

    log << MSG::VERBOSE 
        << "Now build a CondDB address for path=" << path
        << " channelId=" << channelId
        << " and entryName=" << entryName << endreq;
    // Then build a new Address
    return createCondDBAddress (path, entryName, channelId, clid);    
  } else {
    log << MSG::VERBOSE 
        << "Href points to a regular URL: " << href << endreq;
    // here we deal with a regular URL
    // first parse the href to get entryName and location
    std::string::size_type poundPosition = href.find_last_of('#');
    std::string entryName;
    std::string location;
    if ( poundPosition != href.npos ) {
      // builds an entryName
      entryName = "/" + href.substr(poundPosition + 1);
      // builds the location of the file
      location = href.substr(0, poundPosition);
    }
    else { // if there is no '#' we use the file name (/path/to/file.xml -> /path/to/file.xml#file.xml)
      location = href;
      std::string::size_type pos = href.find_last_of('/');
      if ( pos == href.npos ) {
        // no '/' -> href = "something"
        entryName = "/" + href;
      }
      else {
        entryName = "/" + href.substr(pos + 1);
      }
    }
    
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing

      if ( parent->ipar()[0] == 1 ) {

        // If we got here, it means that an XML string contains something like href="#blahblah"
        // This means that the address should point to the "href" of the parent (par[2]) with just the name
        // of the object replaced.
        // In oreder to handle properly the new href, I have to recurse.
        return createAddressForHref (parent->par()[2] + href.substr(poundPosition), clid, parent);

      } else {
        // The address points to a file (usual situation)
        location = parent->par()[0];
      }
    } else {
      // gets the directory where the xmlFile is located
      std::string locDir;
      // I need to prepend the parent path only if the ref is not pointing to an absolute pat
      bool is_abs_path = (
                          ( location[0] == '/' ) // unix absolute path
                          // win absolute path (like C:\...)
                          || (
                              (
                               ( location[0] >= 'a' && location[0] <= 'z' )
                               || ( location[0] >= 'A' && location[0] <= 'Z' )
                               ) && location[1] == ':'
                              )
                          );
      // consider conddb URLs as absolute paths
      bool is_url = location.find("conddb:") == 0;
      if ( (!is_abs_path) && (!is_url) ) {
        if ( parent->ipar()[0] == 0) {
          // The address points to a file
          std::string::size_type dPosU  = parent->par()[0].find_last_of('/'); // search Unix separator
          std::string::size_type dPosW  = parent->par()[0].find_last_of('\\'); // search Win separator
          std::string::size_type dPos   = parent->par()[0].npos; // "not found" marker
          if ( dPosW == dPos ) {  // '\\' not found, assume it is a Unix path
            dPos = dPosU;
          } else if ( dPosU == dPos ) { // '/' not found, assume it is a Windows path
            dPos = dPosW;
          } else { // it seems one of those funny paths with both '\\' and '/', take the latest
            if ( dPosU >= dPosW ) {
              dPos = dPosU;
            } else {
              dPos = dPosW;
            }
          }
          locDir = parent->par()[0].substr( 0, dPos + 1 );
        } else {
          // The address is an xml string
          // this can only be a conddb path, I do not need to check for '\\'
          std::string::size_type dPos  = parent->par()[2].find_last_of('/');
          locDir = parent->par()[2].substr( 0, dPos + 1 );
        }
        location = locDir + location;
      }
    }

    // Handle the relative path problem of "../".
    // the idea is that each time the pattern "../" is found, the
    // pattern and the parent directory are removed from the final location.
    // The operation is repeated until no pattern is found.

    // I'm including the leading '/' of "../" to ease the search
    // for the start of the parent directory name.
    std::string  upwardString    = std::string("/../");
    std::string::size_type upwardStringPos = location.find(upwardString);

    while (location.size() > upwardStringPos){
      std::string::size_type parentDirPos = location.find_last_of('/', upwardStringPos - 1);
      std::string::size_type cutLength    = (upwardStringPos + upwardString.size()) - (parentDirPos + 1);
      location = location.erase(parentDirPos + 1, cutLength);
      upwardStringPos = location.find(upwardString);
    }
    // Now, location should be a clean URL or absolute path.
    if ( parent->ipar()[0] == 1  // the URL comes from a string
         && location[0] != '/' ) // avoid infinite loops
      return createAddressForHref(location + "#" + entryName.substr(1), clid, parent);

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

  MsgStream log( msgSvc(), "XmlGenericCnv" );
  
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
                                                    unsigned long channelId,
                                                    CLID clid) const {
  const std::string par[2] = {path, entryName};
  IOpaqueAddress* result;
  StatusCode sc = addressCreator()->createAddress (CONDDB_StorageType,
                                                   clid,
                                                   par,
                                                   &channelId,
                                                   result);
  if (!sc.isSuccess()) {
    throw XmlCnvException ("XmlGenericCnv : Unable to create Address from href",
                           sc);
  }
  MsgStream log( msgSvc(), "XmlGenericCnv" );
  log << MSG::DEBUG << "New address created : path = " << path
      << ", channel id = " << channelId
      << ", entry name = " << entryName << endreq;
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
    xercesc::XMLString::release(&cString);
  }
  return stdString;
}
 
