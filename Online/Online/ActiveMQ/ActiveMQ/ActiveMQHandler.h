// $Id: ActiveMQHandler.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/ActiveMQHandler.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_ActiveMQHANDLER_H
#define ACTIVEMQ_ActiveMQHANDLER_H 1

// Framework include files
#include "CPP/Interactor.h"

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ  {

  // Forward declarations
  class ActiveMQSensor;

  /**@class ActiveMQHandler ActiveMQHandler.h ActiveMQ/ActiveMQHandler.h
   *
   * @author M.Frank
   */
  class ActiveMQHandler : public Interactor {
  public:
    enum { PROTO_SIMPLE=0, PROTO_XML=1 };
  protected:
    /// Service data handler
    ActiveMQSensor* m_sensor;
    /// Protocol specifier
    int m_protocol;
  public:
    /// Default constructor
    ActiveMQHandler(ActiveMQSensor* sensor, int prot) : m_sensor(sensor), m_protocol(prot)  {}
    /// Standard destructor
    virtual ~ActiveMQHandler()  {}
    /// Event handler: Subscribes to new connections
    virtual void handle(const Event& ev);
  };
}      // End namespace ActiveMQ
#endif // ACTIVEMQ_AMQPHANDLER_H
