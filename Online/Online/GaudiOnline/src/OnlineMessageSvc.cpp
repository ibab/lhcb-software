// $Id: OnlineMessageSvc.cpp,v 1.11 2008-06-05 18:40:48 frankb Exp $

#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/IErrorLogger.h"
#include "GaudiOnline/OnlineMessageSvc.h"
#include "RTL/rtl.h"
#include <sstream>

//DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineMessageSvc);

static IErrorLogger* s_logger = 0;
void LHCb::OnlineMessageSvc::setErrorLogger(IErrorLogger* logger)  {
  s_logger = logger;
}

// Constructor
LHCb::OnlineMessageSvc::OnlineMessageSvc( const std::string& name, ISvcLocator* svcloc )
: Service( name, svcloc ) 
{
  m_defaultStream = &std::cout;
  m_outputLevel   = MSG::NIL;
  declareProperty( "Format",      m_defaultFormat = "% F%18W%S%7W%R%T %0W%M");
  // Special properties to control output level of individual sources
  declareProperty( "setVerbose",  m_thresholdProp[MSG::VERBOSE] );
  declareProperty( "setDebug",    m_thresholdProp[MSG::DEBUG] );
  declareProperty( "setInfo",     m_thresholdProp[MSG::INFO] );
  declareProperty( "setWarning",  m_thresholdProp[MSG::WARNING] );
  declareProperty( "setError",    m_thresholdProp[MSG::ERROR] );
  declareProperty( "setFatal",    m_thresholdProp[MSG::FATAL] );
  declareProperty( "setAlways",   m_thresholdProp[MSG::ALWAYS] );
  declareProperty( "LoggerOnly",  m_loggerOnly = false );
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

  // Set the output level thresholds from properties
  for ( int lvl = MSG::VERBOSE; lvl < MSG::NUM_LEVELS; lvl++ ) {
    std::vector<std::string>& thresholds = m_thresholdProp[lvl];
    for ( std::vector<std::string>::iterator it = thresholds.begin();
      it != thresholds.end(); it++) {
        setOutputLevel( *it, lvl );
    }
  }
  return StatusCode::SUCCESS;
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
        std::ostream& stream = *( (*first).second.second );
        stream << ::lib_rtl_timestr() << msg << std::endl;
      }
      first++;
      reported = true;
    }
  }
  else if ( key >= outputLevel(msg.getSource()) )   {
    if ( !m_loggerOnly )  {
      msg.setFormat(m_defaultFormat);
      (*m_defaultStream) << ::lib_rtl_timestr() <<  msg << std::endl << std::flush;
    }
    reported = true;
  }
  return reported;
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const Message& msg)  {
  bool reported = i_reportMessage(msg);
  int lvl = msg.getType();
  if (lvl >= MSG::NIL && lvl <= MSG::NUM_LEVELS) ++m_msgCount[lvl];
  if ( reported && s_logger )  {
    //s_logger->report(msg.getType(), msg.getSource(), msg.getMessage());
    this->report(msg.getType(), msg.getSource(), msg.getMessage());
  }
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const char* src, int type, const char* msg)  {
  reportMessage( Message( src, type, msg) );
}

/// Dispatch a message to the relevant streams.
void LHCb::OnlineMessageSvc::reportMessage(const std::string& src, int type, const std::string& msg) {
  reportMessage( Message( src, type, msg) );
}

/// Find a message for a given status code and dispatch it
void LHCb::OnlineMessageSvc::reportMessage(const StatusCode& key, const std::string& source)  {  
  MessageMap::const_iterator first = m_messageMap.lower_bound( key );
  if ( first != m_messageMap.end() ) {
    MessageMap::const_iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      Message m = (*first).second;
      m.setSource( source );
      std::ostringstream os;
      os << "Status Code " << key.getCode() << std::ends;
      Message stat( source, m.getType(), os.str() );
      reportMessage( stat );
      reportMessage( m );
      first++;
    }
  }
  else {
    Message m = m_defaultMessage;
    m.setSource( source );
    std::ostringstream os;
    os << "Status Code " << key.getCode() << std::ends;
    Message stat( source,  m.getType(), os.str() );
    reportMessage( stat );
    reportMessage( m );
  }
}

/// Insert a stream for a message type.
void LHCb::OnlineMessageSvc::insertStream (int key,
                                     const std::string& name,
                                     std::ostream *stream)
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
void LHCb::OnlineMessageSvc::eraseStream( int key, std::ostream* stream )   {
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
void LHCb::OnlineMessageSvc::eraseStream( std::ostream* stream )    {
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
  if ( IID_IMessageSvc == riid )  {
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
int LHCb::OnlineMessageSvc::outputLevel( const std::string& source )   const {
  ThresholdMap::const_iterator it = m_thresholdMap.find( source );
  return it == m_thresholdMap.end() ? int(m_outputLevel) : (*it).second;
}

// ---------------------------------------------------------------------------
void LHCb::OnlineMessageSvc::setOutputLevel(int new_level)    {
  m_outputLevel = new_level;
}

// ---------------------------------------------------------------------------
void LHCb::OnlineMessageSvc::setOutputLevel(const std::string& source, int level)    {
  std::pair<ThresholdMap::iterator, bool> p;
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
