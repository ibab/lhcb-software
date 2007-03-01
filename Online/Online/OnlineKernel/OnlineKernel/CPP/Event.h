#ifndef __EVENT
#define __EVENT 1

enum EventType {
  TimeEvent,       
  NetEvent,
  UpiEvent,
  IocEvent,
  QioEvent,
  TkEvent,
  PVSSEvent
};

class Message;
class Address;

class Interactor;

class Event {
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
  Event(Interactor* tar, EventType typ) : target(tar), eventtype(typ) {}
  /// Standard constructor
  Event() {}
  /// Standard destructor
  ~Event() {}
private:
  friend class Sensor;
  friend class Interactor;
  Sensor*  sensor;
};
#endif
