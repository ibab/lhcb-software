// $Id: L0TriggerTisTos.cpp,v 1.1 2010-06-23 22:39:24 tskwarni Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

// local
#include "L0TriggerTisTos.h"


//-----------------------------------------------------------------------------
// Implementation file for class : L0TriggerTisTos
//
// 2010-06-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0TriggerTisTos )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0TriggerTisTos::L0TriggerTisTos( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : TriggerTisTos ( type, name , parent )
{
  declareInterface<ITriggerTisTos>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0TriggerTisTos::~L0TriggerTisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0TriggerTisTos::initialize() {
  StatusCode sc = TriggerTisTos::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( m_HltDecReportsLocation.value() == LHCb::HltDecReportsLocation::Default ){
    m_HltDecReportsLocation = "HltLikeL0/DecReports";
  }
  if( m_HltSelReportsLocation.value() == LHCb::HltSelReportsLocation::Default ){
    m_HltSelReportsLocation = "HltLikeL0/SelReports";
  }
  info() << " L0DecReportsMaker and L0SelReportsMaker must be executed before using this tool" << endmsg;  
  info() << " DecReport location = " << m_HltDecReportsLocation.value();
  info() << " SelReport location = " << m_HltSelReportsLocation.value();
  info() << endmsg;  
  
  return StatusCode::SUCCESS;
}

