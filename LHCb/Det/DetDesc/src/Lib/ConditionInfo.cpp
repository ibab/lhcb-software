//$Id: ConditionInfo.cpp,v 1.1 2001-12-13 19:19:15 andreav Exp $

#include "ConditionInfo.h"

#include "DetDesc/Condition.h"
#include "DetDesc/DetDesc.h"

#include "GaudiKernel/SmartDataPtr.h" 

//---------------------------------------------------------------------------

/// Constructor
ConditionInfo::ConditionInfo( IDetectorElement* de,
			      const std::string& condition ) 
{
  m_detElem = de;
  m_conditionName = condition;
  m_condition = 0;
}; 

//---------------------------------------------------------------------------

/// Destructor
ConditionInfo::~ConditionInfo() 
{
};

//----------------------------------------------------------------------------

/// Get a pointer to the data service responsible for condition data
IDataProviderSvc* ConditionInfo::dataSvc() { 
  return DetDesc::detSvc(); 
}

//----------------------------------------------------------------------------

/// Get a pointer to the detector element to which the ConditionInfo belongs
IDetectorElement* ConditionInfo::detElem() const {
  return m_detElem;
};

//----------------------------------------------------------------------------

/// Get the name of the associated condition
const std::string ConditionInfo::conditionName() const {
  return m_conditionName;
};

//----------------------------------------------------------------------------

/// Get a pointer to the associated condition
Condition* ConditionInfo::condition() {
  if( m_condition == 0 ) {
    SmartDataPtr<Condition> pCond( dataSvc(), conditionName() );
    if( 0 != pCond ) m_condition = pCond;
  }
  return m_condition;
};

//----------------------------------------------------------------------------

