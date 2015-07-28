#ifndef CPP_IOCSENSOR_H
#define CPP_IOCSENSOR_H 1

#include "CPP/Sensor.h"


/// CPP namespace declaration
namespace CPP  {

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
    /// Dispatch command requests to clients
    virtual void  dispatch(void* arg);
    /// Rearm sensor action
    virtual void  rearm() {}
    /// Queue command for execution
    void send( Interactor*, int, void* = 0);
    /// Queue command for execution
    void send( Interactor*, int, unsigned long );
    /// Send priority command
    void sendHead( Interactor*, int, void* = 0);
    /// Send priority command
    void sendHead( Interactor*, int, unsigned long );
    /// Clear the command queue
    void clear();
    /// Singleton: Instantiate object
    static IocSensor& instance();
  };
}
using CPP::IocSensor;

#define IOCSENSOR   (::CPP::IocSensor::instance())
#endif
