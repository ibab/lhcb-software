// $Id:  $
// Include files

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

// local
#include "Hlt1TriggerTisTos.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt1TriggerTisTos
//
// 2010-06-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Hlt1TriggerTisTos )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt1TriggerTisTos::Hlt1TriggerTisTos( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TriggerTisTos ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);
}

//=============================================================================
// Destructor
//=============================================================================
Hlt1TriggerTisTos::~Hlt1TriggerTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt1TriggerTisTos::initialize() 
{
  const StatusCode sc = TriggerTisTos::initialize(); 
  if ( sc.isFailure() ) return sc; 

  if ( m_HltDecReportsLocation.value() == LHCb::HltDecReportsLocation::Default )
  {
    m_HltDecReportsLocation = "Hlt1/DecReports";
  }
  if ( m_HltSelReportsLocation.value() == LHCb::HltSelReportsLocation::Default )
  {
    m_HltSelReportsLocation = "Hlt1/SelReports";
  }
  return sc;
}
