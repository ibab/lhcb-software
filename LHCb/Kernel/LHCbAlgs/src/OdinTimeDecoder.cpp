// Include files

// from Gaudi
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

// data model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// detector description
#include "DetDesc/RunChangeIncident.h"

// local
#include "OdinTimeDecoder.h"

#ifdef _WIN32
#pragma warning(disable : 4355) // 'this' used in base member initializer list
#endif

//-----------------------------------------------------------------------------
// Implementation file for class : OdinTimeDecoder
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OdinTimeDecoder )

#define ON_DEBUG if ( UNLIKELY( msgLevel(MSG::DEBUG) ) )
#define DEBUG_MSG ON_DEBUG debug()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OdinTimeDecoder::OdinTimeDecoder( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_odinDecoder("ODINDecodeTool",this),
    m_currentRun(0), m_flaggingMode(true)
{
  declareInterface<IEventTimeDecoder>(this);
}

//=============================================================================
// Destructor
//=============================================================================
OdinTimeDecoder::~OdinTimeDecoder() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode OdinTimeDecoder::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  // try to get the tool to decode the ODIN bank
  return m_odinDecoder.retrieve();
}

//=========================================================================
//  Get the ODIN object from T.E.S.
//=========================================================================
LHCb::ODIN *OdinTimeDecoder::getODIN() const
{
  // Check if the root of the transient store is available before calling the
  // ODIN decoder. (e. g. during the initialize)
  DataObject *tmp;
  if (LIKELY( evtSvc()->findObject("", tmp).isSuccess() )) {
    // Decode the ODIN bank.
    m_odinDecoder->execute();
    // @FIXME: we must get the ODIN object from where the Tool created it
    return getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  }
  return 0;
}


//=========================================================================
//  Return the time of current event
//=========================================================================
Gaudi::Time OdinTimeDecoder::getTime ( ) const {

  static Gaudi::Time last_time(0);

  LHCb::ODIN *odin = getODIN();

  if (odin) {
    DEBUG_MSG << "GPS Time = " << odin->gpsTime() << endmsg;

    // We need to trigger a RunChange incident if the run number changes or
    // we switch from flagging mode to filtering mode.
    if ((m_currentRun != odin->runNumber()) ||
        (m_flaggingMode && !odin->isFlagging()) ) {
      ON_DEBUG {
        debug() << "Firing " << IncidentType::RunChange << " incident. ";
        if (m_currentRun != odin->runNumber())
          debug() << " Run change " << m_currentRun
                  << " -> " << odin->runNumber();
        else
          debug() << " Switched from flagging to filtering mode.";
        debug() << endmsg;
      }
      m_currentRun = odin->runNumber();
      m_flaggingMode = odin->isFlagging();
      incSvc()->fireIncident(RunChangeIncident(name(), m_currentRun));
    }
    try {
      last_time = odin->eventTime();
    } catch(TimeException &e) {
      Warning("Problem with ODIN GPS Time '" + e.message() + "', ignored",
              StatusCode::SUCCESS).ignore();
    }
  }

  return last_time;
}
//=============================================================================
