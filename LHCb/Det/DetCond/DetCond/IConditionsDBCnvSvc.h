//$Id: IConditionsDBCnvSvc.h,v 1.9 2005-02-09 08:30:53 marcocle Exp $
#ifndef DETCOND_ICONDITIONSDBCNVSVC_H
#define DETCOND_ICONDITIONSDBCNVSVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// External constants
#include "GaudiKernel/ClassID.h"

// Type definition
#include "CoolKernel/IValidityKey.h"

// IConditionsDBCnvSvc service ID (interface id, major version, minor version) 
static const InterfaceID IID_IConditionsDBCnvSvc ("IConditionsDBCnvSvc", 2, 0);

// Forward declarations
class DataObject;
class ICondDBAccessSvc;
class IRegistry;
class ITime;
class TimePoint;

///---------------------------------------------------------------------------
/** @class IConditionsDBCnvSvc IConditionsDBCnvSvc.h Det/DetCond/IConditionsDBCnvSvc.h

    Abstract interface of a conversion service for CERN-IT CondDB persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Andrea Valassi
    @date February 2001
    @author Marco Clemencic
    @author Nicolas Gilardi
    @date November 2004
*///--------------------------------------------------------------------------

class IConditionsDBCnvSvc : virtual public IInterface
{

 public:

  // Re-implemented from IInterface

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IConditionsDBCnvSvc; }

 public:
  
  /// Get the global tag name
  virtual const std::string& globalTag ( ) = 0;

  /// Convert from TimePoint class to cool::ValidityKey.
  virtual cool::IValidityKey timeToValKey(const TimePoint &) = 0;
   
  /// Convert from cool::ValidityKey to TimePoint class.
  virtual TimePoint valKeyToTime(const cool::IValidityKey &) = 0;

};

#endif // DETCOND_ICONDITIONSDBCNVSVC_H




