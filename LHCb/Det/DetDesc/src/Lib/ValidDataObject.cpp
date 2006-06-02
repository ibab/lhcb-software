//$Id: ValidDataObject.cpp,v 1.8 2006-06-02 06:59:02 cattanem Exp $
#include <string> 

#include "DetDesc/ValidDataObject.h"

#include "GaudiKernel/Time.h" 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"

//---------------------------------------------------------------------------

static Gaudi::Time local_time_epoch = Gaudi::Time::epoch();
static Gaudi::Time local_time_max   = Gaudi::Time::max();

/// Default constructor 
ValidDataObject::ValidDataObject()
  : IValidity()
  , DataObject()
  , m_validSince (Gaudi::Time::epoch())
  , m_validUntil (Gaudi::Time::max())
  , m_updateMode (DEFAULT)
{}

//---------------------------------------------------------------------------

/// Copy constructor
ValidDataObject::ValidDataObject( ValidDataObject& obj )
  : IValidity()
  , DataObject( (DataObject&)obj )
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
bool ValidDataObject::isValid ( const Gaudi::Time& t ) const {
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
const Gaudi::Time& ValidDataObject::validSince() const {
  if (m_updateMode == ALWAYS_VALID)
    return local_time_epoch;
  return m_validSince; 
};

//---------------------------------------------------------------------------

/// Get end of validity
const Gaudi::Time& ValidDataObject::validTill() const {
  if (m_updateMode == ALWAYS_VALID)
    return local_time_max;
  return m_validUntil;
};

//---------------------------------------------------------------------------

/// Set validity range
void ValidDataObject::setValidity( const Gaudi::Time& since, const Gaudi::Time& till )
{
  setValiditySince( since );
  setValidityTill ( till  );
};

//---------------------------------------------------------------------------

/// Set start of validity
void ValidDataObject::setValiditySince( const Gaudi::Time& since ) 
{
  m_validSince = since;
};

//---------------------------------------------------------------------------

/// Set end of validity
void ValidDataObject::setValidityTill( const Gaudi::Time& till ) 
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
