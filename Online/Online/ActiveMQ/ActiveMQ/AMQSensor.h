// $Id: AMQSensor.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
//====================================================================
//  Comet
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/AMQSensor.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_STOMSERVICE_H
#define ACTIVEMQ_STOMSERVICE_H

#include "Config.h"
#include "ActiveMQ/ActiveMQSensor.h"
#include "RTL/rtl.h"

// C++ include files
#include <string>
#include <vector>
#include <map>

// Forward declarations
struct apr_pool_t;

#include "activemq/transport/DefaultTransportListener.h"
#include "cms/ExceptionListener.h"
#include "cms/MessageListener.h"

namespace cms {
  class Message;
  class Session;
  class CMSException;
  class Destination;
  class Connection;
  class MessageConsumer;
  class MessageProducer;
}

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ  {

  // Forward declarations
  class AMQSensor;

  /**@class DeliveryHandler DeliveryHandler.h Stomp/DeliveryHandler.h
   *
   * @author M.Frank
   */
  class AMQSensor
    : public ActiveMQSensor, 
    public cms::MessageListener,
    public cms::ExceptionListener,
    public activemq::transport::DefaultTransportListener
    {
  public:
      enum TransportStatus { DEAD = 0, ALIVE = 1 };
  protected:
    typedef std::map<std::string,std::pair<cms::Destination*,cms::MessageConsumer*> > Consumers;
    typedef std::map<std::string,std::pair<cms::Destination*,cms::MessageProducer*> > Producers;

    /// Flag to be set when stopping the service
    bool             m_clientAck;
    bool             m_useTopic;

    cms::Session*    m_session;
    cms::Connection* m_connection;
    Consumers        m_consumers;
    Producers        m_producers;
    TransportStatus  m_transport;

    /// Send command to stomp server
    int _command(const std::string& cmd, const std::string& channel, const void* body, int len);

  public:
    /// Initializing constructor
    AMQSensor(const std::string& name, const std::string& host);
    /// Standad destructor
    virtual ~AMQSensor();

    /// Set flag to stop polling thread and allow shutdown
    void stop()  {};

    /// Connect to stomp server
    int connectServer();

    /// Disconnect from stomp server
    int disconnectServer();

    /// Connect to stomp server
    int connect();
    /// Disconnect from stomp service
    int disconnect();
    /// Send data to the stomp service (push)
    virtual int send(const std::string& destination, const std::string& data);
    /// Subscribe to data channel
    virtual int subscribe(const std::string& channel);
    /// Unsubscribe from data channel
    virtual int unsubscribe(const std::string& channel);

    /// Poll for messages....
    virtual int execute();

    /// Sensor overload: Dispatch AMQ message to clients
    virtual void dispatch( void* arg );

    /// cms::MessageListener overload: Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage(const cms::Message* message);

    /// cms::ExceptionListener overload: Registered callback as an ExceptionListener with the connection in case bad things occur
    virtual void onException(const cms::CMSException& ex);
    /// activemq::transport::DefaultTransportListener overload: Callback when the transport layer is broken
    virtual void transportInterrupted();
    /// activemq::transport::DefaultTransportListener overload: Callback when the transport layer has beeen restored
    virtual void transportResumed();

  };
}      // End namespace ActiveMQ
#endif // ACTIVEMQ_STOMSERVICE_H
