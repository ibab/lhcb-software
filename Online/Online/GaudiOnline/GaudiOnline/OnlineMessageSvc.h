// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/OnlineMessageSvc.h,v 1.8 2008-10-21 16:15:15 frankb Exp $
#ifndef GAUDI_ONLINEMESSAGESVC_H
#define GAUDI_ONLINEMESSAGESVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IMessageSvc.h"

// Include files
#include <vector>
#include <map>

// Forward declarations
class ISvcLocator;
class IErrorLogger;

/*
 * LHCb namespace declaration
 */
namespace LHCb {

  /**@class OnlineMessageSvc OnlineMessageSvc.h gaudiOnline/OnlineMessageSvc.h
    *
    * Description: The OnlineMessageSvc service implements the IMessageSvc 
    *              interface and provides the forwarding of the messages
    *              to a DIM based error logger as used in the LHCb online 
    *              system.
    *
    * Author:      B.Jost / M.Frank
    */
  class OnlineMessageSvc 
    : public Service, 
    virtual public IMessageSvc,
    virtual public IErrorLogger
    { 
  public:
    typedef std::pair< std::string, std::ostream* > NamedStream;
    typedef std::multimap< int, NamedStream >       StreamMap;
    typedef std::multimap< StatusCode, Message >    MessageMap;
    typedef std::map< std::string, int >            ThresholdMap;
    
    // Default constructor.
    OnlineMessageSvc( const std::string& name, ISvcLocator* svcloc );
    // Destructor.
    virtual ~OnlineMessageSvc() {};

    // Implementation of IInterface::queryInterface()
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

    // Implementation of IService::initialize()
    virtual StatusCode initialize();

    // Implementation of IService::finalize()
    virtual StatusCode finalize();

    // Implementation of IMessageSvc::reportMessage()
    virtual void reportMessage( const Message& message );

    // Implementation of IMessageSvc::reportMessage()
    virtual void reportMessage( const StatusCode& code, const std::string& source = "");

    // Implementation of IMessageSvc::reportMessage()
    virtual void reportMessage( const char* source, int type, const char* message);

    /// Implementation of IMessageSvc::reportMessage()
    virtual void reportMessage(const Message& message, int outputLevel);

    // Implementation of IMessageSvc::reportMessage()
    virtual void reportMessage( const std::string& source, int type, const std::string& message);
    
    // Implementation of IMessageSvc::insertMessage()
    virtual void insertMessage( const StatusCode& code, const Message& message );

    // Implementation of IMessageSvc::eraseMessage()
    virtual void eraseMessage();

    // Implementation of IMessageSvc::eraseMessage()
    virtual void eraseMessage( const StatusCode& code ) ;

    // Implementation of IMessageSvc::eraseMessage()
    virtual void eraseMessage( const StatusCode& code, const Message& message );

    // Implementation of IMessageSvc::insertStream()
    virtual void insertStream( int message_type, const std::string& name, std::ostream* stream );
    
    // Implementation of IMessageSvc::eraseStream()
    virtual void eraseStream();
    
    // Implementation of IMessageSvc::eraseStream()
    virtual void eraseStream( int message_type );
    
    // Implementation of IMessageSvc::eraseStream()
    virtual void eraseStream( int message_type, std::ostream* stream );

    // Implementation of IMessageSvc::eraseStream()
    virtual void eraseStream( std::ostream* stream );

    // Implementation of IMessageSvc::desaultStream()
    virtual std::ostream* defaultStream() const { 
      return m_defaultStream; 
    }

    // Implementation of IMessageSvc::setDefaultStream()
    virtual void setDefaultStream( std::ostream* stream ) { 
      m_defaultStream = stream;
    }

    // Implementation of IMessageSvc::ouputLevel()
    virtual int outputLevel()   const;

    // Implementation of IMessageSvc::ouputLevel()
    virtual int outputLevel(const std::string& source)   const;

    // Implementation of IMessageSvc::setOuputLevel()
    virtual void setOutputLevel(int new_level);

    // Implementation of IMessageSvc::setOuputLevel()
    virtual void setOutputLevel(const std::string& source, int new_level);

    // Implementation of IMessageSvc::useColor()
    virtual bool useColor() const { return false; }

    // Implementation of IMessageSvc::getLogColor()
    virtual std::string getLogColor(int /* logLevel */) const {  return ""; };

    /// Get the number of messages issued at a particular level
    virtual int messageCount( MSG::Level level ) const;

  protected:
    virtual bool i_reportMessage( const Message& message );
    static void setErrorLogger(IErrorLogger* logger);
    void setupLimits(Property& prop);
    void setupThreshold(Property& prop);

  protected:

    bool          m_loggerOnly;                 ///< Property to suppress local logging
    int           m_msgCount[MSG::NUM_LEVELS];  ///< Counter of messages of a given type
    std::ostream* m_defaultStream;              ///< Pointer to the output stream.
    Message       m_defaultMessage;             ///< Default Message
    StreamMap     m_streamMap;                  ///< Stream map
    MessageMap    m_messageMap;                 ///< Message map
    ThresholdMap  m_thresholdMap;               ///< Output level threshold map
    std::string   m_defaultFormat;              ///< Default format for the messages
    std::string   m_defaultTimeFormat;          ///< Default format for timestamps in the messages
    StringArrayProperty     m_thresholdProp[MSG::NUM_LEVELS]; ///< Properties controling
    BooleanProperty         m_printAlways;
    BooleanProperty         m_stats;
    UnsignedIntegerProperty m_statLevel;
    IntegerProperty         m_msgLimit[MSG::NUM_LEVELS];
    BooleanProperty         m_suppress;
 };
} // End namespace LHCb
#endif
