#ifndef CPP_EVENTHANDLER_H
#define CPP_EVENTHANDLER_H

// Forward declarations
class EventReactor;
class EventHandler;

/** @class EventReactor EventHandler.h CPP/EventHandler.h
  *
  *  Definiton of the abstract EventReactor class
  *  React on stimuli from event handlers.
  *
  *  @author M.Frank
  *  @version 1.0
  */
class EventReactor  {
public:
  //@Man: public member functions
  /// Destructor
  virtual ~EventReactor()	{  }
  /// Abstract method to handle event from given handler
  virtual int handle ( EventHandler* handler ) = 0;
};

/** @class EventHandler EventHandler.h CPP/EventHandler.h
  *
  *  Definiton of the abstract EventHandler class
  *  On given event occurence, invoke handle of Reactor
  *
  *  @author M.Frank
  *  @version 1.0
  */
class EventHandler {
protected:
  //@Man: Protected member variables
  /// Pointer to associated Reactor pattern
  EventReactor*   m_pEventReactor;
public:
  //@Man: public member functions
  /// Constructor
  explicit EventHandler(EventReactor* reactor) : m_pEventReactor(reactor)  {}
  /// Destructor
  virtual ~EventHandler()    {}
  /// Forward event handling to the proper reactor pattern
  virtual int handleEvent()  { return m_pEventReactor->handle(this); }
};
#endif  // CPP_EVENTHANDLER_H
