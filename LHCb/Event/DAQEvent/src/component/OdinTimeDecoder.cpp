// $Id: OdinTimeDecoder.cpp,v 1.2 2007-04-17 15:39:59 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// data model
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// local
#include "OdinTimeDecoder.h"

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
  : GaudiTool ( type, name , parent )
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
  if ( m_forceRawEvent || ! exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){

    debug() << "Get LHCb::RawEvent at " << LHCb::RawEventLocation::Default << endmsg;
    LHCb::RawEvent * rawEvent = get<LHCb::RawEvent>(LHCb::RawEventLocation::Default);
    
    debug() << "Get Odin bank" << endmsg;
    // check if there is ODIN bank present
    const std::vector<LHCb::RawBank*>& odinBanks = rawEvent->banks(LHCb::RawBank::ODIN);
    if(odinBanks.size()){

      LHCb::ODIN *odin = new LHCb::ODIN();
      if ( ! odin->set(*odinBanks.begin()).isSuccess() ) Error("Cannot decode ODIN bank");
      put(odin,LHCb::ODINLocation::Default);
      return odin;

    } else {

      warning() << "Cannot find ODIN bank" << endmsg;
      return NULL;

    }

  }
  else {
    return get<LHCb::ODIN>(LHCb::ODINLocation::Default);
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
    last_time = odin->eventTime();
  }
  
  return last_time;
}
//=============================================================================
