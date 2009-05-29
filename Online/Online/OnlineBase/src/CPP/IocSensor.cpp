#include "CPP/Interactor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"
#include <memory>

//----------------------------------------------------------------------------
IocSensor::IocSensor() : Sensor( WT_FACILITY_SENSOR2, "IocSensor") {
}

//----------------------------------------------------------------------------
void IocSensor::dispatch( void* arg )  {
  std::auto_ptr<Message> msg((Message*)arg);
  Event ev(msg->Destination, IocEvent);
  ev.type =  msg->Type; 
  ev.data =  msg->Data;
  ev.target->handle(ev);
}

//----------------------------------------------------------------------------
void IocSensor::send( Interactor* it, int type, void* data )  {
  ::wtc_insert(WT_FACILITY_SENSOR2, new Message(it, type, data));    
}

//----------------------------------------------------------------------------
void IocSensor::send( Interactor* it, int type, unsigned long data )  {
  ::wtc_insert(WT_FACILITY_SENSOR2, new Message(it, type, (void*)data));    
}

//----------------------------------------------------------------------------
void IocSensor::sendHead( Interactor* it, int type, void* data )  {
  ::wtc_insert_head(WT_FACILITY_SENSOR2, new Message(it, type, data));    
}

//----------------------------------------------------------------------------
void IocSensor::sendHead( Interactor* it, int type, unsigned long data )  {
  ::wtc_insert_head(WT_FACILITY_SENSOR2, new Message(it, type, (void*)data));    
}

/// Clear the command queue
void IocSensor::clear() {
  ::wtc_flush(WT_FACILITY_SENSOR2);
}

//----------------------------------------------------------------------------
IocSensor& IocSensor::instance()  {
  static IocSensor* sensor = new IocSensor;
  return *sensor;
}

