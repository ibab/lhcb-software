#ifndef CPP_AMSSENSOR_H
#define CPP_AMSSENSOR_H

#include "CPP/Sensor.h"
#include "CPP/Message.h"

/** @class AmsSensor AmsSensor.h  CPP/AmsSensor.h
  *
  *    The AmsSensor class implements the object that is in charge
  *    of sending/receiving network AMS messages. It also in charge
  *    (for that reason inherits from Sensor) of receiving asynchronous
  *    messages coming from the network addressed to one of the 
  *    objects (Interactor). An Interactor is any object that is able
  *    to receive any kind of external stimuli (I/O, messages, time, 
  *    etc.). The AmsSensor must dispatch the unrequested incoming 
  *    messages to the corresponding interactor.
  *
  *    @author P.Mato
  *    @author M.Frank
  */
class AmsSensor : public Sensor {
protected:
  AmsSensor();
  /// Standard destructor
  virtual ~AmsSensor();
public:
  static AmsSensor& instance();
  //------As a Sensor------//
  virtual void add(Interactor*, void* );
  virtual void add(Interactor*, const Address& );
  virtual void add(Interactor*, const Address&, bool );
  virtual void add(Interactor*, const std::string& addr, bool = false );
  virtual void remove(Interactor*, void* );
  virtual void remove(Interactor*, const Address* );
  virtual void dispatch(void*);
  virtual void rearm() {}
  //------Application interface-----//
  virtual int subscribe( int, bool = false );
  virtual int send( const Message* msg, const Address& dest );
  virtual int receive( Message**, Address* src, int timeout = 0 );
  virtual int dispatchBroadCast();
};

#endif // CPP_AMSSENSOR_H
