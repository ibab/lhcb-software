#ifndef __EVENT
#define __EVENT 1

/// CPP namespace declaration
namespace CPP  {

  enum EventType {
    TimeEvent,       
    NetEvent,
    UpiEvent,
    IocEvent,
    QioEvent,
    TkEvent,
    PVSSEvent,
    ScrMouseEvent,
    NoEventType
  };

  class Message;
  class Address;
  class Sensor;
  class Interactor;

  class Event {
  private:
    friend class Sensor;
    friend class Interactor;
    Sensor*  sensor;
  public:
    Interactor* target;
    EventType   eventtype;
    /*---TIMER--*/   
    void*       timer_id;
    void*       timer_data;
    /*---NetEvent----*/   
    Message*    message;
    Address*    source;  
    /*---UPI----*/   
    int         menu_id;
    int         command_id;
    int         param_id;
    int         index_id;
    /*---IOC----*/   
    int         type;
    void*       data;
    /*---QIO----*/   
    char*       device;
    char*       buffer_ptr;
    int         buffer_len;
    /// Initializing constructor
    Event(Interactor* tar, EventType typ) 
      : sensor(0), target(tar), eventtype(typ),
        timer_id(0), timer_data(0), 
        message(0), source(0),
        menu_id(0), command_id(0), param_id(0), index_id(0), 
        type(0), data(0),
        device(0), buffer_ptr(0), buffer_len(0)
    {}
    /// Standard constructor
    Event()
      : sensor(0), target(0), eventtype(NoEventType),
        timer_id(0), timer_data(0), 
        message(0), source(0),
        menu_id(0), command_id(0), param_id(0), index_id(0), 
        type(0), data(0),
        device(0), buffer_ptr(0), buffer_len(0)
    {}
    /// Standard destructor
    ~Event() {}
    int iocType() const                        {  return type;                      }
    template <typename T> T* iocPtr() const    {  return (T*)data;                  }
    template <typename T> T iocData() const    {  return (T)data;                   }
    unsigned long iocData() const              {  return (unsigned long)data;       }
    int menu() const                           {  return menu_id;                   }
    int command() const                        {  return command_id;                }
    int param() const                          {  return param_id;                  }
    int index() const                          {  return index_id;                  }
    unsigned long timerID() const              {  return (unsigned long)timer_id;   }
    unsigned long timerData() const            {  return (unsigned long)timer_data; }
    template <typename T> T* get()             {  return (T*)&timer_id;             }
    template <typename T> const T* get() const {  return (T*)&timer_id;             }
  };

}
using CPP::Event;
using CPP::EventType;
using CPP::TimeEvent;       
using CPP::NetEvent;
using CPP::UpiEvent;
using CPP::IocEvent;
using CPP::QioEvent;
using CPP::TkEvent;
using CPP::PVSSEvent;
using CPP::ScrMouseEvent;

#endif
