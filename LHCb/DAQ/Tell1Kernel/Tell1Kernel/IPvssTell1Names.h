// $Id: IPvssTell1Names.h,v 1.1 2009-09-08 16:18:42 krinnert Exp $
#ifndef KERNEL_IPVSSTELL1NAMES_H 
#define KERNEL_IPVSSTELL1NAMES_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Velo 
{
  static const InterfaceID IID_IPvssTell1Names ( "IPvssTell1Names", 1, 0 );

  /** @class IPvssTell1Names IPvssTell1Names.h Kernel/IPvssTell1Names.h
   *  
   *  Provides PVSS TELL1 names.
   * 
   *  @author Kurt Rinnert
   *  @date   2009-08-21
   */

  class IPvssTell1Names : virtual public IAlgTool {

    public: 

      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_IPvssTell1Names; }

      /// get PVSS TELL1 name for given sensor number
      virtual const std::string& pvssName( unsigned int sensorNumber )=0;

  };
}
#endif // KERNEL_IPVSSTELL1NAMES_H
