//$Id: ValidDataObject.cpp,v 1.5 2003-06-16 13:42:36 sponce Exp $
#include <string> 

#include "DetDesc/ValidDataObject.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"

//---------------------------------------------------------------------------

/// Default constructor 
ValidDataObject::ValidDataObject()
  : IValidity(), DataObject()
  , m_validSince ( 0 ) 
  , m_validTill  ( 0 ) 
{
  m_validSince = new TimePoint( time_absolutepast   ) ;
  m_validTill  = new TimePoint( time_absolutefuture ) ;
}; 

//---------------------------------------------------------------------------

/// Constructor
ValidDataObject::ValidDataObject( const ITime& since, 
				  const ITime& till  )
  : IValidity(), DataObject()
  , m_validSince ( 0 ) 
  , m_validTill  ( 0 ) 
{
  m_validSince = new TimePoint( since ) ;
  m_validTill  = new TimePoint( till  ) ;
}; 

//---------------------------------------------------------------------------

/// Copy constructor
ValidDataObject::ValidDataObject( ValidDataObject& obj )
  : IValidity(), DataObject( (DataObject&)obj )
  , m_validSince ( 0 ) 
  , m_validTill  ( 0 ) 
{
  m_validSince = new TimePoint( obj.validSince() ) ;
  m_validTill  = new TimePoint( obj.validTill()  ) ;
}; 

//---------------------------------------------------------------------------

/*
/// Copy operator (virtual!).
/// Overloaded from DataObject to point to new TimePoint objects.
ValidDataObject& ValidDataObject::operator= ( ValidDataObject& obj )
{
  DataObject::operator= ( obj );
  // Call virtual method update to deep copy all contents
  update( obj );
  return *this;
}; 
*/

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void ValidDataObject::update ( ValidDataObject& obj )
{
  delete m_validSince;
  delete m_validTill;
  m_validSince = new TimePoint( obj.validSince() ) ;
  m_validTill  = new TimePoint( obj.validTill()  ) ;
}; 

//---------------------------------------------------------------------------

/// Destructor
ValidDataObject::~ValidDataObject() 
{
  delete m_validSince;
  m_validSince = 0;
  delete m_validTill;
  m_validTill = 0;
};

//---------------------------------------------------------------------------

/// Check if the data object has a well defined validity range
bool ValidDataObject::isValid ( ) {
  return  validSince() <= validTill();
};

//---------------------------------------------------------------------------

/// Check if the data object is valid at the specified time
bool ValidDataObject::isValid ( const ITime& t ) {
  return validSince() <= t &&  t <= validTill();
};

//---------------------------------------------------------------------------

/// Get start of validity
const ITime& ValidDataObject::validSince() { 
  return *m_validSince; 
};

//---------------------------------------------------------------------------

/// Get end of validity
const ITime& ValidDataObject::validTill() { 
  return *m_validTill;  
};

//---------------------------------------------------------------------------

/// Set validity range
void ValidDataObject::setValidity( const ITime& since, const ITime& till )
{
  setValiditySince( since );
  setValidityTill ( till  );
};

//---------------------------------------------------------------------------

/// Set start of validity
void ValidDataObject::setValiditySince( const ITime& since ) 
{
  delete m_validSince;
  m_validSince = new TimePoint( since );
};

//---------------------------------------------------------------------------

/// Set end of validity
void ValidDataObject::setValidityTill( const ITime& till ) 
{
  delete m_validTill;
  m_validTill = new TimePoint( till );
};

//---------------------------------------------------------------------------

/// Update the validity range (foreseen for tree-like structures)
StatusCode ValidDataObject::updateValidity() 
{
  return StatusCode::SUCCESS;
};

//---------------------------------------------------------------------------


