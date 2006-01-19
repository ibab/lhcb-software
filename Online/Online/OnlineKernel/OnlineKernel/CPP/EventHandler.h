#ifndef __EventHandler_H__
#define __EventHandler_H__
class EventReactor;
class EventHandler;

/// Definiton of the abstract EventReactor class
/** 
    {\Large{\bf Class EventReactor}}

   React on stimuli from event handlers
*/
class EventReactor  {
public:
  //@Man: public member functions
  /// Destructor
  virtual ~EventReactor()	{
  }
  /// Abstract method to handle event from given handler
  virtual int Handle ( EventHandler* handler ) = 0;
};

/// Definiton of the abstract EventHandler class
/** 
    {\Large{\bf Class EventHandler}}

   On given event occurence, invoke handle of Reactor
*/
class EventHandler {
protected:
  //@Man: Protected member variables
  /// Pointer to associated Reactor pattern
  EventReactor*   _pEventReactor;
public:
  //@Man: public member functions
  /// Constructor
  EventHandler ( EventReactor* reactor ) : _pEventReactor(reactor)  {
  }
  /// Destructor
  virtual ~EventHandler()  {
  }
  /// Forward event handling to the proper reactor pattern
  virtual int HandleEvent()  {
    return _pEventReactor->Handle ( this );
  }
};
#endif
