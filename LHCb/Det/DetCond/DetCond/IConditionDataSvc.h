//$Id: IConditionDataSvc.h,v 1.2 2001-11-26 18:57:46 andreav Exp $
#ifndef DETCOND_ICONDITIONDATASVC_H
#define DETCOND_ICONDITIONDATASVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// IConditionDataSvc service ID (interface id, major version, minor version)
static const InterfaceID IID_IConditionDataSvc (19700, 1, 0);

// Forward declarations
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionDataSvc IConditionDataSvc.h Det/DetCond/IConditionDataSvc.h

    Abstract interface for a DataSvc manipulating condition data
    (i.e. DataObjects implementing IValidity).

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionDataSvc : virtual public IInterface
{

 public:

  // Re-implemented from IInterface methods

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IConditionDataSvc; }

 public:

  // Handling of synchronous condition data, registered in the TDS

  /// Set the new event time  
  virtual void setEventTime            ( const ITime& time )             = 0;

  /// Check if the event time has been set 
  virtual const bool checkEventTime    ( )                               = 0;

  /// Get the event time  
  virtual const ITime& eventTime       ( )                               = 0;

};

#endif // DETCOND_ICONDITIONDATASVC_H




