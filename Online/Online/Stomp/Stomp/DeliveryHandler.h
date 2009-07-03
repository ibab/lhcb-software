// $Id: DeliveryHandler.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/Stomp/DeliveryHandler.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
#ifndef STOMP_DELIVERYHANDLER_H
#define STOMP_DELIVERYHANDLER_H 1

// Framework include files
#include "CPP/Interactor.h"

/*
 *   Stomp namespace declaration
 */
namespace Stomp  {

  // Forward declarations
  class StompSensor;

  /**@class DeliveryHandler DeliveryHandler.h Stomp/DeliveryHandler.h
   *
   * @author M.Frank
   */
  class DeliveryHandler : public Interactor {
  protected:
    /// Service data handler
    StompSensor* m_sensor;
  public:
    /// Default constructor
    DeliveryHandler(StompSensor* sensor) : m_sensor(sensor) {}
    /// Standard destructor
    virtual ~DeliveryHandler()  {}
    /// Event handler: Subscribes to new connections
    virtual void handle(const Event& ev);
  };
}      // End namespace Stomp
#endif // STOMP_DELIVERYHANDLER_H
