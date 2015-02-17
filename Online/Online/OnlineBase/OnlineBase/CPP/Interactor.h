// ====================================================================
//  Interactor
// --------------------------------------------------------------------
//
//	Author    : Markus Frank
//
// ====================================================================
#ifndef CPP_INTERACTOR_H
#define CPP_INTERACTOR_H 1

#include "CPP/Sensor.h"

/// CPP namespace declaration
namespace CPP {

  // Forward declarations
  class Event;

  /// Interactor definition: the target class of sensors
  /**
   *  \author P. Mato
   *  \author M. Frank
   */
  class Interactor {
  public:
    /// Standard constructor
    Interactor()                              {                           }
    /// Standard destructor
    virtual ~Interactor()                     {                           }
    virtual void handle(const Event&)         {                           }
    void listen(Sensor& sensor , void* id)    {   sensor.add(this,id);    }
  };
}

using CPP::Event;
using CPP::Interactor;

#endif  // CPP_INTERACTOR_H

