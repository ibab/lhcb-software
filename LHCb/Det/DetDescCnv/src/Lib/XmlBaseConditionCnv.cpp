// $Id: XmlBaseConditionCnv.cpp,v 1.3 2003-04-25 08:53:08 sponce Exp $

// include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/MsgStream.h"

#include <xercesc/dom/DOMNodeList.hpp>

#include "DetDesc/Condition.h"
#include "DetDescCnv/XmlBaseConditionCnv.h"
#include "XmlTools/IXmlSvc.h"

#if defined (__GNUC__) && ( __GNUC__ <= 2 )
#include <strstream> 
#else
#include <sstream>
#endif
#include <string>
#include <vector>

class ISvcLocator;

// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseConditionCnv::classID()) {
  m_doGenericCnv = false;
  specificString = xercesc::XMLString::transcode("specific");
  paramString = xercesc::XMLString::transcode("param");
  paramVectorString = xercesc::XMLString::transcode("paramVector");
  typeString = xercesc::XMLString::transcode("type");
  nameString = xercesc::XMLString::transcode("name");
  commentString = xercesc::XMLString::transcode("comment");
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::XmlBaseConditionCnv (ISvcLocator* svc,
                                          const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
  specificString = xercesc::XMLString::transcode("specific");
  paramString = xercesc::XMLString::transcode("param");
  paramVectorString = xercesc::XMLString::transcode("paramVector");
  typeString = xercesc::XMLString::transcode("type");
  nameString = xercesc::XMLString::transcode("name");
  commentString = xercesc::XMLString::transcode("comment");
}


// -----------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
XmlBaseConditionCnv::~XmlBaseConditionCnv () {
  xercesc::XMLString::release(&(XMLCh*)specificString);
  xercesc::XMLString::release(&(XMLCh*)paramString);
  xercesc::XMLString::release(&(XMLCh*)paramVectorString);
  xercesc::XMLString::release(&(XMLCh*)typeString);
  xercesc::XMLString::release(&(XMLCh*)nameString);
  xercesc::XMLString::release(&(XMLCh*)commentString);
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
      log << MSG::DEBUG << "Generic conversion status: "
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
StatusCode XmlBaseConditionCnv::i_createObj (xercesc::DOMElement* /*element*/,
                                             DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );
  
  // creates an object for the node found
  log << MSG::DEBUG << "Normal generic condition conversion" << endreq;
  // const XMLCh* elementName = element->getAttribute(nameString);
  // Since the name is never used afterwars, we just don't pass it to
  // the condition object
  refpObject = new Condition (/*elementName*/);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseConditionCnv::i_fillObj (xercesc::DOMElement* childElement,
                                           DataObject* refpObject,
                                           IOpaqueAddress* address) {
  MsgStream log(msgSvc(), "XmlBaseConditionCnv" );

  // gets the object
  Condition* dataObj = dynamic_cast<Condition*> (refpObject);
  // gets the element's name
  const XMLCh* tagName = childElement->getNodeName();
  // dispatches, based on the name
  if (0 == xercesc::XMLString::compareString(tagName, specificString)) {
    // this is the place where the user will put new elements he wants
    // to add to the default condition
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    xercesc::DOMNodeList* specificChildren = childElement->getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren->getLength(); i++) {
      if (specificChildren->item(i)->getNodeType() ==
          xercesc::DOMNode::ELEMENT_NODE) {
        // gets the current child
        xercesc::DOMNode* childNode = specificChildren->item(i);
        StatusCode sc = i_fillSpecificObj ((xercesc::DOMElement*) childNode,
                                           dataObj,
                                           address);
        if (sc.isFailure()) {
          const XMLCh* childNodeName =
            ((xercesc::DOMElement*) childNode)->getNodeName();
          char* nameString = xercesc::XMLString::transcode(childNodeName);
          log << MSG::WARNING << "parsing of specific child "
              << nameString << " raised errors." << endreq;
          xercesc::XMLString::release(&nameString);
        }
      }
    }
  } else if (0 == xercesc::XMLString::compareString(paramString, tagName) ||
             0 == xercesc::XMLString::compareString(paramVectorString,
                                                    tagName)) {
    // get the attributes
    std::string type = dom2Std (childElement->getAttribute (typeString));
    std::string name = dom2Std (childElement->getAttribute (nameString));
    std::string comment = dom2Std (childElement->getAttribute (commentString));
    
    // gets the value
    std::string value;
    xercesc::DOMNodeList* nodeChildren = childElement->getChildNodes();
    unsigned int i;
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
          if (i>0) value += " ";
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if (0 == xercesc::XMLString::compareString(paramString, tagName)) {
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
    } else if (0 == xercesc::XMLString::compareString
               (paramVectorString, tagName)) {
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
        double dd = xmlSvc()->eval(*it, false);
        if ("int" == type) {
          i_vect.push_back ((int)dd);
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
const CLID& XmlBaseConditionCnv::classID() {
  return CLID_Condition;
}

