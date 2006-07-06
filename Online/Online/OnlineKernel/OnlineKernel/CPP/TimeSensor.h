/*
**++
**  FACILITY:  DEXPERT  (TimeSensor.h)
**
**  MODULE DESCRIPTION:
**
**      TimeSensor class definition.
**
**  AUTHORS:
**
**      P. Mato
**
**  CREATION DATE:  16-Nov-1990
**
**  DESIGN ISSUES:
**
**      The TimeSensor behaves as similar way that the AmsSensor or 
**      UpiSensor all of them are derivated from a unique Sensor class.
**
**
**  MODIFICATION HISTORY:
**
**      16-Nov-1990 Derivated from the existing Timer.h 
**--
*/

#ifndef CPP_TIMESENSOR_H
#define CPP_TIMESENSOR_H

#include "CPP/Sensor.h"

class TimeSensor : public Sensor {
  /// Standard constructor
  TimeSensor();
public:

  /// Standard destructor
  virtual ~TimeSensor();
  /// Add interactor
  virtual void add( Interactor*, void* );
  /// Add interactor
  virtual void  add(Interactor*, void*, void*);
  /// Add interactor
  virtual void  add(Interactor*, int, void* = 0 );
  /// Remove interactor
  virtual void  remove(Interactor*, void* = 0);
  /// Timer event dispatching routine
  virtual void  dispatch(void*);
  /// Timer rearm callback
  virtual void  rearm();
  /// Singleton instantiation
  static TimeSensor& instance();
};

#define TIMESENSOR (TimeSensor::instance())

#endif // CPP_TIMESENSOR_H
