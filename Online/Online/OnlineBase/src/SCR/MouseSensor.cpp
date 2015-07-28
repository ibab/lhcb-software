// Framework include files
#include "SCR/MouseSensor.h"
#include "CPP/Interactor.h"
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "CPP/Event.h"
#include "RTL/SysTime.h"

using namespace SCR;
typedef unsigned char uc8;

static int scr_mouse_handler(SCR::Pasteboard* /* pb */,int key,int x,int y) {
  static unsigned int click = 0;
  unsigned int now=RTL::SysTime::now(), diff = now-click;
  Event* ev = new Event(0,ScrMouseEvent);
  new(ev->get<void>()) MouseEvent(key,x,y,~0x0);
  ::wtc_insert(WT_FACILITY_SCR_MOUSE,ev);
  if ( diff<300 && click != 0 ) {
    ev = new Event(0,ScrMouseEvent);
    new(ev->get<void>()) MouseEvent(key,x,y,diff);
    ::wtc_insert(WT_FACILITY_SCR_MOUSE,ev);
    click = 0;
  }
  else {
    click = now;
  }
  return 1;
}


/// Standard constructor
MouseEvent::MouseEvent(int key, int x_val, int y_val, unsigned int ms) 
  : button(uc8(key&0x3)), modifier(uc8(key>>2)), x(uc8(x_val-0x20)), y(uc8(y_val-0x20)), msec(ms), display(0)
{
}

/// Standard constructor
MouseSensor::MouseSensor() : CPP::Sensor(WT_FACILITY_SCR_MOUSE,"ScrMouseSensor",true),m_pasteboard(0) {
}

/// Standard destructor
MouseSensor::~MouseSensor() {
}

/// The MouseSensor is a singleton: Static instantiator
MouseSensor& MouseSensor::instance()  {
  static MouseSensor* sensor = new MouseSensor;
  return *sensor;
}

/// Enable mouse handling
void MouseSensor::start(Pasteboard* pb) {
  if ( pb ) {
    ::scrc_enable_mouse(pb);
    ::scrc_set_mouse_handler(pb, scr_mouse_handler);
    m_pasteboard = pb;
  }
}

/// Enable mouse handling
void MouseSensor::stop() {
  if ( m_pasteboard ) {
    ::scrc_disable_mouse(m_pasteboard);
    ::scrc_set_mouse_handler(m_pasteboard, 0);
    m_pasteboard = 0;
  }
}

/// Sensor overload: Dispatch interrupts and deliver callbacks to clients
void MouseSensor::dispatch( void* arg )  {
  std::auto_ptr<CPP::Event> ev((Event*)arg);
  if ( ev.get() ) {
    MouseEvent* m = ev->get<MouseEvent>();
    Display* display = ::scrc_display_at (m_pasteboard,m->y,m->x);
    if ( display ) {
      Clients::const_iterator i = m_clients.find(display);
      if ( i != m_clients.end() ) {
        const Targets& t = (*i).second;
        m->display = display;
        for(Targets::const_iterator j=t.begin(); j!=t.end(); ++j) {
          (*j)->handle(*(ev.get()));
        }
      }
    }
  }
}

/// Subscribe Interactor target to display mouse-events
void MouseSensor::add(CPP::Interactor* actor, void* display) {
  if ( display ) {
    Targets& t = m_clients[display];
    for(Targets::iterator j=t.begin(); j!=t.end(); ++j)
      if ( *j == actor ) return;
    t.push_back(actor);
  }
}

/// Unsubscribe all Interactor targets from display mouse-events
void MouseSensor::remove(void* display) {
  remove((Interactor*)0, display);
}

/// Unsubscribe Interactor target from display mouse-events
void MouseSensor::remove(CPP::Interactor* actor, void* display) {
  Clients::iterator i = m_clients.find(display);
  if ( i != m_clients.end() ) {
    Targets& t = (*i).second;
    if ( actor == 0 ) {
      t.clear();
    }
    else {
      for(Targets::iterator j=t.begin(); j!=t.end(); ++j) {
        if ( (*j) == actor ) {
          t.erase(j);
          break;
        }
      }
    }
    if ( t.empty() ) m_clients.erase(i);
  }
}
