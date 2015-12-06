//$Id: ConditionInfo.cpp,v 1.5 2009-02-02 12:20:22 marcocle Exp $

#include "DetDesc/ConditionInfo.h"

#include "DetDesc/Condition.h"
#include "DetDesc/DetDesc.h"

#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

//---------------------------------------------------------------------------

/// Constructor
ConditionInfo::ConditionInfo( IDetectorElement* de,
			      const std::string& condition ) 
{
  m_detElem = de;
  m_conditionName = condition;
  m_condition = 0;
  m_count = 0;
  m_services = DetDesc::services();
}

//---------------------------------------------------------------------------

/// Destructor
ConditionInfo::~ConditionInfo() 
{
  m_services->release();
}

//----------------------------------------------------------------------------

/// Get a pointer to the data service responsible for condition data
IDataProviderSvc* ConditionInfo::dataSvc() const { 
  return m_services->detSvc();
}

//----------------------------------------------------------------------------

/// Get a pointer to the detector element to which the ConditionInfo belongs
IDetectorElement* ConditionInfo::detElem() const {
  return m_detElem;
}

//----------------------------------------------------------------------------

/// Get the name of the associated condition
const std::string& ConditionInfo::conditionName() const {
  return m_conditionName;
}

//----------------------------------------------------------------------------

/// Get a pointer to the associated condition
Condition* ConditionInfo::condition() {
  if( !m_condition ) {
    SmartDataPtr<Condition> pCond( dataSvc(), conditionName() );
    if( pCond ) m_condition = pCond;
  }
  return m_condition;
}

//----------------------------------------------------------------------------

StatusCode ConditionInfo::queryInterface( const InterfaceID& ID , void** ppI ) 
{
  if ( !ppI ) { return StatusCode::FAILURE; }
  *ppI = nullptr ;
  if      ( IConditionInfo::interfaceID()  == ID ) 
    { *ppI = static_cast<IConditionInfo*> ( this ) ; } 
  else if ( IInterface:: interfaceID()    == ID ) 
    { *ppI = static_cast<IInterface*>    ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE                  ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS; 
}

/// add reference
unsigned long ConditionInfo::addRef () { return ++m_count ; }
/// release 
unsigned long ConditionInfo::release() { return  0 < m_count ? --m_count : 0 ;}
