// $Id: OdinTimeDecoder.cpp,v 1.5 2008-07-24 19:18:09 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/Incident.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/ToolFactory.h" 

// data model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// local
#include "OdinTimeDecoder.h"
#include "RunChangeIncident.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OdinTimeDecoder
//
// 2006-09-21 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OdinTimeDecoder );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OdinTimeDecoder::OdinTimeDecoder( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_currentRun(0)
{
  declareProperty( "ForceRawEvent", m_forceRawEvent = false );
  declareInterface<IEventTimeDecoder>(this);
}

//=============================================================================
// Destructor
//=============================================================================
OdinTimeDecoder::~OdinTimeDecoder() {}

//=========================================================================
//  Get the ODIN object from T.E.S.
//=========================================================================
LHCb::ODIN *OdinTimeDecoder::getODIN() const 
{
  if ( ! exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    // Decode ODIN bank from RawEvent
    return decodeODIN();
  }
  else if( m_forceRawEvent ) {
    debug() << "Removing existing " << LHCb::ODINLocation::Default << " object" << endmsg;
    evtSvc()->unlinkObject( LHCb::ODINLocation::Default );
    // Decode ODIN bank from RawEvent
    return decodeODIN();
  }
  else {
    return get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  }
}
//=========================================================================
// Decode ODIN bank from RawEvent
//=========================================================================
LHCb::ODIN *OdinTimeDecoder::decodeODIN() const 
{
  debug() << "Getting " << LHCb::RawEventLocation::Default << endmsg;
  LHCb::RawEvent* rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
  
  debug() << "Creating " << LHCb::ODINLocation::Default << " object from ODIN bank" << endmsg;
  // check if there is ODIN bank present
  const std::vector<LHCb::RawBank*>& odinBanks = rawEvent->banks(LHCb::RawBank::ODIN);
  if(odinBanks.size()){
    LHCb::ODIN *odin = new LHCb::ODIN();
    if ( ! odin->set(*odinBanks.begin()).isSuccess() ) Error("Cannot decode ODIN bank");
    put(odin,LHCb::ODINLocation::Default);
    return odin;
  }
  else {
    warning() << "Cannot find ODIN bank in RawEvent" << endmsg;
    return NULL;
  }
}

//=========================================================================
//  Return the time of current event
//=========================================================================
Gaudi::Time OdinTimeDecoder::getTime ( ) const {

  static Gaudi::Time last_time(0);

  LHCb::ODIN *odin = getODIN();

  if (odin) {
    debug() << "GPS Time = " << odin->gpsTime() << endmsg;
    
    // Check the run number in ODIN
    if (m_currentRun != odin->runNumber()) {
      debug() << "Firing " << IncidentType::RunChange << " incident. Old run="
              << m_currentRun;
      m_currentRun = odin->runNumber();
      debug() << ", new run=" << m_currentRun << endmsg;
      incSvc()->fireIncident(Incident(name(),IncidentType::RunChange));
    }
    
    last_time = odin->eventTime();    
  }
  
  return last_time;
}
//=============================================================================
