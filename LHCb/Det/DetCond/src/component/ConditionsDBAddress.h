//$Id: ConditionsDBAddress.h,v 1.4 2001-11-27 18:23:52 andreav Exp $
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
  ConditionsDBAddress( const std::string& folderName,    // CondDB folderName
		       const std::string& tagName,       // CondDB tagName
		       const CLID& clid,                 // classID
		       const unsigned char& stringType); // string type

  /// Constructor.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  ConditionsDBAddress( const std::string& folderName,    // CondDB folderName
		       const std::string& tagName);      // CondDB tagName

  /// Destructor
  ~ConditionsDBAddress();

 public:

  // Implementation of IConditionsDBAddress

  /// Get the folder name 
  const std::string folderName    ( );

  /// Get the tag name
  const std::string tagName       ( );

  /// Get the storage type for the string stored in the ConditionsDB
  const unsigned char& stringType ( );

};

#endif // DETCOND_CONDITIONSDBADDRESS_H

