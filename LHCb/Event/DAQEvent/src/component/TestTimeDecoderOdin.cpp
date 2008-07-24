// $Id: TestTimeDecoderOdin.cpp,v 1.1 2008-07-24 19:22:09 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/Incident.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/ToolFactory.h" 

// data model
#include "Event/ODIN.h"

// local
#include "TestTimeDecoderOdin.h"
#include "RunChangeIncident.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestTimeDecoderOdin
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( DAQEventTests, TestTimeDecoderOdin );

using namespace DAQEventTests;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestTimeDecoderOdin::TestTimeDecoderOdin( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_currentRun(0)
{
  declareInterface<IEventTimeDecoder>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TestTimeDecoderOdin::~TestTimeDecoderOdin() {}

//=========================================================================
//  Return the time of current event
//=========================================================================
Gaudi::Time TestTimeDecoderOdin::getTime ( ) const {

  static Gaudi::Time last_time(0);
  
  LHCb::ODIN *odin = new LHCb::ODIN();
  debug() << "Firing " << IncidentType::RunChange << " incident. Old run="
          << m_currentRun;
  odin->setRunNumber(++m_currentRun);
  debug() << ", new run=" << m_currentRun << endmsg;
  
  put(odin,LHCb::ODINLocation::Default);
  
  incSvc()->fireIncident(Incident(name(),IncidentType::RunChange));
  
  last_time = odin->eventTime();
  
  return last_time;
}
//=============================================================================
