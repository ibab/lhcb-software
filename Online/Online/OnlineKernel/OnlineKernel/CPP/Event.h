#ifndef __EVENT
#define __EVENT 1

enum EventType {
  TimeEvent,       
  NetEvent,
  UpiEvent,
  IocEvent,
  QioEvent,
  TkEvent,
  PVSSEvent,
  ScrMouseEvent
};

class Message;
class Address;
class Sensor;
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
  int iocType() const             {  return type;                      }
  template <typename T> T* iocPtr() const  {  return (T*)data;         }
  template <typename T> T iocData() const  {  return (T)data;          }
  unsigned long iocData() const   {  return (unsigned long)data;       }
  int menu() const                {  return menu_id;                   }
  int command() const             {  return command_id;                }
  int param() const               {  return param_id;                  }
  int index() const               {  return index_id;                  }
  unsigned long timerID() const   {  return (unsigned long)timer_id;   }
  unsigned long timerData() const {  return (unsigned long)timer_data; }
  template <typename T> T* get()  {  return (T*)&timer_id;             }
  template <typename T> const T* get() const  {  return (T*)&timer_id; }
private:
  friend class Sensor;
  friend class Interactor;
  Sensor*  sensor;
};
#endif
