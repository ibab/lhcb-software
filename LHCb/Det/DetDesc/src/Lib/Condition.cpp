//$Id: Condition.cpp,v 1.5 2003-06-16 13:42:36 sponce Exp $
#include <string> 

#include "DetDesc/Condition.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"

//---------------------------------------------------------------------------

/// Default constructor
Condition::Condition() : ParamValidDataObject() {}; 

//---------------------------------------------------------------------------

/// Constructor
Condition::Condition (const ITime& since, const ITime& till)
  : ParamValidDataObject (since, till) {}; 

//---------------------------------------------------------------------------

/// Copy constructor
Condition::Condition (Condition& obj)
  : ICondition(), ParamValidDataObject ((ParamValidDataObject&)obj) {}; 

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void Condition::update (Condition& obj)
{
  ParamValidDataObject::update ( obj );
}; 

//---------------------------------------------------------------------------

/// Destructor
Condition::~Condition() {};

//----------------------------------------------------------------------------

unsigned long Condition::addRef  () {
  return ParamValidDataObject::addRef();
}

unsigned long Condition::release () {
  return ParamValidDataObject::release();
}

/// queryInterface
StatusCode Condition::queryInterface( const InterfaceID& ID , void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if ( ICondition::interfaceID() == ID ) 
    { *ppI = static_cast<ICondition*> ( this ) ; } 
  else if ( IInterface::interfaceID() == ID ) 
    { *ppI = static_cast<IInterface*> ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS;
};

//----------------------------------------------------------------------------

/// initialize
StatusCode Condition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
