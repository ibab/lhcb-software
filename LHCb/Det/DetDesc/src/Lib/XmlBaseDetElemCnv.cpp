//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlBaseDetElemCnv.cpp,v 1.13 2001-12-13 19:17:39 andreav Exp $

// include files
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/XmlCnvException.h"
#include "DetDesc/XmlBaseDetElemCnv.h"
#include "DetDesc/IXmlSvc.h"

class ISvcLocator;

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseDetElemCnv::classID()) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc,
                                      const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::initialize() {
  StatusCode sc = XmlGenericCnv::initialize();
  if (sc.isSuccess()) {
    MsgStream log (msgSvc(), "XmlDetElemCnv");
    log << MSG::VERBOSE << "Initializing" << endreq;
    if (0 != m_xmlSvc) {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      log << MSG::INFO << "Generic conversion status: "
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
  // initializes it
  dataObj->initialize();
  // returns
  log << MSG::VERBOSE << "Method fillObjRefs() ending" << endreq;
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_createObj (DOM_Element element,
                                           DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  
  // creates an object for the node found
  log << MSG::DEBUG << "Normal generic detector element conversion" << endreq;
  std::string elementName = dom2Std (element.getAttribute("name"));
  refpObject = new DetectorElement (elementName);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_fillObj (DOM_Element childElement,
                                         DataObject* refpObject,
                                         IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );

  // gets the object
  DetectorElement* dataObj = dynamic_cast<DetectorElement*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if (("detelemref" == tagName) || ("detelem" == tagName)) {
    IOpaqueAddress* addr;
    if ("detelemref" == tagName) {
      // gets the reference value and the position of the '#' in it
      std::string referenceValue = dom2Std (childElement.getAttribute("href"));
      // creates the address
      addr = createAddressForHref
        (referenceValue, CLID_DetectorElement, address);
    } else { // here "detelem" == tagName
      std::string entryName = "/" + dom2Std (childElement.getAttribute("name"));
      std::string location = address->par()[0];
      addr = createXmlAddress (location, entryName, CLID_DetectorElement);
    }
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
  } else if ("version" == tagName || "author" == tagName) {
    // currently ignored
  } else if ("geometryinfo" == tagName) {
    // Everything is in the attributes
    std::string logVolName = dom2Std (childElement.getAttribute ("lvname"));
    std::string support = dom2Std (childElement.getAttribute ("support"));
    std::string replicaPath = dom2Std (childElement.getAttribute ("rpath"));
    std::string namePath = dom2Std (childElement.getAttribute ("npath"));
    log << MSG::DEBUG << "GI volume : " << logVolName  << endreq;
    log << MSG::DEBUG << "GI support: " << support     << endreq;
    log << MSG::DEBUG << "GI rpath  : " << replicaPath << endreq;
    log << MSG::DEBUG << "GI npath  : " << namePath    << endreq;

    // creates a geometryInfo child
    if (logVolName.empty()) {
      dataObj->createGeometryInfo();
    } else if (support.empty()) {
      dataObj->createGeometryInfo (logVolName);
    } else if (!namePath.empty()) {
      dataObj->createGeometryInfo (logVolName, support, namePath);
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
            log << MSG::DEBUG << "Found replica number "
                << repPath.back() << endreq;
            replica = buf;
            i = 0;
          }
        }
        rp++;
      } while (*rp != 0);
      dataObj->createGeometryInfo (logVolName,support,repPath);
    } else {
      log << MSG::ERROR << "File " << address->par()[0] << ": "
          << tagName
          << " Missing \"rpath\" or \"npath\" element, "
          << "please correct XML data\n"
          << " Either remove support element or provide proper rpath or npath"
          << endreq;
      StatusCode st( CORRUPTED_DATA );
      throw XmlCnvException( " Corrupted XML data", st );            
    }
  } else if ("alignmentinfo" == tagName) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement.getAttribute ("condition"));
    log << MSG::DEBUG 
	<< "Create AlignmentInfo with condition : " << condition  << endreq;
    dataObj->createAlignment(condition);
  } else if ("calibrationinfo" == tagName) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement.getAttribute ("condition"));
    log << MSG::DEBUG 
	<< "Create CalibrationInfo with condition : " << condition  << endreq;
    dataObj->createCalibration(condition);
  } else if ("readoutinfo" == tagName) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement.getAttribute ("condition"));
    log << MSG::DEBUG 
	<< "Create ReadOutInfo with condition : " << condition  << endreq;
    dataObj->createReadOut(condition);
  } else if ("slowcontrolinfo" == tagName) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement.getAttribute ("condition"));
    log << MSG::DEBUG 
	<< "Create SlowControlInfo with condition : " << condition  << endreq;
    dataObj->createSlowControl(condition);
  } else if ("fastcontrolinfo" == tagName) {
    // Everything is in the attributes
    std::string condition = dom2Std (childElement.getAttribute ("condition"));
    log << MSG::DEBUG 
	<< "Create FastControlInfo with condition : " << condition  << endreq;
    dataObj->createFastControl(condition);
  } else if (tagName == "specific") {
    // this is the place where the user will put new elements he wants
    // to add to the default detector element
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    DOM_NodeList specificChildren = childElement.getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren.getLength(); i++) {
      if (specificChildren.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
        // gets the current child
        DOM_Node childNode = specificChildren.item(i);
        StatusCode sc = i_fillSpecificObj ((DOM_Element &) childNode,
                                           dataObj);
        if (sc.isFailure()) {
          std::string childNodeName =
            dom2Std (((DOM_Element &) childNode).getNodeName());
          log << MSG::WARNING << "parsing of specific child "
              << childNodeName << " raised errors."
              << endreq;
        }
      }
    }
  } else if ("param" == tagName || "paramVector" == tagName ||
             "userParameter" == tagName || "userParameterVector" == tagName) {
    // get the attributes
    std::string type = dom2Std (childElement.getAttribute ("type"));
    std::string name = dom2Std (childElement.getAttribute ("name"));
    std::string comment = dom2Std (childElement.getAttribute ("comment"));
    
    // gets the value
    std::string value;
    DOM_NodeList nodeChildren = childElement.getChildNodes();
    unsigned int i;
    for (i = 0; i < nodeChildren.getLength(); i++) {
      if (nodeChildren.item(i).getNodeType() == DOM_Node::TEXT_NODE) {
        std::string newVal = dom2Std (nodeChildren.item(i).getNodeValue());
        unsigned int begin = 0;
        while (begin < newVal.length() && isspace(newVal[begin])) {
          begin++;
        }
        if (begin < newVal.length()) {
          unsigned int end = newVal.length() - 1;
          while (isspace(newVal[end])) {
            end--;
          }
          value += " ";
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if ("userParameter" == tagName || "param" == tagName) {
      // adds the new parameter to the detectorElement
      log << MSG::DEBUG << "Adding user parameter " << name << " with value "
          << value << ", type " << type << " and comment \"" << comment
          << "\"" << endreq;
      if (type == "int") {
        double dd = xmlSvc()->eval(value, false);
        dataObj->addUserParameter (name,
                                   type,
                                   comment,
                                   value,
                                   dd,
                                   int (dd));
      } else if(type == "double") {
        dataObj->addUserParameter (name,
                                   type,
                                   comment,
                                   value,
                                   xmlSvc()->eval(value, false));
      } else {
        dataObj->addUserParameter (name,
                                   type,
                                   comment,
                                   value);
      }
    } else if ("userParameterVector" == tagName || "paramVector" == tagName) {
      // parses the value
      std::vector<std::string> vect;
      const char *textValue = value.c_str();
      std::string val;
      std::istrstream cstr (textValue, value.length());
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
        double dd = xmlSvc()->eval(*it, false);
        if ("int" == type) {
          i_vect.push_back (dd);
          d_vect.push_back (dd);
        } else if ("double" == type) {
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
        dataObj->addUserParameterVector
          (name, type, comment, vect, d_vect, i_vect);
      } else if ("double" == type) {
        dataObj->addUserParameterVector (name, type, comment, vect, d_vect);
      } else {
        dataObj->addUserParameterVector (name, type, comment, vect);
      }
    }
    
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagName << endreq;
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

