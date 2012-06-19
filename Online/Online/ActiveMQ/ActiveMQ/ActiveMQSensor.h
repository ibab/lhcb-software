// $Id: ActiveMQSensor.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/ActiveMQSensor.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_ACTIVEMQSENSOR_H
#define ACTIVEMQ_ACTIVEMQSENSOR_H

#include "Config.h"
#include "CPP/Sensor.h"
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
  class ActiveMQSensor;

  /**@class DeliveryHandler DeliveryHandler.h Stomp/DeliveryHandler.h
   *
   * @author M.Frank
   */
  class ActiveMQSensor : public Sensor   {
  public:
    typedef std::vector<Interactor*>       Clients;
    typedef std::map<std::string, Clients> InteractorTable;
  protected:
    /// Service name
    std::string m_name;
    /// Host name of the stomp data sink connection
    std::string m_host;
    /// Stomp port number for data sink connection
    int         m_port;
    /// Client table
    InteractorTable   m_clients;
    /// Thread handle
    lib_rtl_thread_t  m_thread;
    /// Flag to be set when stopping the service
    bool        m_stop;

    /// Poll thread start routine
    static int _exec(void* arg);

  public:
    /// Initializing constructor
    ActiveMQSensor(const std::string& name, const std::string& host, int port);
    /// Standad destructor
    virtual ~ActiveMQSensor();
    /// Sensor overload: Add interactor to sensor. Dispatch criteria is topic name
    virtual void add( Interactor* client, void* topic);
    /// Sensor overload: Remove interactor from sensor. Dispatch criteria is topic name
    virtual void remove( Interactor* client, void* topic);
    /// Start polling for messages....
    int start();
    /// Set flag to stop polling thread and allow shutdown
    void stop()  {   m_stop = true; };


    /// Connect to stomp server
    virtual int connectServer() = 0;
    /// Disconnect from stomp server
    virtual int disconnectServer() = 0;
    /// Connect to stomp server
    virtual int connect() = 0;
    /// Disconnect from stomp service
    virtual int disconnect() = 0;
    /// Poll for messages....
    virtual int execute() = 0;
    /// Send data to the stomp service (push)
    virtual int send(const std::string& destination, const std::string& data) = 0;
    /// Subscribe to data channel
    virtual int subscribe(const std::string& channel) = 0;
    /// Unsubscribe from data channel
    virtual int unsubscribe(const std::string& channel) = 0;
  };

  /// Static sensor creator
  template <class T> 
    ActiveMQSensor* createSensor(const std::string&, const std::string&, int port=0);

}      // End namespace ActiveMQ
#endif // ACTIVEMQ_ACTIVEMQSENSOR_H
