//$Id: ConditionsDBAddress.cpp,v 1.3 2001-11-26 19:07:34 andreav Exp $

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
					   const ITime& validityTime,
					   const CLID& clid,
					   const unsigned char& stringType,
					   IRegistry* entry )

  : GenericAddress  ( CONDDB_StorageType,
		      clid                )
  , m_folderName    ( folderName          )
  , m_tagName       ( tagName             )
  , m_stringType    ( stringType          )
{
  setRegistry( entry );
  m_time = new TimePoint ( validityTime );
}

//---------------------------------------------------------------------------

/// Constructor 
ConditionsDBAddress::ConditionsDBAddress ( const std::string& folderName,
					   const std::string& tagName,   
					   const ITime& validityTime,
					   IRegistry* entry )
  : GenericAddress  ( CONDDB_StorageType,
		      ConditionsDBAddress_undefinedClassID )
  , m_folderName    ( folderName          )
  , m_tagName       ( tagName             )
  , m_stringType    ( ConditionsDBAddress_undefinedStringType )

{
  setRegistry( entry );
  m_time = new TimePoint ( validityTime );
}

//---------------------------------------------------------------------------

/// Destructor
ConditionsDBAddress::~ConditionsDBAddress()
{
  delete m_time;
}

//---------------------------------------------------------------------------

/// Get the event time  
const ITime& ConditionsDBAddress::time ( ) {
  return *m_time; 
}

//---------------------------------------------------------------------------

/// Set the new validity time  
void ConditionsDBAddress::setTime ( const ITime& time ) {
  if( 0 != m_time ) delete m_time; 
  m_time = new TimePoint( time );   
}

//---------------------------------------------------------------------------

/// Get the folder name
const std::string ConditionsDBAddress::folderName ( ) {
  return m_folderName; 
}

//---------------------------------------------------------------------------

/// Get the tag name
const std::string ConditionsDBAddress::tagName ( ) {
  return m_tagName; 
}

//---------------------------------------------------------------------------

/// Get the string type
const unsigned char& ConditionsDBAddress::stringType ( ) {
  return m_stringType; 
}

//---------------------------------------------------------------------------

