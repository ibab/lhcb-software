// $Id: RODimFsmService.cpp,v 1.2 2010-10-18 06:25:08 ocallot Exp $
// Include files
#include <cstdio>

// local
#include "RODimFsmService.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RODimFsmService
//
// 2007-12-21 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RODimFsmService::RODimFsmService( std::string part, int nb ) {
  m_part = part;
  m_dimBuffer.number = nb;
  char serviceName[100];
  sprintf( serviceName, "FarmMonitor/%s/Fsm%02d", part.c_str(), nb );
  char format[30]="I:2;C:12;C:128";
  m_service = new DimService( serviceName, format, (void*) &m_dimBuffer, 160 );
}
//=============================================================================
// Destructor
//=============================================================================
RODimFsmService::~RODimFsmService() {}

//=========================================================================
//  Update the line from the counter, and publish if needed
//=========================================================================
void RODimFsmService::update ( int severity, std::string state, std::string message) {
  //== update the Dim service if this is not a blank service and blank counter.
  if ( "" == state && "" == message ) {
    if ( strcmp( m_dimBuffer.state, state.c_str() ) == 0 ) return;
  }
  m_dimBuffer.severity = severity;
  strcpy( m_dimBuffer.state,   state.c_str() );
  strcpy( m_dimBuffer.message, message.c_str() );
  m_service->updateService();
}
//=============================================================================
