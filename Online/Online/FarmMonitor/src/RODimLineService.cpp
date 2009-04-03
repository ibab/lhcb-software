// $Id: RODimLineService.cpp,v 1.1 2009-04-03 09:16:40 ocallot Exp $
// Include files

// local
#include "RODimLineService.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RODimLineService
//
// 2007-12-21 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RODimLineService::RODimLineService( std::string part, int nb ) {
  m_part = part;
  m_dimBuffer.number = nb;
  char serviceName[100];
  sprintf( serviceName, "FarmMonitor/%s/Line%02d", part.c_str(), nb );
  char* format="I:1;C:12;I:11";
  m_service = new DimService( serviceName, format, (void*) &m_dimBuffer, 100 );
}
//=============================================================================
// Destructor
//=============================================================================
RODimLineService::~RODimLineService() {}

//=========================================================================
//  Update the line from the counter, and publish if needed
//=========================================================================
void RODimLineService::update ( RONodeCounter& counter) {
  //== update the Dim service if this is not a blank service and blank counter.
  if ( "" == counter.name() ) {
    if ( strcmp( m_dimBuffer.name, counter.name().c_str() ) == 0 ) return;
  }
  strcpy( m_dimBuffer.name, counter.name().c_str() );
  m_dimBuffer.mep = counter.mep();
  m_dimBuffer.evt = counter.evt();
  m_dimBuffer.snd = counter.snd();
  m_dimBuffer.con = counter.con();
  m_dimBuffer.acc = counter.acc();
  m_dimBuffer.tsk = counter.tsk();
  m_dimBuffer.mepRate = int(counter.mepRate());
  m_dimBuffer.evtRate = int(counter.evtRate());
  m_dimBuffer.sndRate = int(counter.sndRate());
  m_dimBuffer.conRate = int(counter.conRate());
  m_dimBuffer.accRate = int(counter.accRate());
  m_service->updateService();
}
//=============================================================================
