// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/OnlineMessageSvc.h,v 1.4 2006-03-17 07:48:49 frankb Exp $
#ifndef GAUDI_ONLINEMESSAGESVC_H
#define GAUDI_ONLINEMESSAGESVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/IMessageSvc.h"

// Include files
#include <sstream>
#include <string>
#include <vector>
#include <map>

// Forward declarations
class ISvcLocator;
class DimService;

//
// ClassName:   MessageSvc 
//  
// Description: The MessageSvc service implements the IMessageSvc interface anbd provides the
//              basic messaging needed by batch oriented applications. 
//
// Author:      Iain Last
//
#define SRCSIZE 256
class OnlineDimMessage// : public Message
{
public:
	OnlineDimMessage();
	OnlineDimMessage(const Message& message);
	virtual ~OnlineDimMessage();
	void * makeDimBuffer (const Message& message);
	char *getDimBuffer (){return m_DimBuffer;};
	int getBufferSize() {return m_msize;};
	void deleteDimBuffer ();
private:
	char *m_DimBuffer;
	int m_msize;
	int m_allocs;
};
class OnlineMessageSvc : public Service,
                   virtual public IMessageSvc { 
public:
  typedef std::pair< std::string, std::ostream* > NamedStream;
  typedef std::multimap< int, NamedStream > StreamMap;
  typedef std::multimap< StatusCode, Message > MessageMap;
  typedef std::map< std::string, int > ThresholdMap;
  
  // Default constructor.
  OnlineMessageSvc( const std::string& name, ISvcLocator* svcloc );
  // Destructor.
  virtual ~OnlineMessageSvc() {};

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

  // Implementation of IInterface::queryInterface()
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  // Implementation of IMessageSvc::useColor()
  virtual bool useColor() const { return false; }

  // Implementation of IMessageSvc::getLogColor()
  virtual std::string getLogColor(int /* logLevel */) const {  return ""; };
  

private:
  std::ostream* m_defaultStream;      ///< Pointer to the output stream.
  Message m_defaultMessage;           ///< Default Message
  StreamMap m_streamMap;              ///< Stream map
  MessageMap m_messageMap;            ///< Message map
  ThresholdMap m_thresholdMap;        ///< Output level threshold map
  std::string m_defaultFormat;        ///< Default format for the messages
  std::vector<std::string> m_thresholdProp[MSG::NUM_LEVELS]; ///< Properties controling
  bool                                      m_useDIM;
  DimService                               *m_LogSrvc;
  OnlineDimMessage                          m_mess;
  bool                                      m_inited;
};

#endif
