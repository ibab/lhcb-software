//$Id: IConditionAddress.h,v 1.3 2001-11-26 18:57:46 andreav Exp $
#ifndef DETCOND_ICONDITIONADDRESS_H
#define DETCOND_ICONDITIONADDRESS_H 1

// Base class
#include "GaudiKernel/IOpaqueAddress.h"

// Forward declarations
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionAddress IConditionAddress.h Det/DetCond/IConditionAddress.h

    Abstract interface to opaque addresses for condition data objects 
    (i.e. DataObjects implementing IValidity)

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionAddress {

 public:

  /// Get the event time  
  virtual const ITime& time ( )              = 0 ;

  /// Set the new event time  
  virtual void setTime ( const ITime& time ) = 0 ;

};

#endif // DETCOND_ICONDITIONADDRESS_H

