//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/IConditionsDBCnvSvc.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBCNVSVC_H
#define DETCOND_ICONDITIONSDBCNVSVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// External constants
#include "GaudiKernel/ClassID.h"

// Type definitions
#include "ConditionsDB/CondDBKey.h"

// IConditionsDBCnvSvc service ID (interface id, major version, minor version) 
static const InterfaceID IID_IConditionsDBCnvSvc (19702, 1, 0);

// Forward declarations
class ConditionData;
class IConditionsDBGate;
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionsDBCnvSvc IConditionsDBCnvSvc.h Det/DetCond/IConditionsDBCnvSvc.h

    Abstract interface of a conversion service for CERN-IT CondDB persistency.

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
  
  // Handling of arbitrary condition data, not registered in the TDS

  /// Create a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  virtual 
    StatusCode createConditionData ( ConditionData*&      refpCdata,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time,
				     const CLID&          classID,
				     const unsigned char& type )       = 0;

  /// Create a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  virtual 
    StatusCode createConditionData ( ConditionData*&      refpCdata,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time )       = 0;

  /// Update a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  virtual 
    StatusCode updateConditionData ( ConditionData*       pCdata,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time,
				     const CLID&          classID,
				     const unsigned char& type )       = 0;

  /// Update a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  virtual 
    StatusCode updateConditionData ( ConditionData*       pCdata,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time )       = 0;

  /// Decode classID and storage type from the folder description string.
  virtual 
    StatusCode decodeDescription   ( const std::string&   description,
				     CLID&                classID,
				     unsigned char&       type )       = 0;

  /// Encode classID and storage type into the folder description string.
  virtual 
    StatusCode encodeDescription   ( const CLID&          classID,
				     const unsigned char& type,
				     std::string&         description) = 0;
  
  /// (TEMPORARY?) Handle to the ConditionsDBGate.
  virtual IConditionsDBGate* 
    conditionsDBGate               ( )                                 = 0;

};

#endif // DETCOND_ICONDITIONSDBCNVSVC_H




