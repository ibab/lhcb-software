// include files

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartIF.h"

#include <xercesc/dom/DOMNodeList.hpp>

#include "DetDesc/DetectorElement.h"
#include "DetDescCnv/XmlCnvException.h"
#include "DetDescCnv/XmlBaseDetElemCnv.h"
#include "XmlTools/IXmlSvc.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
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
    if( msgLevel(MSG::VERBOSE) )
      verbose() << "Initializing converter for class ID " << classID() << endmsg;
    if (0 != m_xmlSvc) {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      if( msgLevel(MSG::VERBOSE) )
        verbose() << "Generic conversion status: "
                  << (unsigned int)m_doGenericCnv << endmsg;
    }
  }
  return sc;
}


// -----------------------------------------------------------------------
// Resolve the references of the created transient object.
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::fillObjRefs (IOpaqueAddress* childElement,
                                           DataObject* refpObject) {
  if( msgLevel(MSG::VERBOSE) )
    verbose() << "Method fillObjRefs() starting" << endmsg;
  // processes the base class
  StatusCode sc = XmlGenericCnv::fillObjRefs ( childElement, refpObject );
  if ( !sc.isSuccess() ) {
    error() << "Could not fill object references" << endmsg;
    return sc;
  }
  // gets the object
  DetectorElement* dataObj = dynamic_cast<DetectorElement*> (refpObject);
  if (!dataObj) {
    std::ostringstream msg;
    msg << "DataObject at " << refpObject->registry()->identifier() << " is not a DetectorElement!!!";
    error() << msg.str() << endmsg;
    throw GaudiException(msg.str(),
                         "XmlBaseDetElemCnv::fillObjRefs",
                         StatusCode::FAILURE);
  }
  // initializes it
  sc = dataObj->initialize();
  if (!sc.isSuccess()) {
    error() << "DetectorElement " << dataObj->name()
            << ": initialization failed." << endmsg;
    return sc;
  }
  // returns
  if( msgLevel(MSG::VERBOSE) )
    verbose() << "Method fillObjRefs() ending" << endmsg;
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_createObj (xercesc::DOMElement* element,
                                           DataObject*& refpObject) {
  // creates an object for the node found
  if( msgLevel(MSG::VERBOSE) )
    verbose() << "Normal generic detector element conversion" << endmsg;
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
    StatusCode sc = StatusCode::FAILURE;
    SmartIF<IDataManagerSvc> mgr( dataProvider() );
    if (mgr) {
      sc = mgr->registerAddress(address->registry(), addr->par()[1], addr);
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
    if( msgLevel(MSG::VERBOSE) )
      verbose() << "GI volume        : " << logVolName    << endmsg
                << "GI support       : " << support       << endmsg
                << "GI rpath         : " << replicaPath   << endmsg
                << "GI npath         : " << namePath      << endmsg
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
            if( msgLevel(MSG::VERBOSE) )
              verbose() << "Found replica number " << repPath.back() << endmsg;
            replica = buf;
            i = 0;
          }
        }
        rp++;
      } while (*rp != 0);
      dataObj->createGeometryInfo (logVolName,support,repPath, conditionPath);
    } else {
      char* tagNameString = xercesc::XMLString::transcode(tagName);
      error() << "File " << address->par()[0] << ": "
              << tagNameString
              << " Missing \"rpath\" or \"npath\" element, "
              << "please correct XML data\n"
              << " Either remove support element or provide proper rpath or npath"
              << endmsg;
      xercesc::XMLString::release(&tagNameString);
      StatusCode st( CORRUPTED_DATA );
      throw XmlCnvException( " Corrupted XML data", st );            
    }
  } else if (0 == xercesc::XMLString::compareString
             (alignmentinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement->getAttribute (conditionString));
    if( msgLevel(MSG::DEBUG) )
      debug()  << "Create AlignmentInfo with condition : " << condition  << endmsg;
    dataObj->createAlignment(condition);
  } else if (0 == xercesc::XMLString::compareString
             (calibrationinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement->getAttribute (conditionString));
    if( msgLevel(MSG::DEBUG) )
      debug() << "Create CalibrationInfo with condition : " << condition  << endmsg;
    dataObj->createCalibration(condition);
  } else if (0 == xercesc::XMLString::compareString
             (readoutinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement->getAttribute (conditionString));
    if( msgLevel(MSG::DEBUG) )
      debug() << "Create ReadOutInfo with condition : " << condition  << endmsg;
    dataObj->createReadOut(condition);
  } else if (0 == xercesc::XMLString::compareString
             (slowcontrolinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement->getAttribute (conditionString));
    if( msgLevel(MSG::DEBUG) )
      debug() << "Create SlowControlInfo with condition : " << condition  << endmsg;
    dataObj->createSlowControl(condition);
  } else if (0 == xercesc::XMLString::compareString
             (fastcontrolinfoString, tagName)) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement->getAttribute (conditionString));
    if( msgLevel(MSG::DEBUG) )
      debug() << "Create FastControlInfo with condition : " << condition  << endmsg;
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
          warning() << "parsing of specific child "
                    << childNodeName << " raised errors."
                    << endmsg;
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
      if( msgLevel(MSG::VERBOSE) )
        verbose() << "Added user parameter " << name << " with value "
                  << value << ", type " << type << " and comment \"" << comment
                  << "\"" << endmsg;
    } else {
      // we have a vector of parameters here
      // parses the value
      std::vector<std::string> vect;
      std::istringstream cstr (value);
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
      if( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Adding user parameter vector " << name
                  << " with values ";
        std::vector<std::string>::iterator it2;
        for (it2 = vect.begin();
             vect.end() != it2;
             ++it2) {
          verbose() << *it2 << " ";
        }
        verbose() << ", type " << type << " and comment \""
                  << comment << "\"." << endmsg;
      }
      
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
    if( msgLevel(MSG::VERBOSE) )
      verbose() << "Create SmartRef<Condition> : \"" << name << "\" -> " << path  << endmsg;
    dataObj->createCondition(name,path);

  } else {
    // Something goes wrong, does it?
    char* tagNameString = xercesc::XMLString::transcode(tagName);
    warning() << "This tag makes no sense to element : "
              << tagNameString << endmsg;
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

