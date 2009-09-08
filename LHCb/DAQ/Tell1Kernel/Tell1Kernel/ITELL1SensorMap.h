// $Id: ITELL1SensorMap.h,v 1.1 2009-09-08 16:18:42 krinnert Exp $
#ifndef KERNEL_ITELL1SENSORMAP_H 
#define KERNEL_ITELL1SENSORMAP_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Velo 
{
  static const InterfaceID IID_ITELL1SensorMap ( "ITELL1SensorMap", 1, 0 );

  /** @class ITELL1SensorMap ITELL1SensorMap.h Kernel/ITELL1SensorMap.h
   *  
   *  Interface for map of  TELL1 IDs to sensor numbers and vice versa.
   *
   *  Note that there is some confusion in the naming conventions for the various 
   *  numbering schemmes.  The mapping stored in the CondDB that is accessible 
   *  via VeloDet methods is actually mapping source IDs to sensor numbers.
   *  These can be chosen to be TELL1 numbers, which we did a while ago.
   *  Now they are chosen to be identical with the software sensor numbers in order 
   *  to make changes in the hardware more transparent to the analysis software. 
   *  
   *  However, we still need some means of mapping software sensor numbers to TELL1
   *  hardware IDs (*not source IDs*).  Implementations of this interface must 
   *  follow these semantics. 
   *
   *  Currently the only reliable source for this mapping is the hardware
   *  connectivity data base.  Accessing this from within a Gaudi job requires
   *  a number of system calls and network access. It is therefore unlikely that
   *  there will be an implementation on the WIN32 platform.
   * 
   *  @author Kurt Rinnert
   *  @date   2009-08-11
   */

  class ITELL1SensorMap : virtual public IAlgTool {
    public: 

      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_ITELL1SensorMap; }

      virtual unsigned int tell1ToSensor(unsigned int tell1)=0;
      virtual unsigned int sensorToTell1(unsigned int sensor)=0;

    protected:

    private:

  };
}
#endif // KERNEL_ITELL1SENSORMAP_H
