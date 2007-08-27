// $Id: L0MuonKernelFromXML.h,v 1.3 2007-08-27 09:32:24 jucogan Exp $

#ifndef L0MUONKERNEL_L0MUONKERNELFROMXML_H
#define L0MUONKERNEL_L0MUONKERNELFROMXML_H     1

/** @class L0MuonKernelFromXML L0MuonKernelFromXML.h L0MuonKernel/L0MuonKernelFromXML.h

Class representing a factory for creating and 
owning registers
   
   
*/ 

#include <string>
#include <map>
#include "ProcessorKernel/RegisterFactory.h"
//#include "ProcessorKernel/Unit.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace L0Muon {

//   Unit* L0MuonKernelFromXML(std::string XmlFileName);
  void L0MuonKernelFromXML(std::string XmlFileName, bool emulator=true);

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONKERNELFROMXML_H  
