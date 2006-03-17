// $Id: XmlBaseDetElemCnv.cpp,v 1.14 2006-03-17 10:02:07 marcocle Exp $

// include files

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"

#include <xercesc/dom/DOMNodeList.hpp>

#include "DetDesc/DetectorElement.h"
#include "DetDescCnv/XmlCnvException.h"
#include "DetDescCnv/XmlBaseDetElemCnv.h"
#include "XmlTools/IXmlSvc.h"

#include <cstdlib>
#include <iostream>
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream> 
#else
#include <sstream>
#endif
#include <string>
#include <vector>
#include <cctype>

class ISvcLocator;

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseDetElemCnv::classID()) {
  m_doGenericCnv = false;
  initStrings();
}


// -----------------------------------------------------------------------
// initStrings
// ------------------------------------------------------------------------
void XmlBaseDetElemCnv::initStrings() {
  specificString = xercesc::XMLString::transcode("specific");
  paramString = xercesc::XMLString::transcode("param");
  paramVectorString = xercesc::XMLString::transcode("paramVector");
  detelemString = xercesc::XMLString::transcode("detelem");
  detelemrefString = xercesc::XMLString::transcode("detelemref");
  versionString = xercesc::XMLString::transcode("version");
  authorString = xercesc::XMLString::transcode("author");
  geometryinfoString = xercesc::XMLString::transcode("geometryinfo");
  alignmentinfoString = xercesc::XMLString::transcode("alignmentinfo");
  calibrationinfoString = xercesc::XMLString::transcode("calibrationinfo");
  readoutinfoString = xercesc::XMLString::transcode("readoutinfo");
  slowcontrolinfoString = xercesc::XMLString::transcode("slowcontrolinfo");
  fastcontrolinfoString = xercesc::XMLString::transcode("fastcontrolinfo");
  userParameterString = xercesc::XMLString::transcode("userParameter");
  userParameterVectorString =
    xercesc::XMLString::transcode("userParameterVector");
  conditioninfoString = xercesc::XMLString::transcode("conditioninfo");

  typeString = xercesc::XMLString::transcode("type");
  nameString = xercesc::XMLString::transcode("name");
  commentString = xercesc::XMLString::transcode("comment");
  hrefString = xercesc::XMLString::transcode("href");
  lvnameString = xercesc::XMLString::transcode("lvname");
  supportString = xercesc::XMLString::transcode("support");
  rpathString = xercesc::XMLString::transcode("rpath");
  npathString = xercesc::XMLString::transcode("npath");
  conditionString = xercesc::XMLString::transcode("condition");
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc,
                                      const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
  initStrings();
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::~XmlBaseDetElemCnv () {
  xercesc::XMLString::release((XMLCh**)&specificString);
  xercesc::XMLString::release((XMLCh**)&detelemString);
  xercesc::XMLString::release((XMLCh**)&detelemrefString);
  xercesc::XMLString::release((XMLCh**)&versionString);
  xercesc::XMLString::release((XMLCh**)&authorString);
  xercesc::XMLString::release((XMLCh**)&geometryinfoString);
  xercesc::XMLString::release((XMLCh**)&alignmentinfoString);
  xercesc::XMLString::release((XMLCh**)&calibrationinfoString);
  xercesc::XMLString::release((XMLCh**)&readoutinfoString);
  xercesc::XMLString::release((XMLCh**)&slowcontrolinfoString);
  xercesc::XMLString::release((XMLCh**)&fastcontrolinfoString);
  xercesc::XMLString::release((XMLCh**)&paramString);
  xercesc::XMLString::release((XMLCh**)&paramVectorString);
  xercesc::XMLString::release((XMLCh**)&userParameterString);
  xercesc::XMLString::release((XMLCh**)&userParameterVectorString);
  xercesc::XMLString::release((XMLCh**)&conditioninfoString);

  xercesc::XMLString::release((XMLCh**)&typeString);
  xercesc::XMLString::release((XMLCh**)&nameString);
  xercesc::XMLString::release((XMLCh**)&commentString);
  xercesc::XMLString::release((XMLCh**)&hrefString);
  xercesc::XMLString::release((XMLCh**)&lvnameString);
  xercesc::XMLString::release((XMLCh**)&supportString);
  xercesc::XMLString::release((XMLCh**)&rpathString);
  xercesc::XMLString::release((XMLCh**)&npathString);
  xercesc::XMLString::release((XMLCh**)&conditionString);
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::initialize() {
  StatusCode sc = XmlGenericCnv::initialize();
  if (sc.isSuccess()) {
    MsgStream log (msgSvc(), "XmlBaseDetElemCnv");
    log << MSG::VERBOSE << "Initializing converter for class ID " << classID() << endreq;
    if (0 != m_xmlSvc) {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      log << MSG::VERBOSE << "Generic conversion status: "
          << (unsigned int)m_doGenericCnv << endreq;
    }
  }
  return sc;
}


// -----------------------------------------------------------------------
// Resolve the references of the created transient object.
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::fillObjRefs (IOpaqueAddress* childElement,
                                           DataObject* refpObject) {
  MsgStream log( msgSvc(), "XmlBaseDetElemCnv" );
  log << MSG::VERBOSE << "Method fillObjRefs() starting" << endreq;
  // processes the base class
  StatusCode sc = XmlGenericCnv::fillObjRefs ( childElement, refpObject );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not fill object references" << endreq;
    return sc;
  }
  // gets the object
  DetectorElement* dataObj = dynamic_cast<DetectorElement*> (refpObject);
  if (!dataObj) {
    std::ostringstream msg;
    msg << "DataObject at " << refpObject->registry()->identifier() << " is not a DetectorElement!!!";
    log << MSG::ERROR << msg.str() << endmsg;
    throw GaudiException(msg.str(),
                         "XmlBaseDetElemCnv::fillObjRefs",
                         StatusCode::FAILURE);
  }
  // initializes it
  sc = dataObj->initialize();
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "DetectorElement " << dataObj->name()
        << ": initialization failed." << endreq;
    return sc;
  }
  // returns
  log << MSG::VERBOSE << "Method fillObjRefs() ending" << endreq;
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_createObj (xercesc::DOMElement* element,
                                           DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlBaseDetElemCnv" );
  
  // creates an object for the node found
  log << MSG::VERBOSE << "Normal generic detector element conversion" << endreq;
  std::string elementName = dom2Std (element->getAttribute(nameString));
  refpObject = new DetectorElement (elementName);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_fillObj (xercesc::DOMElement* childElement,
                                         DataObject* refpObject,
                                         IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlBaseDetElemCnv" );

  // gets the object
  DetectorElement* dataObj = dynamic_cast<DetectorElement*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  // dispatches, based on the name
  if ((0 == xercesc::XMLString::compareString(detelemrefString, tagName)) ||
      (0 == xercesc::XMLString::compareString(detelemString, tagName))) {
    IOpaqueAddress* addr;

    // in order to handle properly both XML files and CondDB XML strings
    // we use always the "createAddressForHref" system
    std::string referenceValue;
    if (0 == xercesc::XMLString::compareString(detelemrefString, tagName)) {
      // that's a real detelemref entry
      referenceValue = dom2Std (childElement->getAttribute (hrefString));
    } else {
      // it is an element: build a (fake) reference
      referenceValue = std::string("#") + dom2Std (childElement->getAttribute (nameString));
    }
    // creates the address
    addr = createAddressForHref (referenceValue, CLID_DetectorElement, address);

    // Registers the address to current data object we're converting now
    IDataProviderSvc* dsvc = dataProvider();
    IDataManagerSvc* mgr = 0;
    StatusCode sc = dsvc->queryInterface(IID_IDataManagerSvc,(void**)&mgr);
    if (sc.isSuccess()) {
      sc = mgr->registerAddress(address->registry(), addr->par()[1], addr);
      mgr->release();        
    }
    if (!sc.isSuccess()) {
      throw GaudiException ("Unable to register new Address",
                            "XmlBaseDetElemCnv",
                            sc);
    }
  } else if (0 == xercesc::XMLString::compareString(versionString, tagName) ||
             0 == xercesc::XMLString::compareString(authorString, tagName)) {
    // currently ignored
  } else if (0 == xercesc::XMLString::compareString
             (geometryinfoString, tagName)) {
    // Everything is in the attributes
    std::string logVolName =
      dom2Std (childElement->getAttribute (lvnameString));
    std::string conditionPath = 
      dom2Std (childElement->getAttribute (conditionString));
    std::string support =
      dom2Std (childElement->getAttribute (supportString));
    std::string replicaPath =
      dom2Std (childElement->getAttribute (rpathString));
    std::string namePath =
      dom2Std (childElement->getAttribute (npathString));
    log << MSG::VERBOSE << std::endl
        << "GI volume        : " << logVolName    << std::endl
        << "GI support       : " << support       << std::endl
        << "GI rpath         : " << replicaPath   << std::endl
        << "GI npath         : " << namePath      << std::endl
        << "GI conditionPath : " << conditionPath << endmsg;

    // creates a geometryInfo child
    if (logVolName.empty()) {
      dataObj->createGeometryInfo();
    } else if (support.empty()) {
      dataObj->createGeometryInfo (logVolName);
    } else if (!namePath.empty()) {
      dataObj->createGeometryInfo (logVolName, support, 
                                   namePath, conditionPath);
    } else if (!replicaPath.empty()) {
      ILVolume::ReplicaPath repPath;            
      // Replica path has the format "1/3/7/2"
      const char *rp = replicaPath.c_str();
      char buf[512];
      char *replica = buf;
      bool wasColon = false;
      unsigned int i = 0;
      do {
        wasColon = false;
        if (*rp == '/') {
          wasColon = true;
        } else if (isdigit(*rp)) {
          *replica = *rp;
          replica++;
          i++;
        }
        if (true == wasColon || *(rp + 1) == '\0') {
          if (i > 0) {
            *replica = '\0';
            i = (unsigned int)atol (buf);
            repPath.push_back (i);
            log << MSG::VERBOSE << "Found replica number "
                << repPath.back() << endreq;
            replica = buf;
            i = 0;
          }
        }
        rp++;
      } while (*rp != 0);
      dataObj->createGeometryInfo (logVolName,support,repPath, conditionPath);
    } else {
      char* tagNameString = xercesc::XMLString::transcode(tagName);
      log << MSG::ERROR << "File " << address->par()[0] << ": "
          << tagNameString
          << " Missing \"rpath\" or \"npath\" element, "
          << "please correct XML data\n"
          << " Either remove support element or provide proper rpath or npath"
          << endreq;
      xercesc::XMLString::release(&tagNameString);
      StatusCode st( CORRUPTED_DATA );
      throw XmlCnvException( " Corrupted XML data", st );            
    }
  } else if (0 == xercesc::XMLString::compareString
             (alignmentinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition =
      dom2Std (childElement->getAttribute (conditionString));
    log << MSG::DEBUG 
        << "Create AlignmentInfo with condition : " << condition  << endreq;
    dataObj->createAlignment(condition);
  } else if (0 == xercesc::XMLString::compareString
             (calibrationinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition =
      dom2Std (childElement->getAttribute (conditionString));
    log << MSG::DEBUG 
        << "Create CalibrationInfo with condition : " << condition  << endreq;
    dataObj->createCalibration(condition);
  } else if (0 == xercesc::XMLString::compareString
             (readoutinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition =
      dom2Std (childElement->getAttribute (conditionString));
    log << MSG::DEBUG 
        << "Create ReadOutInfo with condition : " << condition  << endreq;
    dataObj->createReadOut(condition);
  } else if (0 == xercesc::XMLString::compareString
             (slowcontrolinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition =
      dom2Std (childElement->getAttribute (conditionString));
    log << MSG::DEBUG 
        << "Create SlowControlInfo with condition : " << condition  << endreq;
    dataObj->createSlowControl(condition);
  } else if (0 == xercesc::XMLString::compareString
             (fastcontrolinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition =
      dom2Std (childElement->getAttribute (conditionString));
    log << MSG::DEBUG 
        << "Create FastControlInfo with condition : " << condition  << endreq;
    dataObj->createFastControl(condition);
  } else if (0 == xercesc::XMLString::compareString(tagName, specificString)) {
    // this is the place where the user will put new elements he wants
    // to add to the default detector element
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    xercesc::DOMNodeList* specificChildren = childElement->getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren->getLength(); i++) {
      if (specificChildren->item(i)->getNodeType() ==
          xercesc::DOMNode::ELEMENT_NODE) {
        // gets the current child
        xercesc::DOMNode* childNode = specificChildren->item(i);
        StatusCode sc = i_fillSpecificObj ((xercesc::DOMElement *) childNode,
                                           dataObj,
                                           address);
        if (sc.isFailure()) {
          std::string childNodeName =
            dom2Std (((xercesc::DOMElement *) childNode)->getNodeName());
          log << MSG::WARNING << "parsing of specific child "
              << childNodeName << " raised errors."
              << endreq;
        }
      }
    }
  } else if
    (0 == xercesc::XMLString::compareString(paramString, tagName) ||
     0 == xercesc::XMLString::compareString(paramVectorString, tagName) ||
     0 == xercesc::XMLString::compareString(userParameterString, tagName) ||
     0 == xercesc::XMLString::compareString
     (userParameterVectorString, tagName)) {
    
    bool isSingleParam =
      (0 == xercesc::XMLString::compareString(userParameterString, tagName) ||
       0 == xercesc::XMLString::compareString(paramString, tagName));

    // get the attributes
    std::string type = dom2Std (childElement->getAttribute (typeString));
    std::string name = dom2Std (childElement->getAttribute (nameString));
    std::string comment = dom2Std (childElement->getAttribute (commentString));
    
    // gets the value
    std::string value;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
    bool firstWord = true;
    for (i = 0; i < nodeChildren->getLength(); i++) {
      if (nodeChildren->item(i)->getNodeType() == xercesc::DOMNode::TEXT_NODE) {
        std::string newVal = dom2Std (nodeChildren->item(i)->getNodeValue());
        unsigned int begin = 0;
        while (begin < newVal.length() && isspace(newVal[begin])) {
          begin++;
        }
        if (begin < newVal.length()) {
          unsigned int end = newVal.length() - 1;
          while (isspace(newVal[end])) {
            end--;
          }
          if (firstWord) {
            firstWord = false;
          } else {
            value += " ";
          }
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if (isSingleParam) {
      // adds the new parameter to the detectorElement
      if (type == "int") {
        dataObj->addParam<int>(name,(int)xmlSvc()->eval(value, false),comment);
      } else if(type == "double") {
        dataObj->addParam<double>(name,xmlSvc()->eval(value, false),comment);
      } else {
        // check whether there are "" around the string. If yes, remove them
        if ((value.length() > 1) &&
            (value[0] == '"' && value[value.length()-1] == '"')) {
          value = value.substr(1, value.length()-2);
        }
        dataObj->addParam<std::string>(name,value,comment);
      }
      log << MSG::VERBOSE << "Added user parameter " << name << " with value "
          << value << ", type " << type << " and comment \"" << comment
          << "\"" << endreq;
    } else {
      // we have a vector of parameters here
      // parses the value
      std::vector<std::string> vect;
#if defined (__GNUC__) && ( __GNUC__ <= 2 )
      const char *textValue = value.c_str();
      std::istrstream cstr (textValue, value.length());
#else
      std::istringstream cstr (value);
#endif
      std::string val;
      while (cstr >> val) {
        vect.push_back (val);
      }

      // depending on the type, evaluates the value
      std::vector<double> d_vect;
      std::vector<int> i_vect;
      std::vector<std::string>::iterator it;
      for (it = vect.begin();
           vect.end() != it;
           ++it) {
        if ("int" == type) {
          double dd = xmlSvc()->eval(*it, false);
          i_vect.push_back ((int)dd);
        } else if ("double" == type) {
          double dd = xmlSvc()->eval(*it, false);
          d_vect.push_back (dd);
        }
      }
      // adds the new parameterVector to the detectorElement
      log << MSG::DEBUG << "Adding user parameter vector " << name
          << " with values ";
      std::vector<std::string>::iterator it2;
      for (it2 = vect.begin();
           vect.end() != it2;
           ++it2) {
        log << *it2 << " ";
      }
      log << ", type " << type << " and comment \""
          << comment << "\"." << endreq;
      if ("int" == type) {
        dataObj->addParam(name,i_vect,comment);
      } else if ("double" == type) {
        dataObj->addParam(name,d_vect,comment);
      } else {
        dataObj->addParam(name,vect,comment);
      }
    }

  } else if (0 == xercesc::XMLString::compareString(conditioninfoString, tagName)) {

    std::string name = dom2Std(childElement->getAttribute(nameString));
    std::string path = dom2Std(childElement->getAttribute(conditionString));
    log << MSG::DEBUG << "Create SmartRef<Condition> : \"" << name << "\" -> " << path  << endreq;
    dataObj->createCondition(name,path);

  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagNameString << endreq;
    xercesc::XMLString::release(&tagNameString);
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Accessor to the classID
// -----------------------------------------------------------------------
const CLID& XmlBaseDetElemCnv::classID() {
  return CLID_DetectorElement;
}

