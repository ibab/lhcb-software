/*
**++
**  FACILITY:  DEXPERT (sensor.h)
**
**  MODULE DESCRIPTION:
**
**  Sensor class. Object Sensor is used to interface any external estimulus
**      to the program with the corresponding interactor. There exists one
**      Sensor object for each different input type to the program. For example,
**      AMS messages, alarms, keyword , etc.
**
**  AUTHORS:
**
**      P. Mato
**
**  CREATION DATE:  14-Nov-1990
**
**  DESIGN ISSUES:
**
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/

#ifndef __SENSOR
#define __SENSOR 1

#include <string>

class Sensor;
class Interactor;

class Sensor {
private:
  Sensor  *m_next;
public:
  unsigned int     m_facility;
  bool             m_rearmPending;
  std::string Name;
  explicit Sensor( unsigned int fac, const char* = "NONAME", bool = true);
  /// Standard destructor
  virtual ~Sensor();
  Sensor* next()  const   {  return m_next; }
  void setRearm(bool val) {  m_rearmPending = val;  }

  virtual void add( Interactor*, void* ) {}
  virtual void remove( Interactor*, void* ) {}
  virtual void dispatch(void*) {}
  virtual void rearm() {}
  virtual int  run();
  int runThreaded();
};
#endif
