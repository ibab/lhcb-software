//$Id: ValidDataObject.cpp,v 1.6 2005-04-22 13:10:41 marcocle Exp $
#include <string> 

#include "DetDesc/ValidDataObject.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"

//---------------------------------------------------------------------------

/// Default constructor 
ValidDataObject::ValidDataObject()
  : IValidity(), DataObject()
  , m_validSince (time_absolutepast)
  , m_validUntil (time_absolutefuture)
  , m_updateMode (DEFAULT)
{}

//---------------------------------------------------------------------------

/// Constructor
ValidDataObject::ValidDataObject( const ITime& since, 
				  const ITime& till  )
  : IValidity(), DataObject()
  , m_validSince (since)
  , m_validUntil (till)
  , m_updateMode (DEFAULT)
{}

//---------------------------------------------------------------------------

/// Copy constructor
ValidDataObject::ValidDataObject( ValidDataObject& obj )
  : DataObject( (DataObject&)obj ), IValidity()
  , m_validSince (obj.validSince()) 
  , m_validUntil (obj.validTill())
  , m_updateMode (obj.updateMode())
{}

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void ValidDataObject::update ( ValidDataObject& obj )
{
  // copy the validity interval
  m_validSince = obj.validSince();
  m_validUntil = obj.validTill();

  // reset the update mode if needed
  if ( updateMode() == FORCE_UPDATE){
    setUpdateMode(DEFAULT);
  }

}

//---------------------------------------------------------------------------

/// Destructor
ValidDataObject::~ValidDataObject() 
{
}

//---------------------------------------------------------------------------

/// Check if the data object has a well defined validity range
bool ValidDataObject::isValid ( ) const {
  return  validSince() <= validTill();
};

//---------------------------------------------------------------------------

/// Check if the data object is valid at the specified time
bool ValidDataObject::isValid ( const ITime& t ) const {
  switch(m_updateMode){
  case DEFAULT: 
    return validSince() <= t &&  t < validTill();
  case ALWAYS_VALID:
    return true;
  case FORCE_UPDATE:
    return false;
  }
  // just to make the compiler happy
  return true;
};

//---------------------------------------------------------------------------

/// Get start of validity
const ITime& ValidDataObject::validSince() const {
  if (m_updateMode == ALWAYS_VALID)
    return time_absolutepast;
  return m_validSince; 
};

//---------------------------------------------------------------------------

/// Get end of validity
const ITime& ValidDataObject::validTill() const {
  if (m_updateMode == ALWAYS_VALID)
    return time_absolutefuture;
  return m_validUntil;
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
  m_validSince = since;
};

//---------------------------------------------------------------------------

/// Set end of validity
void ValidDataObject::setValidityTill( const ITime& till ) 
{
  m_validUntil = till;
};

//---------------------------------------------------------------------------

/// Update the object using the data provider
StatusCode ValidDataObject::update() {
  IRegistry *pReg = registry();
  if (pReg) {
    IDataProviderSvc *pDataProv = pReg->dataSvc();
    if (pDataProv) {
      return pDataProv->updateObject(this);
    }
  }
  return StatusCode::FAILURE;
}

//---------------------------------------------------------------------------

// Methods for the update mode
const ValidDataObject::UpdateModeFlag & ValidDataObject::updateMode() const {
  return m_updateMode;
}
void ValidDataObject::setUpdateMode(UpdateModeFlag mode){
  m_updateMode = mode;
}
void ValidDataObject::defaultUpdateMode(){
  m_updateMode = DEFAULT;
}
void ValidDataObject::forceUpdateMode(){
  m_updateMode = FORCE_UPDATE;
}
void ValidDataObject::neverUpdateMode(){
  m_updateMode = ALWAYS_VALID;
}
