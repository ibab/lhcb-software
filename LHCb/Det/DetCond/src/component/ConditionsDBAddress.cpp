//$Id: ConditionsDBAddress.cpp,v 1.4 2001-11-27 18:23:52 andreav Exp $

#include "ConditionsDBAddress.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/TimePoint.h"

/// If not specifed, type and classID are discovered at runtime in the CondDB
/// Use invalid values as keywords: CLID_DataObject and TEST_StorageType
const CLID&          ConditionsDBAddress_undefinedClassID   = CLID_DataObject;
const unsigned char& ConditionsDBAddress_undefinedStringType= TEST_StorageType;

//---------------------------------------------------------------------------

/// Constructor 
ConditionsDBAddress::ConditionsDBAddress ( const std::string& folderName,
					   const std::string& tagName,   
					   const CLID& clid,
					   const unsigned char& stringType )
  : GenericAddress  ( CONDDB_StorageType,
		      clid,
		      folderName,
		      tagName,
		      (unsigned long)stringType )
{
}

//---------------------------------------------------------------------------

/// Constructor 
ConditionsDBAddress::ConditionsDBAddress ( const std::string& folderName,
					   const std::string& tagName    )
  : GenericAddress  ( CONDDB_StorageType,
		      ConditionsDBAddress_undefinedClassID,
		      folderName,
		      tagName,
		      (unsigned long)ConditionsDBAddress_undefinedStringType )
{
}

//---------------------------------------------------------------------------

/// Destructor
ConditionsDBAddress::~ConditionsDBAddress()
{
}

//---------------------------------------------------------------------------

/// Get the folder name
const std::string ConditionsDBAddress::folderName ( ) {
  return par()[0]; 
}

//---------------------------------------------------------------------------

/// Get the tag name
const std::string ConditionsDBAddress::tagName ( ) {
  return par()[1]; 
}

//---------------------------------------------------------------------------

/// Get the string type
const unsigned char& ConditionsDBAddress::stringType ( ) {
  return (unsigned char)(ipar()[0]); 
}

//---------------------------------------------------------------------------


