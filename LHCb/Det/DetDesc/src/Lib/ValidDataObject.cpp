//$Id: ValidDataObject.cpp,v 1.2 2001-12-13 19:01:56 andreav Exp $
#include <string> 

#include "DetDesc/ValidDataObject.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"

//---------------------------------------------------------------------------

/// Default constructor 
ValidDataObject::ValidDataObject()
  : DataObject()
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
  : DataObject()
  , m_validSince ( 0 ) 
  , m_validTill  ( 0 ) 
{
  m_validSince = new TimePoint( since ) ;
  m_validTill  = new TimePoint( till  ) ;
}; 

//---------------------------------------------------------------------------

/// Copy constructor
ValidDataObject::ValidDataObject( ValidDataObject& obj )
  : DataObject( (DataObject&)obj )
  , m_validSince ( 0 ) 
  , m_validTill  ( 0 ) 
{
  m_validSince = new TimePoint( obj.validSince() ) ;
  m_validTill  = new TimePoint( obj.validTill()  ) ;
}; 

//---------------------------------------------------------------------------

/// Copy operator.
/// Overloaded from DataObject to point to new TimePoint objects.
ValidDataObject& ValidDataObject::operator= ( ValidDataObject& obj )
{
  DataObject::operator= ( obj );
  delete this->m_validSince;
  delete this->m_validTill;
  this->m_validSince = new TimePoint( obj.validSince() ) ;
  this->m_validTill  = new TimePoint( obj.validTill()  ) ;
  return *this;
}; 

//---------------------------------------------------------------------------

/// Destructor
ValidDataObject::~ValidDataObject() 
{
  delete m_validSince;
  delete m_validTill;
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


