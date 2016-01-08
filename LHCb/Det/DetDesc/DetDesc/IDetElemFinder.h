#ifndef KERNEL_IDETELEMFINDER_H 
#define KERNEL_IDETELEMFINDER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

class IDetectorElement;


/** @class IDetElemFinder IDetElemFinder.h Kernel/IDetElemFinder.h
 *  
 *  Interface for a service that provides associations from the path inside the 
 *  hierarchy of physical volumes to the corresponding detector element.
 *
 *  @author Marco Clemencic
 *  @date   2006-09-01
 */
class IDetElemFinder : public extend_interfaces<IInterface> {
public: 

  /// Return the interface ID
  DeclareInterfaceID( IDetElemFinder, 2, 0 );

  /// Find the detector element instance associated to a given physical volume path.
  /// Returns 0 if there is no suitable detector element.
  virtual const IDetectorElement * detectorElementForPath(const std::string &path) const = 0;


};
#endif // KERNEL_IDETELEMFINDER_H
