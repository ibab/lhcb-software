// $Id: DimNodeService.cpp,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
// Include files 



// local
#include "DimNodeService.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DimNodeService
//
// 2007-12-21 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DimNodeService::DimNodeService( int nb ) {
  m_dimBuffer.number = nb;
  char serviceName[100];
  sprintf( serviceName, "FarmMonitor/Line%02d/Status", nb );
  char* format="I:1;C:12;C:12;I:5";
  m_service = new DimService( serviceName, format, (void*) &m_dimBuffer, 80 );
}
//=============================================================================
// Destructor
//=============================================================================
DimNodeService::~DimNodeService() {}

//=========================================================================
//  Update the line from the counter, and publish if needed
//=========================================================================
void DimNodeService::update ( NodeCounter* counter) {

  //== update the Dim service
  bool mustUpdate = false;
  if ( m_dimBuffer.name != counter->name() ) {
    mustUpdate = true;
    strcpy( m_dimBuffer.name, counter->name().c_str() );
  }
  if ( m_dimBuffer.rqd != counter->nRequested() ) {
    mustUpdate = true;
    m_dimBuffer.rqd = counter->nRequested();
  }
  if ( m_dimBuffer.rcv != counter->nReceived() ) {
    mustUpdate = true;
    m_dimBuffer.rcv = counter->nReceived();
  }
  if ( m_dimBuffer.inc != counter->nIncomplete() ) {
    mustUpdate = true;
    m_dimBuffer.inc = counter->nIncomplete();
  }
  if (m_dimBuffer.pro != counter->nProduced() ) {
    mustUpdate = true;
    m_dimBuffer.pro = counter->nProduced();
  }
  if ( m_dimBuffer.wri != counter->nWritten() ) {
    mustUpdate = true;
    m_dimBuffer.wri = counter->nWritten();
  }
  if ( mustUpdate ) {
    char format[12]="%H:%M:%S";
    time_t tt = time( 0 );
    const struct tm* myTime =  localtime( &tt );
    strftime( m_dimBuffer.time, 12, format, myTime );

    m_service->updateService();
  }
}
//=============================================================================
