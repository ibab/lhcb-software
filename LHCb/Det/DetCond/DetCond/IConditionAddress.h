//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/IConditionAddress.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_ICONDITIONADDRESS_H
#define DETCOND_ICONDITIONADDRESS_H 1

// Base class
#include "GaudiKernel/IOpaqueAddress.h"

// Forward declarations
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionAddress IConditionAddress.h Det/DetCond/IConditionAddress.h

    Abstract interface to ConditionData opaque addresses.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionAddress : virtual public IOpaqueAddress {

 public:

  /// Get the event time  
  virtual const ITime* time ( )              = 0 ;

  /// Set the new event time  
  virtual void setTime ( const ITime& time ) = 0 ;

};

#endif // DETCOND_ICONDITIONADDRESS_H

