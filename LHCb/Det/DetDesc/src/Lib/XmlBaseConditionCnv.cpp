//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlBaseConditionCnv.cpp,v 1.4 2002-01-22 09:17:54 sponce Exp $

// include files
#include <string>
#include <vector>

#include <dom/DOM_NodeList.hpp>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "DetDesc/Condition.h"
#include "DetDesc/XmlBaseConditionCnv.h"
#include "DetDesc/IXmlSvc.h"

class ISvcLocator;

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseConditionCnv::classID()) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc,
                                          const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::initialize() {
  StatusCode sc = XmlGenericCnv::initialize();
  if (sc.isSuccess()) {
    MsgStream log (msgSvc(), "XmlBaseConditionCnv");
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
StatusCode XmlBaseConditionCnv::fillObjRefs (IOpaqueAddress* /*childElement*/,
                                             DataObject* refpObject) {
  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);
  // initializes it
  dataObj->initialize();
  // returns
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_createObj (DOM_Element /*element*/,
                                             DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );
  
  // creates an object for the node found
  log << MSG::DEBUG << "Normal generic condition conversion" << endreq;
  // std::string elementName = dom2Std (element.getAttribute("name"));
  // Since the name is never used afterwars, we just don't pass it to
  // the condition object
  refpObject = new Condition (/*elementName*/);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_fillObj (DOM_Element childElement,
                                           DataObject* refpObject,
                                           IOpaqueAddress* /*address*/) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );

  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if (tagName == "specific") {
    // this is the place where the user will put new elements he wants
    // to add to the default condition
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    DOM_NodeList specificChildren = childElement.getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren.getLength(); i++) {
      if (specificChildren.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
        // gets the current child
        DOM_Node childNode = specificChildren.item(i);
        StatusCode sc = i_fillSpecificObj ((DOM_Element &) childNode,
                                           dataObj,
                                           address);
        if (sc.isFailure()) {
          std::string childNodeName =
            dom2Std (((DOM_Element &) childNode).getNodeName());
          log << MSG::WARNING << "parsing of specific child "
              << childNodeName << " raised errors."
              << endreq;
        }
      }
    }
  } else if ("param" == tagName || "paramVector" == tagName) {
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
          if (i>0) value += " ";
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if ("param" == tagName) {
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
    } else if ("paramVector" == tagName) {
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
const CLID& XmlBaseConditionCnv::classID() {
  return CLID_Condition;
}

