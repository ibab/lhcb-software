//$Id: IConditionsDBCnvSvc.h,v 1.6 2001-12-17 21:22:46 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBCNVSVC_H
#define DETCOND_ICONDITIONSDBCNVSVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// External constants
#include "GaudiKernel/ClassID.h"

// Type definition
#include "DetCond/CondDBKey.h"

// IConditionsDBCnvSvc service ID (interface id, major version, minor version) 
static const InterfaceID IID_IConditionsDBCnvSvc ("IConditionsDBCnvSvc", 1, 0);

// Forward declarations
class DataObject;
class IConditionsDBGate;
class IRegistry;
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionsDBCnvSvc IConditionsDBCnvSvc.h Det/DetCond/IConditionsDBCnvSvc.h

    Abstract interface of a conversion service for CERN-IT CondDB persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Andrea Valassi 
    @date February 2001
*///--------------------------------------------------------------------------

class IConditionsDBCnvSvc : virtual public IInterface
{

 public:

  // Re-implemented from IInterface

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IConditionsDBCnvSvc; }

 public:
  
  // Create/update condition DataObject not necessarily registered in the TDS.

  /// Create a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store,
  /// but may register TDS addresses for its children if needed (e.g. Catalog).
  /// The string storage type is discovered at runtime in the CondDB.
  /// The entry name identifies a condition amongst the many in the string.
 virtual
    StatusCode createConditionData( DataObject*&         refpObject,
				    const std::string&   folderName,
				    const std::string&   tagName,
				    const std::string&   entryName,
				    const ITime&         time,
				    const CLID&          classID,
				    IRegistry*           entry=0 ) = 0;
  
  /// Update a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store,
  /// but may register TDS addresses for its children if needed (e.g. Catalog).
  /// The string storage type is discovered at runtime in the CondDB.
 /// The entry name identifies a condition amongst the many in the string.
  virtual
    StatusCode updateConditionData( DataObject*          pObject,
				    const std::string&   folderName,
				    const std::string&   tagName,
				    const std::string&   entryName,
				    const ITime&         time,
				    const CLID&          classID,
				    IRegistry*           entry=0 ) = 0;
  
  /// Decode the string storage type from the folder description string
  virtual 
    StatusCode decodeDescription( const std::string&   description,
				  unsigned char&       type ) = 0;
  
  /// Encode the string storage type into the folder description string
  virtual 
    StatusCode encodeDescription( const unsigned char& type,
				  std::string&         description ) = 0;
  
  /// Get the global tag name
  virtual const std::string& globalTag ( ) = 0;

  /// Get a handle to the ConditionsDBGate
  virtual IConditionsDBGate* conditionsDBGate ( ) = 0;

};

#endif // DETCOND_ICONDITIONSDBCNVSVC_H




