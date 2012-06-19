// $Id: SubscribeHandler.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
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
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/SubscribeHandler.h,v 1.1 2010/11/01 17:20:22 frankb Exp $
#ifndef ACTIVEMQ_SUBSCRIBEHANDLER_H
#define ACTIVEMQ_SUBSCRIBEHANDLER_H 1

// Framework include files
#include "CPP/Interactor.h"
// C++ include files
#include <string>
#include <map>

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ  {

  // Forward declarations
  class Connector;

  /**@class SubscribeHandler SubscribeHandler.h ActiveMQ/SubscribeHandler.h
   *
   * @author M.Frank
   */
  class SubscribeHandler : public Interactor {
  public:
    /// Definition of connection points
    typedef std::map<std::string,Connector*>   Connections;
  protected:
    /// Service data handler
    Interactor* m_sender;
    /// Map with known connections
    Connections m_con;
  public:
    /// Default constructor
    SubscribeHandler(const std::string& ini, Interactor* sender);
    /// Standard destructor
    virtual ~SubscribeHandler();
    /// Load services from file
    void loadServices(const std::string& ini);

    /// Event handler: Subscribes to new connections
    virtual void handle(const Event& ev);
    /// Ioc event handler
    virtual void handleIoc(const Event& ev);
    /// Network event handler
    void handleNet(const Event& ev);
  };
}      // End namespace ActiveMQ
#endif // ACTIVEMQ_SUBSCRIBEHANDLER_H
