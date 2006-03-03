// $Id: L0MuonKernelFromXML.h,v 1.2 2006-03-03 15:13:39 jucogan Exp $

#ifndef L0MUONKERNEL_L0MUONKERNELFROMXML_H
#define L0MUONKERNEL_L0MUONKERNELFROMXML_H     1

/** @class L0MuonKernelFromXML L0MuonKernelFromXML.h L0MuonKernel/L0MuonKernelFromXML.h

Class representing a factory for creating and 
owning registers
   
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/L0MUnit.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {

  Unit * L0MuonKernelFromXML(std::string XmlFileName);

  int getAttributeInt(DOMNamedNodeMap* di, const char* key);
  std::string getAttributeStr(DOMNamedNodeMap* di, const char* key);

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONKERNELFROMXML_H  
