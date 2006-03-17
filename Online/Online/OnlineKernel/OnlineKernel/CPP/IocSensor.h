#ifndef CPP_IOCSENSOR_H
#define CPP_IOCSENSOR_H 1

#include "CPP/Sensor.h"

/**
  *	Inter Object Communication sensor object. This object handles the
  *	messages between the different objects which compose the DEXPERT
  *	application
  *
  * @author P.Mato
  * @author M.Frank
  *
  */
class IocSensor : public Sensor {
  struct Message {
    Interactor*  Destination;
    int          Type;
    void*        Data;
    Message(Interactor* dst, int t, void* d) : Destination(dst), Type(t), Data(d) {}
  };
public:
  IocSensor();
  /// Standard destructor
  virtual ~IocSensor() {}
  virtual void  dispatch(void* arg);
  virtual void  rearm() {}
  void send( Interactor*, int, void* = 0);
  void send( Interactor*, int, unsigned long );
  static IocSensor& instance();
};

#define IOCSENSOR   (IocSensor::instance())
#endif
