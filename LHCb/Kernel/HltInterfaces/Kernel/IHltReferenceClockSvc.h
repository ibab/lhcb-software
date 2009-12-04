// $Id: IHltReferenceClockSvc.h,v 1.1 2009-12-04 08:11:06 graven Exp $
#ifndef IHltReferenceClockSvc_h
#define IHltReferenceClockSvc_h

// Include files
#include "GaudiKernel/INamedInterface.h"

/** @class IHltReferenceClockSvc IHltReferenceClockSvc.h
 *  
 *  functionality:
 *
 *
 *  @author Gerhard Raven and Wouter Hulsbergen
 *  @date   2009-01-01
 */


class IHltReferenceClockSvc : virtual public INamedInterface
{
public:
  static const InterfaceID& interfaceID() {
    static const InterfaceID id( "IHltReferenceClockSvc", 1, 0 );
    return id; 
  }
  
  virtual ~IHltReferenceClockSvc() {}
  virtual double rate() const = 0 ;
  virtual size_t tick() const = 0 ;
};
#endif // IHltReferenceClockSvc_h
