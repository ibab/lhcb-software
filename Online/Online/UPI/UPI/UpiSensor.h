#ifndef CPP_UPISENSOR_H
#define CPP_UPISENSOR_H 1

#include "CPP/Sensor.h"
#include <map>

/** @class UpiSensor UpiSensor.h CPP/UpiSensor.h
  *
  *	Sensor to receive UPI input type
  *      The UpiSensor  class is derivated from the Sensor class
  *
  *  @author P. Mato
  *
  */
class UpiSensor : public Sensor {
  typedef std::map<int,Interactor*> Table;
  Table    m_table;
  UpiSensor();
  virtual ~UpiSensor();
public:
  static UpiSensor& instance();
  static int newID();
  virtual void  add(Interactor* source, void* id)
  {  Sensor::add(source,id);                             }
  virtual void  add(Interactor* source, int id);
  virtual void  remove(Interactor* source, int id);
  virtual void  remove(Interactor* source, void* id)
  {  Sensor::remove(source,id);                          }
  virtual void  dispatch(void* param);
  virtual void  rearm() { }
};

#endif    // CPP_UPISENSOR_H
