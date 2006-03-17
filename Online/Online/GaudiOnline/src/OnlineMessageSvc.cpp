// $Id: OnlineMessageSvc.cpp,v 1.3 2006-03-17 07:33:06 frankb Exp $

#include "GaudiKernel/SvcFactory.h"
#include "OnlineMessageSvc.h"
#include <cstdlib>
#include "dis.hxx"

DECLARE_SERVICE_FACTORY(OnlineMessageSvc);

OnlineDimMessage::OnlineDimMessage()  {
  m_msize = 0;
  m_allocs = 0;
  m_DimBuffer = 0;
}
OnlineDimMessage::OnlineDimMessage(const Message& message)  {
  m_DimBuffer = 0;
  m_msize = 0;
  m_allocs = 0;
  makeDimBuffer(message);
}
OnlineDimMessage::~OnlineDimMessage()   {
  if (m_DimBuffer != 0)  {
    free(m_DimBuffer);
    m_DimBuffer = 0;
    m_msize = 0;
    m_allocs = 0;
  }
}

void * OnlineDimMessage::makeDimBuffer (const Message& message) {
  int sizneed;
  const char *msg = message.getMessage().c_str();
  int msiz = strlen(msg);
  sizneed = sizeof(int)+SRCSIZE+msiz+1;
  if (m_allocs < sizneed)  {
    free (m_DimBuffer);
    m_allocs = sizneed;
    m_DimBuffer = (char*)malloc(m_allocs);
  }
  m_msize = sizneed;
  char *cPtr = m_DimBuffer;
  *(int*)cPtr	= message.getType();
  cPtr +=4;
  const char *src = message.getSource().c_str();
  int ssiz = strlen(src);
  if (ssiz >= SRCSIZE)
  {
    memcpy(cPtr,src,SRCSIZE-1);
    cPtr[SRCSIZE] = 0;
  }
  else  {
    memcpy(cPtr,src,ssiz);
    cPtr[ssiz]=0;
  }
  cPtr += SRCSIZE;
  memcpy(cPtr, msg, msiz);
  cPtr[msiz] = 0;
  return m_DimBuffer;
}

void OnlineDimMessage::deleteDimBuffer () {
  free(m_DimBuffer);
  m_DimBuffer = 0;
  m_allocs = 0;
  m_msize = 0;

}

// Constructor
OnlineMessageSvc::OnlineMessageSvc( const std::string& name, ISvcLocator* svcloc )
: Service( name, svcloc ) 
{
  m_defaultStream = &std::cout;
  m_outputLevel   = MSG::NIL;
  m_useDIM = false;
  std::cout << "============ Online MessageService constructed" << std::endl;
  declareProperty( "Format",      m_defaultFormat = "% F%18W%S%7W%R%T %0W%M");

  // Special properties to control output level of individual sources
  declareProperty( "setVerbose",  m_thresholdProp[MSG::VERBOSE] );
  declareProperty( "setDebug",    m_thresholdProp[MSG::DEBUG] );
  declareProperty( "setInfo",     m_thresholdProp[MSG::INFO] );
  declareProperty( "setWarning",  m_thresholdProp[MSG::WARNING] );
  declareProperty( "setError",    m_thresholdProp[MSG::ERROR] );
  declareProperty( "setFatal",    m_thresholdProp[MSG::FATAL] );
  declareProperty( "setAlways",   m_thresholdProp[MSG::ALWAYS] );
  declareProperty( "useDIM",      m_useDIM = true);
  m_LogSrvc = 0;
  m_inited = false;
}


/// Initialize Service 
StatusCode OnlineMessageSvc::initialize() {
  if (m_inited)	{
    return StatusCode::SUCCESS;
  }

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  // Release pointer to myself done in Service base class
  if( m_messageSvc ) {
    m_messageSvc->release();
    m_messageSvc = 0;
  }
  // Set my own properties
  setProperties();

  // Set the output level thresholds from properties
  for ( int lvl = MSG::VERBOSE; lvl < MSG::NUM_LEVELS; lvl++ ) {
    std::vector<std::string>& thresholds = m_thresholdProp[lvl];
    for ( std::vector<std::string>::iterator it = thresholds.begin();
      it != thresholds.end(); it++) {
        setOutputLevel( *it, lvl );
      }
  }
  if (m_useDIM)  {
    std::string delimiter("/");
    std::string task = getenv("UTGID") ? getenv("UTGID") : "DefaultTask";
    std::string node = getenv("NODE") ? getenv("NODE") : "DefaultNode";
    std::string Srvc("DIMLogging");

    std::string srvcname = delimiter + node + delimiter + task + delimiter + Srvc;
    Message m(srvcname,MSG::INFO,"DIM Service Starting up");
    m_mess.makeDimBuffer(m);
    const char *svcnam = srvcname.c_str();
    const char *buf = m_mess.getDimBuffer();
    int bsiz = m_mess.getBufferSize();
    m_LogSrvc = new DimService(svcnam, "I:1;C:256;C",(void*)buf, bsiz);
  }
  m_inited = true;
  return StatusCode::SUCCESS;
}

/// Finalize Service
StatusCode OnlineMessageSvc::finalize() {
  if (m_useDIM)  {
    delete m_LogSrvc;
    m_LogSrvc = 0;
  }
  return StatusCode::SUCCESS;
}

// ---------------------------------------------------------------------------
// Routine: reportMessage
// Purpose: dispatches a message to the relevant streams.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::reportMessage( const Message& msg )    {
  if (!m_inited)
  {
    initialize();
  }
  int key = msg.getType();
  StreamMap::const_iterator first = m_streamMap.lower_bound( key );
  if ( first != m_streamMap.end() ) {
    StreamMap::const_iterator last = m_streamMap.upper_bound( key );
    while( first != last ) {
      std::ostream& stream = *( (*first).second.second );
      stream << msg << std::endl;
      first++;
    }
  }
  else if ( key >= outputLevel(msg.getSource()) )   {
    msg.setFormat(m_defaultFormat);
    (*m_defaultStream) <<  msg << std::endl << std::flush;
  }
  if (m_useDIM)
  {
    m_mess.makeDimBuffer(msg);
    char *outstr= m_mess.getDimBuffer();
    int oms = m_mess.getBufferSize();
    m_LogSrvc->updateService(outstr,oms); 
  }
}

// ---------------------------------------------------------------------------
// Routine: reportMessage
// Purpose: dispatches a message to the relevant streams.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::reportMessage (const char* source,
                                      int type,
                                      const char* message) 
{
  if (!m_inited)  {
    initialize();
  }
  Message msg( source, type, message);
  reportMessage( msg );
}

// ---------------------------------------------------------------------------
// Routine: reportMessage
// Purpose: dispatches a message to the relevant streams.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::reportMessage (const std::string& source,
                                      int type,
                                      const std::string& message) 
{
  if (!m_inited)  {
    initialize();
  }
  Message msg( source, type, message);
  reportMessage( msg );
}

// ---------------------------------------------------------------------------
// Routine: sendMessage
// Purpose: finds a message for a given status code and dispatches it.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::reportMessage (const StatusCode& key,
                                      const std::string& source)
{  
  if (!m_inited)  {
    initialize();
  }
  MessageMap::const_iterator first = m_messageMap.lower_bound( key );
  if ( first != m_messageMap.end() ) {
    MessageMap::const_iterator last = m_messageMap.upper_bound( key );
    while( first != last ) {
      Message msg = (*first).second;
      msg.setSource( source );
      std::ostringstream os1;
      os1 << "Status Code " << key.getCode() << std::ends;
      Message stat_code1( source, msg.getType(), os1.str() );
      reportMessage( stat_code1 );
      reportMessage( msg );
      first++;
    }
  }
  else {
    Message mesg = m_defaultMessage;
    mesg.setSource( source );
    std::ostringstream os2;
    os2 << "Status Code " << key.getCode() << std::ends;
    Message stat_code2( source,  mesg.getType(), os2.str() );
    reportMessage( stat_code2 );
    reportMessage( mesg );
  }
}

// ---------------------------------------------------------------------------
// Routine: insertStream
// Purpose: inserts a stream for a message type.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::insertStream (int key,
                                     const std::string& name,
                                     std::ostream *stream)
{
  typedef StreamMap::value_type value_type;
  m_streamMap.insert( value_type( key, NamedStream(name,stream) ) );
}

// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases all the streams for all the message types.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseStream()  {
  m_streamMap.erase( m_streamMap.begin(), m_streamMap.end() );
}

// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases all the streams for a message type.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseStream( int message_type )  {
  m_streamMap.erase( message_type );
}

// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases one stream for a message type.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseStream( int key, std::ostream* stream )   {
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

// ---------------------------------------------------------------------------
// Routine: eraseStream
// Purpose: erases one stream for all message types.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseStream( std::ostream* stream )    {
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

// ---------------------------------------------------------------------------
// Routine: insertMessage
// Purpose: inserts a message for a status code.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::insertMessage( const StatusCode& key, const Message& msg )
{
  typedef MessageMap::value_type value_type;
  m_messageMap.insert( value_type( key, msg ) );
}

// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases all the messages for all the status codes.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseMessage()
{
  m_messageMap.erase( m_messageMap.begin(), m_messageMap.end() );
}

// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases all the messages for a status code.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseMessage( const StatusCode& key )  {
  m_messageMap.erase( key );
}

// ---------------------------------------------------------------------------
// Routine: eraseMessage
// Purpose: erases one message for a status code.
// ---------------------------------------------------------------------------
void OnlineMessageSvc::eraseMessage( const StatusCode& key, const Message& msg )
{
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
StatusCode OnlineMessageSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IID_IMessageSvc == riid )  {
    *ppvInterface = (IMessageSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid, ppvInterface);
}

// ---------------------------------------------------------------------------
int OnlineMessageSvc::outputLevel()   const {
  return m_outputLevel;
}

// ---------------------------------------------------------------------------
int OnlineMessageSvc::outputLevel( const std::string& source )   const {
  ThresholdMap::const_iterator it = m_thresholdMap.find( source );
  return (it == m_thresholdMap.end()) ? m_outputLevel : (*it).second;
}

// ---------------------------------------------------------------------------
void OnlineMessageSvc::setOutputLevel(int new_level)    {
  m_outputLevel = new_level;
}

// ---------------------------------------------------------------------------
void OnlineMessageSvc::setOutputLevel(const std::string& source, int level)    {
  std::pair<ThresholdMap::iterator, bool> p;
  p = m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  if( p.second == false ) {
    // Already esisting an output level for that source. Erase an enter it again
    m_thresholdMap.erase ( p.first );
    m_thresholdMap.insert(ThresholdMap::value_type( source, level) );
  }
}
