// ====================================================================
//  UpiSensor
// --------------------------------------------------------------------
//
//	Author    : Markus Frank
//
// ====================================================================
#ifndef CPP_UPISENSOR_H
#define CPP_UPISENSOR_H 1

#include "CPP/Sensor.h"
#include <map>

/// CPP namespace declaration
namespace CPP {

  /// Sensor to receive UPI input type
  /**
   *    The UpiSensor  class is derivated from the Sensor class
   *
   *  \author P. Mato
   *  \author M. Frank
   */
  class UpiSensor : public CPP::Sensor {
    typedef std::map<int,CPP::Interactor*> Table;
    Table    m_table;
    UpiSensor();
    virtual ~UpiSensor();
  public:
    static UpiSensor& instance();
    static int newID();
    virtual void  add(CPP::Interactor* source, void* id)
    {  Sensor::add(source,id);                             }
    virtual void  add(CPP::Interactor* source, int id);
    virtual void  remove(CPP::Interactor* source, int id);
    virtual void  remove(CPP::Interactor* source, void* id)
    {  Sensor::remove(source,id);                          }
    virtual void  dispatch(void* param);
    virtual void  rearm() { }
  };
}
using CPP::UpiSensor;

#endif    // CPP_UPISENSOR_H
