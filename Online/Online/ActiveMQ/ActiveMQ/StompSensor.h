// $Id: StompSensor.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/StompSensor.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
#ifndef ACTIVEMQ_STOMSERVICE_H
#define ACTIVEMQ_STOMSERVICE_H

#include "ActiveMQ/ActiveMQSensor.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"

// C++ include files
#include <string>
#include <vector>
#include <map>

// Forward declarations
struct apr_pool_t;
struct stomp_connection;

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ  {

  // Forward declarations
  class StompSensor;

  /**@class DeliveryHandler DeliveryHandler.h ActiveMQ/DeliveryHandler.h
   *
   * @author M.Frank
   */
  class StompSensor : public ActiveMQSensor, public Interactor   {
  public:
    struct Frame {
      const char *command;
      void *headers;
      const char *body;
      int body_length;
    };
    struct Message {
      apr_pool_t *pool;
      Frame *frame;
      StompSensor* sensor;
      Message(StompSensor* s) : pool(0), frame(0), sensor(s) {}
      ~Message();
      int read();
      const char* channel() const;
    };
  protected:
    /// APR memory pool
    apr_pool_t *m_pool;
    /// Reference to the stomp data sink connection
    stomp_connection *m_con;

    /// Report message to output device
    int report(int exitCode, const char *message, int reason);

    /// Send command to stomp server
    int _command(const std::string& cmd, const std::string& channel, const char* body, size_t len);

  public:
    /// Initializing constructor
    StompSensor(const std::string& name, const std::string& host, int port);
    /// Standad destructor
    virtual ~StompSensor();

    /// Destry pool e.g. after reading messages
    void destroyPool(apr_pool_t*& pool);

    /// Connect to stomp server
    virtual int connectServer();
    /// Disconnect from stomp server
    virtual int disconnectServer();

    /// Connect to stomp server
    virtual int connect();
    /// Disconnect from stomp service
    virtual int disconnect();
    /// Poll for messages....
    virtual int execute();


    /// Read pending data from channel
    int read(Frame*& frame, apr_pool_t*& pool);
    /// Send data to the stomp service (push)
    int send(const std::string& destination, const std::string& data);
    /// Subscribe to data channel
    int subscribe(const std::string& channel);
    /// Unsubscribe from data channel
    int unsubscribe(const std::string& channel);

    /// Sensor overload: Dispatch Stomp message to clients
    virtual void dispatch( void* arg );

    /// interactor overload: Handle external interrupts
    virtual void handle(const Event& ev);
  };
}      // End namespace ActiveMQ
#endif // ACTIVEMQ_STOMSERVICE_H
