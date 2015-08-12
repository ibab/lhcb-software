// $Id:  $
// Include files

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

// local
#include "Hlt2TriggerTisTos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2TriggerTisTos
//
// 2010-06-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Hlt2TriggerTisTos )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2TriggerTisTos::Hlt2TriggerTisTos( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TriggerTisTos ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);
}

//=============================================================================
// Destructor
//=============================================================================
Hlt2TriggerTisTos::~Hlt2TriggerTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2TriggerTisTos::initialize() 
{
  const StatusCode sc = TriggerTisTos::initialize(); 
  if ( sc.isFailure() ) return sc; 

  if ( m_HltDecReportsLocation.value() == LHCb::HltDecReportsLocation::Default )
  {
    m_HltDecReportsLocation = "Hlt2/DecReports";
  }
  if ( m_HltSelReportsLocation.value() == LHCb::HltSelReportsLocation::Default )
  {
    m_HltSelReportsLocation = "Hlt2/SelReports";
  }
  return sc;
}
