// $Id: IReferenceRate.h,v 1.1 2009-12-04 13:07:06 graven Exp $
#ifndef IReferenceRate_h
#define IReferenceRate_h

// Include files
#include "GaudiKernel/INamedInterface.h"

/** @class IReferenceRate IReferenceRate.h
 *  
 *  functionality: provide a reference rate
 *
 *  @author Gerhard Raven and Wouter Hulsbergen
 *  @date   2009-01-01
 */


class IReferenceRate : virtual public INamedInterface
{
public:
  static const InterfaceID& interfaceID() {
    static const InterfaceID id( "IReferenceRate", 1, 0 );
    return id; 
  }
  
  virtual ~IReferenceRate() {}
  virtual double rate() const = 0 ;
  virtual size_t tick() const = 0 ;
};
#endif // IReferenceRate_h
