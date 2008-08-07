#ifndef ONLINEKERNEL_SCR_MOUSESENSOR_H
#define ONLINEKERNEL_SCR_MOUSESENSOR_H 1

// Framework include files
#include "CPP/Sensor.h"

// System includes
#include <map>
#include <vector>

namespace SCR {
  // Forward declarations
  struct Pasteboard;
  struct Display;

  /**
   * Communication object describing the Mouse click events.
   * After the mouse interaction with a given SCR display
   * Clients retrieve an instance of this object.
   *
   * @author M.Frank
   */
  struct MouseEvent {
    unsigned char button;
    unsigned char modifier;
    unsigned char x;
    unsigned char y;
    unsigned int  msec;
    Display*      display;
    /// Standard constructor
    MouseEvent(int key, int x, int y, unsigned int us);
  };

  /**
   * Mouse sensor. Display clients may subscribe to interrupts of a given display.
   * It is up to the clients to define the appropriate action on the callback of 
   * a given display.
   *
   * @author M.Frank
   */
  class MouseSensor : public Sensor {
    typedef std::vector<Interactor*> Targets;
    typedef std::map<void*, Targets> Clients;

    /// Buffer with clients
    Clients     m_clients;
    /// Reference to pasteboard
    Pasteboard *m_pasteboard;

  protected:
    /// Sensor overload: Dispatch interrupts and deliver callbacks to clients
    virtual void dispatch( void* arg );
    /// Standard constructor
    MouseSensor();
    /// Standard destructor
    virtual ~MouseSensor();

  public:
    /// The MouseSensor is a singleton: Static instantiator
    static MouseSensor& instance();
    /// Subscribe Interactor target to display mouse-events
    void add(Interactor* actor, void* display);
    /// Unsubscribe all Interactor targets from display mouse-events
    void remove(void* display);
    /// Unsubscribe Interactor target from display mouse-events
    void remove(Interactor* actor, void* display);
    /// Start the Mouse listener
    void start(Pasteboard* pb);
    /// Stop the Mouse listener
    void stop();
  };
}
#endif   // ONLINEKERNEL_SCR_MOUSESENSOR_H
