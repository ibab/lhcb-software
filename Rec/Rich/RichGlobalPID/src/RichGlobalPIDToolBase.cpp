
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDToolBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : 
 *        Rich::Rec::GlobalPID::ToolBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichGlobalPIDToolBase.h"

//=============================================================================
// Force creation of templated class
#include "RichGlobalPIDCommonBase.icpp"
template class Rich::Rec::GlobalPID::CommonBase<Rich::Rec::HistoToolBase>;
//=============================================================================

//-----------------------------------------------------------------------------

// Method that handles various Gaudi "software events"
void Rich::Rec::GlobalPID::ToolBase::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) 
  { 
    this->deleteGPIDEvent();
  }
}

StatusCode Rich::Rec::GlobalPID::ToolBase::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = CommonBase<Rich::Rec::HistoToolBase>::initialize();
  if ( sc.isFailure() ) return sc;

  // Setup incident services
  this->incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;  
}

