// $Id: L0MuonKernelFromXML.h,v 1.5 2008-12-15 10:19:47 cattanem Exp $

#ifndef L0MUONKERNEL_L0MUONKERNELFROMXML_H
#define L0MUONKERNEL_L0MUONKERNELFROMXML_H     1

#include <string>

namespace L0Muon {

  /**   Function to create the units and registers factories for the L0Muon
        processor from  the xml representation in the given file.

        Notes :
        - the register factory instance has to be specified before hand
        - input parameter emulator :
        - false : only the Register factory is created
        - true  : the Unit factory is created in addition to the Register factory

        @author Julien Cogan
        @date  July the 1st 2005
  */
  void L0MuonKernelFromXML(std::string XmlFileName, bool emulator=true);

}  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MUONKERNELFROMXML_H
