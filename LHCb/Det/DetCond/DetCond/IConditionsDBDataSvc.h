//$Id: IConditionsDBDataSvc.h,v 1.2 2001-11-26 18:57:46 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBDATASVC_H
#define DETCOND_ICONDITIONSDBDATASVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"
#include "DetCond/IConditionDataSvc.h"

// External constants
#include "GaudiKernel/ClassID.h"

// Service ID (interface id, major version, minor version)
static const InterfaceID IID_IConditionsDBDataSvc (19701, 1, 0);

// Forward declarations
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionsDBDataSvc IConditionsDBDataSvc.h Det/DetCond/IConditionsDBDataSvc.h

    Abstract interface for a ConditionDataSvc based on the CERN-IT CondDB.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionsDBDataSvc : virtual public IInterface
{

 public:

  // Reimplemented from IInterface

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IConditionsDBDataSvc; }

 public:

  /// Set the global tag name 
  virtual void setTagName              ( const std::string& tag )         = 0;

  /// Get the global tag name
  virtual const std::string& tagName   ( )                                = 0;

  /// Convert the folder name in the CondDB to the transient data store path
  virtual StatusCode getNameInCondDB   ( std::string& folderName,
					 const std::string& path )        = 0;

  /// Convert the transient data store path to the folder name in the CondDB
  virtual StatusCode getNameInStore    ( std::string& path,
					 const std::string& folderName )  = 0;

  /// Create a valid DataObject by folder name (for default tag and key),
  /// then load it in the TDS using the implicit naming convention:
  /// if the DataObject exists already, update it instead (if necessary).
  /// Specify the clID of the DataObject and the technology type 
  /// for the strings stored in the CondDB.
  virtual 
    StatusCode retrieveValidCondition  ( DataObject*&         refpObject,
					 const std::string&   folderName,
					 const CLID&          classID,
					 const unsigned char& type       ) = 0;
  
  /// Create a valid DataObject by folder name (for default tag and key),
  /// then load it in the TDS using the implicit naming convention:
  /// if the DataObject exists already, update it instead (if necessary).
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  virtual 
    StatusCode retrieveValidCondition  ( DataObject*&         refpObject,
					 const std::string&   folderName ) = 0;
  
};

#endif // DETCOND_ICONDITIONSDBDATASVC_H





