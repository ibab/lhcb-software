// $Id: IDetElemFinder.h,v 1.1 2008-07-21 14:51:45 cattanem Exp $
#ifndef KERNEL_IDETELEMFINDER_H 
#define KERNEL_IDETELEMFINDER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"

class IDetectorElement;

static const InterfaceID IID_IDetElemFinder ( "IDetElemFinder", 1, 0 );

/** @class IDetElemFinder IDetElemFinder.h Kernel/IDetElemFinder.h
 *  
 *  Interface for a service that provides associations from the path inside the 
 *  hierarchy of physical volumes to the corresponding detector element.
 *
 *  @author Marco Clemencic
 *  @date   2006-09-01
 */
class IDetElemFinder : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IDetElemFinder; }  

  /// Find the detector element instance associated to a given physical volume path.
  /// Returns 0 if there is no suitable detector element.
  virtual const IDetectorElement * detectorElementForPath(const std::string &path) const = 0;

protected:

private:

};
#endif // KERNEL_IDETELEMFINDER_H
