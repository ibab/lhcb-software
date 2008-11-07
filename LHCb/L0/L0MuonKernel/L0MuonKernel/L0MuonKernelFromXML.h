// $Id: L0MuonKernelFromXML.h,v 1.4 2008-11-07 16:23:38 jucogan Exp $

#ifndef L0MUONKERNEL_L0MUONKERNELFROMXML_H
#define L0MUONKERNEL_L0MUONKERNELFROMXML_H     1

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
  
  /**   Function to create the units and registers factories for the L0Muon processor from 
        the xml representation in the given file.
    
        Notes : 
        - the register factory instance has to be specified before hand
        - input parameter emulator : 
        - false : only the Register factory is created 
        - true  : the Unit factory is created in addition to the Register factory
        
        @author Julien Cogan
        @date  July the 1st 2005
  */
  void L0MuonKernelFromXML(std::string XmlFileName, bool emulator=true);

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONKERNELFROMXML_H  
