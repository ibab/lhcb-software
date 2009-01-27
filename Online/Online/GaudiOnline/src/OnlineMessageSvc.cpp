// $Id: OnlineMessageSvc.cpp,v 1.15 2009-01-27 08:30:02 cattanem Exp $

#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/IErrorLogger.h"
#include "GaudiOnline/OnlineMessageSvc.h"
#include "RTL/rtl.h"
#include <sstream>

//DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineMessageSvc);
using namespace std;

static IErrorLogger* s_logger = 0;
void LHCb::OnlineMessageSvc::setErrorLogger(IErrorLogger* logger)  {
  s_logger = logger;
}

// Constructor
LHCb::OnlineMessageSvc::OnlineMessageSvc( const string& name, ISvcLocator* svcloc )
: Service( name, svcloc ) 
{
  m_defaultStream = &cout;
  m_outputLevel   = MSG::NIL;
  declareProperty( "Format",       m_defaultFormat = Message::getDefaultFormat() );
  declareProperty( "timeFormat",   m_defaultTimeFormat = Message::getDefaultTimeFormat() );
  declareProperty( "doPrintAlways",m_printAlways = true );
  declareProperty( "showStats",    m_stats = false );
  declareProperty( "statLevel",    m_statLevel = 0 );

  // Special properties to control output level of individual sources
  declareProperty( "setVerbose",  m_thresholdProp[MSG::VERBOSE] );
  declareProperty( "setDebug",    m_thresholdProp[MSG::DEBUG] );
  declareProperty( "setInfo",     m_thresholdProp[MSG::INFO] );
  declareProperty( "setWarning",  m_thresholdProp[MSG::WARNING] );
  declareProperty( "setError",    m_thresholdProp[MSG::ERROR] );
  declareProperty( "setFatal",    m_thresholdProp[MSG::FATAL] );
  declareProperty( "setAlways",   m_thresholdProp[MSG::ALWAYS] );
  declareProperty( "LoggerOnly",  m_loggerOnly = false );

  const int defaultLimit = 500;
  declareProperty( "fatalLimit",    m_msgLimit[MSG::FATAL]   = defaultLimit );
  declareProperty( "errorLimit",    m_msgLimit[MSG::ERROR]   = defaultLimit );
  declareProperty( "warningLimit",  m_msgLimit[MSG::WARNING] = defaultLimit );
  declareProperty( "infoLimit",     m_msgLimit[MSG::INFO]    = defaultLimit );
  declareProperty( "debugLimit",    m_msgLimit[MSG::DEBUG]   = defaultLimit );
  declareProperty( "verboseLimit",  m_msgLimit[MSG::VERBOSE] = defaultLimit );
  declareProperty( "alwaysLimit",   m_msgLimit[MSG::ALWAYS]  = 0 );

  declareProperty( "defaultLimit",  m_msgLimit[MSG::NIL]     = defaultLimit );
  declareProperty( "enableSuppression", m_suppress = false );
}


/// Initialize Service 
StatusCode LHCb::OnlineMessageSvc::initialize() {
  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  // Release pointer to myself done in Service base class
  if( m_messageSvc ) {
    m_messageSvc->release();
    m_messageSvc = 0;
  }
  // Set my own properties
  setProperties();
  
  // Reset message counters
  for(int i=0; i<MSG::NUM_LEVELS;++i) 
    m_msgCount[i] = 0;

  for (int ic=0; ic<MSG::NUM_LEVELS; ++ic) {
    //m_logColors[ic].declareUpdateHandler(&OnlineMessageSvc::setupColors, this);
    m_msgLimit[ic].declareUpdateHandler(&OnlineMessageSvc::setupLimits, this);
    m_thresholdProp[ic].declareUpdateHandler(&OnlineMessageSvc::setupThreshold, this);
  }
  return StatusCode::SUCCESS;
}

void LHCb::OnlineMessageSvc::setupLimits(Property& prop) {
  int ic = 0;
  if (prop.name() == "fatalLimit") {
    ic = MSG::FATAL;
  } else if (prop.name() == "errorLimit") {
    ic = MSG::ERROR;
  } else if (prop.name() == "warningLimit") {
    ic = MSG::WARNING;
  } else if (prop.name() == "infoLimit") {
    ic = MSG::INFO;
  } else if (prop.name() == "debugLimit") {
    ic = MSG::DEBUG;
  } else if (prop.name() == "verboseLimit") {
    ic = MSG::VERBOSE;
  } else if (prop.name() == "alwaysLimit") {
    IntegerProperty *p = dynamic_cast<IntegerProperty*>(&prop);
    if (p && p->value() != 0) {
      cout << "MessageSvc ERROR: cannot suppress ALWAYS messages" << endl;
      p->setValue(0);
    }
    ic = MSG::ALWAYS;
  } else if (prop.name() == "defaultLimit") {
    for (int i = MSG::VERBOSE; i< MSG::NUM_LEVELS; ++i) {
      if (i != MSG::ALWAYS) {
        m_msgLimit[i] = m_msgLimit[MSG::NIL].value();
      }
    }
  } else {
    cout << "MessageSvc ERROR: Unknown message limit parameter: "
         << prop.name() << endl;
    return;
  }

}

void LHCb::OnlineMessageSvc::setupThreshold(Property& prop) {
  int ic = 0;
  if (prop.name() == "setFatal") {
    ic = MSG::FATAL;
  } else if (prop.name() == "setError") {
    ic = MSG::ERROR;
  } else if (prop.name() == "setWarning") {
    ic = MSG::WARNING;
  } else if (prop.name() == "setInfo") {
    ic = MSG::INFO;
  } else if (prop.name() == "setDebug") {
    ic = MSG::DEBUG;
  } else if (prop.name() == "setVerbose") {
    ic = MSG::VERBOSE;
  } else if (prop.name() == "setAlways") {
    ic = MSG::ALWAYS;
  } else {
    cerr << "MessageSvc ERROR: Unknown message theshold parameter: "
         << prop.name() << endl;
    return;
  }

  StringArrayProperty *sap = dynamic_cast<StringArrayProperty*>( &prop);
  if (sap == 0) {
    cerr << "could not dcast " << prop.name()
              << " to a StringArrayProperty (which it should be!)" << endl;
    return;
  } else {
    vector<string>::const_iterator itr;
    for ( itr =  sap->value().begin();
          itr != sap->value().end();
          ++itr) {
      setOutputLevel( *itr, ic );
    }
  }

}

/// Finalize Service
StatusCode LHCb::OnlineMessageSvc::finalize() {
  return StatusCode::SUCCESS;
}

bool LHCb::OnlineMessageSvc::i_reportMessage(const Message& msg)  {
  bool reported = false;
  int key = msg.getType();
  StreamMap::const_iterator first = m_streamMap.lower_bound( key );
  if ( first != m_streamMap.end() ) {
    StreamMap::const_iterator last = m_streamMap.upper_bound( key );
    while( first != last ) {
      if ( !m_loggerOnly )  {
        ostream& stream = *( (*first).second.second );
        stream << ::lib_rtl_timestr() << msg << endl;
      }
      first++;
      reported = true;
    }
  }
  else if ( key >= outputLevel(msg.getSource()) )   {
    if ( !m_loggerOnly )  {
      msg.setFormat(m_defaultFormat);
      (*m_defaultStream) << ::lib_rtl_timestr() <<  msg << endl << flush;
    }
    reported = true;
  }
  return reported;
}

/// Implementation of IMessageSvc::reportMessage()
void LHCb::OnlineMessageSvc::reportMessage(const Message& msg, int lvl) {  
  bool report_always = m_printAlways;
  bool reported = i_reportMessage(msg);
  if ( lvl >= MSG::NIL && lvl <= MSG::NUM_LEVELS) ++m_msgCount[lvl];
  if ( lvl==MSG::ALWAYS && !report_always ) 
    return;
  else if ( reported )
    this->report(lvl, msg.getSource(), msg.getMessage());
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const Message& msg)  {
  reportMessage(msg,msg.getType());
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const char* src, int type, const char* msg)  {
  reportMessage( Message( src, type, msg) );
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const string& src, int type, const string& msg) {
  reportMessage( Message( src, type, msg) );
}

/// Find a message for a given status code and dispatch it
void LHCb::OnlineMessageSvc::reportMessage(const StatusCode& key, const string& source)  {  
  MessageMap::const_iterator first = m_messageMap.lower_bound( key );
  if ( first != m_messageMap.end() ) {
    MessageMap::const_iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      Message m = (*first).second;
      m.setSource( source );
      ostringstream os;
      os << "Status Code " << key.getCode() << ends;
      Message stat( source, m.getType(), os.str() );
      reportMessage( stat );
      reportMessage( m );
      first++;
    }
  }
  else {
    Message m = m_defaultMessage;
    m.setSource( source );
    ostringstream os;
    os << "Status Code " << key.getCode() << ends;
    Message stat( source,  m.getType(), os.str() );
    reportMessage( stat );
    reportMessage( m );
  }
}

/// Insert a stream for a message type.
void LHCb::OnlineMessageSvc::insertStream (int key,
                                     const string& name,
                                     ostream *stream)
{
  typedef StreamMap::value_type value_type;
  m_streamMap.insert( value_type( key, NamedStream(name,stream) ) );
}

/// Erase all the streams for all the message types.
void LHCb::OnlineMessageSvc::eraseStream()  {
  m_streamMap.erase( m_streamMap.begin(), m_streamMap.end() );
}

/// Erase all the streams for a message type.
void LHCb::OnlineMessageSvc::eraseStream( int message_type )  {
  m_streamMap.erase( message_type );
}

/// Erase one stream for a message type.
void LHCb::OnlineMessageSvc::eraseStream( int key, ostream* stream )   {
  if ( 0 != stream )    {
    bool changed = true;
    while( changed ) {
      changed = false;
      StreamMap::iterator first = m_streamMap.lower_bound( key );
      StreamMap::iterator last = m_streamMap.upper_bound( key );
      while( first != last ) {
        if ( (*first).second.second == stream ) {
          m_streamMap.erase( first );
          changed = true;
          break;
        }
      }
    }      
  }
}

/// Erase one stream for all message types.
void LHCb::OnlineMessageSvc::eraseStream( ostream* stream )    {
  if ( 0 != stream )    {
    bool changed = true;
    while( changed ) {
      changed = false;
      StreamMap::iterator first = m_streamMap.begin();
      while( first != m_streamMap.end() ) {
        if ( (*first).second.second == stream ) {
          m_streamMap.erase( first );
          changed = true;
          break;
        }
      }
    }      
  }
}

/// Insert a message for a status code.
void LHCb::OnlineMessageSvc::insertMessage( const StatusCode& key, const Message& msg ) {
  m_messageMap.insert( MessageMap::value_type( key, msg ) );
}

/// Erase all the messages for all the status codes.
void LHCb::OnlineMessageSvc::eraseMessage() {
  m_messageMap.erase( m_messageMap.begin(), m_messageMap.end() );
}

/// Erase all the messages for a status codes.
void LHCb::OnlineMessageSvc::eraseMessage( const StatusCode& key )  {
  m_messageMap.erase( key );
}

/// Erase one message for a status codes.
void LHCb::OnlineMessageSvc::eraseMessage( const StatusCode& key, const Message& msg )  {
  bool changed = true;
  while( changed ) {
    changed = false;
    MessageMap::iterator first = m_messageMap.lower_bound( key );
    MessageMap::iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      const Message& message = (*first).second;
      if ( message == msg ) {
        m_messageMap.erase( first );
        changed = true;
        break;
      }
    }      
  }
}

// ---------------------------------------------------------------------------
StatusCode LHCb::OnlineMessageSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IMessageSvc::interfaceID() == riid )  {
    *ppvInterface = (IMessageSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppvInterface);
}

// ---------------------------------------------------------------------------
int LHCb::OnlineMessageSvc::outputLevel()   const {
  return m_outputLevel;
}

// ---------------------------------------------------------------------------
int LHCb::OnlineMessageSvc::outputLevel( const string& source )   const {
  ThresholdMap::const_iterator it = m_thresholdMap.find( source );
  return it == m_thresholdMap.end() ? int(m_outputLevel) : (*it).second;
}

// ---------------------------------------------------------------------------
void LHCb::OnlineMessageSvc::setOutputLevel(int new_level)    {
  m_outputLevel = new_level;
}

// ---------------------------------------------------------------------------
void LHCb::OnlineMessageSvc::setOutputLevel(const string& source, int level)    {
  pair<ThresholdMap::iterator, bool> p;
  p = m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  if( p.second == false ) {
    // Already esisting an output level for that source. Erase an enter it again
    m_thresholdMap.erase ( p.first );
    m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  }
}

// ---------------------------------------------------------------------------
int LHCb::OnlineMessageSvc::messageCount( MSG::Level lvl ) const  {
  return (lvl >= MSG::NIL && lvl <= MSG::NUM_LEVELS) ? m_msgCount[lvl] : -1;
}
