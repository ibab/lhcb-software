//$Id: ConditionsDBAddress.h,v 1.3 2001-11-26 19:07:34 andreav Exp $
#ifndef DETCOND_CONDITIONSDBADDRESS_H
#define DETCOND_CONDITIONSDBADDRESS_H 1

#include <string>

// Base classes
#include "DetCond/IConditionsDBAddress.h"
#include "GaudiKernel/GenericAddress.h"

///---------------------------------------------------------------------------
/** @class ConditionsDBAddress ConditionsDBAddress.h Det/DetCond/ConditionsDBAddress.h

    An IOpaqueAddress for CERN-IT CondDB data persistency.

    @author Andrea Valassi 
    @date August 2001
*///---------------------------------------------------------------------------

class ConditionsDBAddress : virtual public IConditionsDBAddress, 
			    public GenericAddress {
    
 public:
    
  /// Constructor.
  ConditionsDBAddress( const std::string& folderName,   // CondDB folderName
		       const std::string& tagName,      // CondDB tagName
		       const ITime& validityTime,       // CondDB time
		       const CLID& clid,                // classID
		       const unsigned char& stringType, // string type
		       IRegistry* entry = 0);           // data store entry

  /// Constructor.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  ConditionsDBAddress( const std::string& folderName,   // CondDB folderName
		       const std::string& tagName,      // CondDB tagName
		       const ITime& validityTime,       // CondDB time
		       IRegistry* entry = 0);           // data store entry

  /// Destructor
  ~ConditionsDBAddress();

 public:

  // Implementation of IConditionAddress

  /// Get the event time  
  virtual const ITime& time ( );

  /// Set the new validity time  
  virtual void setTime ( const ITime& time );

 public:

  // Implementation of IConditionsDBAddress

  /// Get the folder name 
  const std::string folderName    ( );

  /// Get the tag name
  const std::string tagName       ( );

  /// Get the storage type for the string stored in the ConditionsDB
  const unsigned char& stringType ( );

 private:
  
  // IConditionAddress-specific data members

  /// Validity time of the ConditionData
  ITime*          m_time;

 private:
  
  // IConditionsDBAddress-specific data members

  /// Folder name in the CondDB
  std::string     m_folderName;

  /// Tag name in the CondDB
  std::string     m_tagName;

  /// Storage type for the string stored in the CondDB
  unsigned char   m_stringType;

};

#endif // DETCOND_CONDITIONSDBADDRESS_H

