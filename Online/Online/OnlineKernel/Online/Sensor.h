#ifndef ONLINE_SENSOR_H
#define ONLINE_SENSOR_H

#include <string>

namespace Online {
  typedef int Status;

  class Interactor;

  class Event {
  public:
    Interactor* m_target;
    int         m_type;
    Event(Interactor* actor, int typ) : m_target(actor), m_type(typ) {}
  };

  class Interactor {
  public:
    Interactor() {}
    virtual ~Interactor() {}
    virtual void handle(Event& event) {}
  };

  class Sensor {
    static long staticRun( void* parameter );
    static Status staticRearm( int facility, int parameter );
    static Status staticAction( int facility, int parameter );
  public:
    Sensor     *m_next;
    void       *m_handle;
    int         m_facility;
    bool        m_rearmPending;
    std::string m_name;
    Sensor(int facility, const std::string& = "NONAME", bool wt_subs = true);
    virtual ~Sensor();
    Sensor* next() const  {  return m_next; }
    virtual void add( Interactor* interactor) {}
    virtual void remove( Interactor* interactor) {}
    virtual void dispatch(int parameter) {}
    virtual void rearm() {}
    virtual void wait();
    virtual void run();
  };
}
#endif //ONLINE_SENSOR_H
