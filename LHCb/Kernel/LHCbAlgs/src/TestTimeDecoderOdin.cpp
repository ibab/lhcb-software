// Include files

// from Gaudi
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

// detector description
#include "DetDesc/RunChangeIncident.h"

// data model
#include "Event/ODIN.h"

// local
#include "TestTimeDecoderOdin.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestTimeDecoderOdin
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( LHCbAlgsTests, TestTimeDecoderOdin )

using namespace LHCbAlgsTests;
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
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Firing " << IncidentType::RunChange << " incident. Old run="
            << m_currentRun;
  odin->setRunNumber(++m_currentRun);
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << ", new run=" << m_currentRun << endmsg;

  put(odin,LHCb::ODINLocation::Default);

  incSvc()->fireIncident(RunChangeIncident(name(),m_currentRun));

  last_time = odin->eventTime();

  return last_time;
}
//=============================================================================
