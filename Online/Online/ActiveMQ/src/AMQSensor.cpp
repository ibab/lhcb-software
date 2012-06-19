// $Id: AMQSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/src/AMQSensor.cpp,v 1.1 2010/11/01 17:20:22 frankb Exp $

#include "ActiveMQ/ActiveMQSensor.h"
namespace ActiveMQ {
  template <class T> 
  ActiveMQSensor* createSensor(const std::string&, const std::string&, int) { return 0; }
}
#ifndef HAVE_ACTIVEMQ
namespace ActiveMQ { 
  class AMQSensor;
  template ActiveMQSensor* createSensor<AMQSensor>(const std::string&, const std::string&, int); 
}
#else

#include "ActiveMQ/AMQSensor.h"

#include "CPP/Event.h"
#include "CPP/Interactor.h"
#include "CPP/IocSensor.h"
#include "WT/wtdef.h"
#include <cstring>
#include <iostream>
#include <memory>

#include "activemq/core/ActiveMQConnectionFactory.h"
#include "activemq/core/ActiveMQConnection.h"
#include "activemq/library/ActiveMQCPP.h"
#include "cms/Topic.h"
#include "cms/Queue.h"
#include "cms/TemporaryTopic.h"
#include "cms/TemporaryQueue.h"
#include "cms/TextMessage.h"
#include "cms/BytesMessage.h"

using namespace std;
using namespace cms;
using namespace ActiveMQ;

namespace {
  union cmsEntry {
    const void*           ptr;
    const Destination*    dst;
    const Topic*          topic;
    const Queue*          queue;
    const TemporaryTopic* tmpTopic;
    const TemporaryQueue* tmpQueue;
    cmsEntry(const void* p) { ptr = p; }
  };
}

namespace ActiveMQ {
  /// Static AMQ sensor creator
  template <>
  ActiveMQSensor* createSensor<AMQSensor>(const std::string& name, const std::string& host, int) {
    return new AMQSensor(name,host);
  }
}

/// Initializing constructor
AMQSensor::AMQSensor(const string& nam, const string& host)
  : ActiveMQSensor(nam,host,0), m_clientAck(false), m_useTopic(true), m_transport(DEAD)
{
  activemq::library::ActiveMQCPP::initializeLibrary();
  connectServer();
}

/// Standad destructor
AMQSensor::~AMQSensor()  {
  disconnectServer();
  activemq::library::ActiveMQCPP::shutdownLibrary();
}

/// Connect to stomp server
int AMQSensor::connectServer()   {
  try {
    activemq::core::ActiveMQConnectionFactory fac(m_host);
    m_connection = fac.createConnection();
    activemq::core::ActiveMQConnection* amq = dynamic_cast<activemq::core::ActiveMQConnection*>(m_connection);
    if( 0 != amq ) {
      amq->addTransportListener( this );
    }
    m_connection->start();
    m_connection->setExceptionListener(this);
    // Create a Session
    m_session = m_connection->createSession(m_clientAck ? Session::CLIENT_ACKNOWLEDGE : Session::AUTO_ACKNOWLEDGE);
  }
  catch (CMSException& e) {
    e.printStackTrace();
  }
  return 1;
}

/// Disconnect from stomp server
int AMQSensor::disconnectServer()   {
  for(Consumers::iterator i=m_consumers.begin(); i!=m_consumers.end(); ++i)  {
    // Destroy resources.
    try  {
      if ( (*i).second.first ) delete (*i).second.first;
      if ( (*i).second.second ) delete (*i).second.second;
    }
    catch (CMSException& e) {
    }
    (*i).second.first = 0;
    (*i).second.second = 0;
  }
  m_consumers.clear();
  try  {
    if( 0 != m_session ) delete m_session;
    if( 0 != m_connection ) delete m_connection;
  }
  catch (CMSException& e) {
  }
  m_session = 0;
  m_connection = 0;
  return 1;
}

/// Connect to stomp service
int AMQSensor::connect()   {
  return 1;
}

/// Disconnect to stomp service
int AMQSensor::disconnect()   {
  return 1;
}  

/// Subscribe to data channel
int AMQSensor::subscribe(const string& chan)   {
  string channel = chan;
  if (chan.find("/topic/") == 0 || chan.find("/queue/") == 0) 
    channel = chan.substr(7);
  Consumers::iterator i = m_consumers.find(channel);
  if ( i == m_consumers.end() )  {
    try {
      Destination* d = m_useTopic 
	? (Destination*)m_session->createTopic(channel) 
	: (Destination*)m_session->createQueue(channel);
      MessageConsumer* c = m_session->createConsumer(d);
      c->setMessageListener(this);
      m_consumers.insert(make_pair(channel,make_pair(d,c)));
    }
    catch (CMSException& e) {
      e.printStackTrace();
      return 0;
    }
  }
  // We were already subscribed....
  return 1;
}

/// Unsubscribe to data channel
int AMQSensor::unsubscribe(const string& channel)   {
  Consumers::iterator i = m_consumers.find(channel);
  if ( i != m_consumers.end() )  {
    // Destroy resources.
    try  {
      if ( (*i).second.first ) delete (*i).second.first;
      if ( (*i).second.second ) delete (*i).second.second;
    }
    catch (CMSException& e) {
    }
    (*i).second.first = 0;
    (*i).second.second = 0;
    m_consumers.erase(i);
  }
  return 1;
}

/// Send data to the stomp service (push)
int AMQSensor::send(const string& channel, const string& data)  {
  if ( m_transport == ALIVE ) {
    try {
      Producers::iterator i = m_producers.find(channel);
      if ( i == m_producers.end() )  {
	// Create a MessageProducer from the Session to the Topic or Queue
	Destination* d = m_useTopic 
	  ? (Destination*)m_session->createTopic(channel) 
	  : (Destination*)m_session->createQueue(channel);
	MessageProducer* p = m_session->createProducer(d);
	p->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
	m_producers.insert(make_pair(channel,make_pair(d,p)));
	i = m_producers.find(channel);
      }
      if ( i != m_producers.end() )  {
	::lib_rtl_output(LIB_RTL_DEBUG,"AMQ> Send message: %s",channel.c_str());
	::lib_rtl_output(LIB_RTL_VERBOSE,"AMQ>  -> %s",data.c_str());
	auto_ptr<TextMessage> m(m_session->createTextMessage(data));
	(*i).second.second->send(m.get());
	return 1;
      }
    }
    catch ( CMSException& e ) {
      e.printStackTrace();
    }
    ::lib_rtl_output(LIB_RTL_ERROR,"AMQSensor> Failed to send message to:%s",channel.c_str());
    ::lib_rtl_output(LIB_RTL_ERROR,"AMQSensor> Thsi is FATAL and the process will exit in 10 seconds.");
    ::lib_rtl_sleep(10);
    ::exit(0);
  }
  return 0;
}

/// Sensor overload: Dispatch AMQ message to clients
void AMQSensor::dispatch(void* arg)  {
  cms::Message* msg = (cms::Message*)arg;
  cmsEntry d(msg->getCMSDestination());
  if ( d.dst ) {
    string dev_name;
    switch(d.dst->getDestinationType()) {
    case Destination::TOPIC:
      d.topic = dynamic_cast<const Topic*>(d.dst);
      dev_name = d.topic->getTopicName();
      break;
    case Destination::QUEUE:
      d.queue = dynamic_cast<const Queue*>(d.dst);
      dev_name = d.queue->getQueueName();
      break;
    case Destination::TEMPORARY_TOPIC:
      d.tmpTopic = dynamic_cast<const TemporaryTopic*>(d.dst);
      dev_name = d.tmpTopic->getTopicName();
      break;
    case Destination::TEMPORARY_QUEUE:
      d.tmpQueue = dynamic_cast<const TemporaryQueue*>(d.dst);
      dev_name = d.tmpQueue->getQueueName();
      break;
    default:
      ::lib_rtl_output(LIB_RTL_DEBUG,"AMQSensor> Cannot dispatch unknown destination type:%d",
		       d.dst->getDestinationType());
      return;
    }
    ::lib_rtl_output(LIB_RTL_VERBOSE,"AMQSensor> Dispatch topic:%s",
		     dev_name.c_str());
    InteractorTable::iterator i = m_clients.find(dev_name);
    if ( i != m_clients.end() )  {
      string txt;
      Clients& cl = (*i).second;
      const TextMessage* t=0;
      const BytesMessage* b=0;
      if      ( 0 != (t=dynamic_cast<const TextMessage*>(msg)) )   {
	txt = t->getText();
      }
      else if ( 0 != (b=dynamic_cast<const BytesMessage*>(msg)) )  {
	std::auto_ptr<char> buff(new char[b->getBodyLength()+1]);
	::memcpy(buff.get(),b->getBodyBytes(),b->getBodyLength());
	buff.get()[b->getBodyLength()] = 0;
	txt = buff.get();
      }
      if ( !txt.empty() ) {
	for(Clients::iterator j=cl.begin(); j!=cl.end(); ++j)  {
	  Event ev(*j,NetEvent);
	  ev.message    = (::Message*)msg;
	  ev.source     = (::Address*)d.dst;
	  ev.type       = d.dst->getDestinationType();
	  ev.device     = (char*)dev_name.c_str();
	  ev.data       = (void*)this;
	  ev.buffer_ptr = (char*)txt.c_str();
	  ev.buffer_len = txt.length();
	  ev.target->handle(ev);       
	}
      }
    }
  }
}

/// Registered callback as an ExceptionListener with the connection in case bad things occur
void AMQSensor::onException( const CMSException& ex AMQCPP_UNUSED ) {
  ::printf("CMS Exception occurred.  Shutting down client.\n");
  exit(1);
}

/// Callback when the transport layer is broken
void AMQSensor::transportInterrupted() {
  ::lib_rtl_output(LIB_RTL_ERROR,"AMQSensor> The Connection's Transport has been Interrupted.");
  m_transport = DEAD;
}

/// Callback when the transport layer has beeen restored
void AMQSensor::transportResumed() {
  ::lib_rtl_output(LIB_RTL_ERROR,"AMQSensor> The Connection's Transport has been Restored.");
  m_transport = ALIVE;
}

/// Called from the consumer since this class is a registered MessageListener.
void AMQSensor::onMessage(const cms::Message* msg){
  try    {
    if( 0 != dynamic_cast<const TextMessage*>(msg) )
      this->dispatch((void*)msg); // ::wtc_insert(m_facility,msg->clone());
    else if( 0 != dynamic_cast<const BytesMessage*>(msg) )
      this->dispatch((void*)msg); // ::wtc_insert(m_facility,msg->clone());
    else
      ::lib_rtl_output(LIB_RTL_ERROR,"AMQSensor> received a message with unknown format!");

    if( m_clientAck ) msg->acknowledge();
  }
  catch (CMSException& e) {
    e.printStackTrace();
  }
}

/// Poll for messages....
int AMQSensor::execute()  {
  IocSensor::instance().run();
  return 1;
}
#endif
