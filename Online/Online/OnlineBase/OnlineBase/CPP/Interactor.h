#ifndef CPP_INTERACTOR_H
#define CPP_INTERACTOR_H 1

#include "CPP/Sensor.h"
class Event;

class Interactor {
public:
  /// Standard constructor
  Interactor()                              {                           }
  /// Standard destructor
  virtual ~Interactor()                     {                           }
  virtual void handle(const Event&)         {                           }
  void listen(Sensor& sensor , void* id)    {   sensor.add(this,id);    }
};

#endif  // CPP_INTERACTOR_H

