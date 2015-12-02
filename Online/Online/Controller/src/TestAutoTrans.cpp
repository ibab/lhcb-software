/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#include "FiniteStateMachine/Type.h"
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/Functors.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/TestAutoTrans.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"
#include <cstdio>

using namespace FiniteStateMachine::DAQ;
using namespace FiniteStateMachine;
using namespace std;
typedef FSM::ErrCond ErrCond;

/// Default constructor
TestAutoTrans::TestAutoTrans(Machine* m, int r)
  : m_machine(m), cnt(0), rounds(r)
{
}

/// Default destructior
TestAutoTrans::~TestAutoTrans() 
{
}

/// Configure the transition objects for the DAQ type machine
TestAutoTrans& TestAutoTrans::configDAQ()  {
  Machine* m = m_machine;
  const Type* typ = m->type();
  m->setCompletionAction(typ->transition(ST_NAME_OFFLINE,ST_NAME_NOT_READY), Callback::make(this,&TestAutoTrans::configure));
  m->setCompletionAction(typ->transition(ST_NAME_NOT_READY,ST_NAME_READY),   Callback::make(this,&TestAutoTrans::start));
  m->setCompletionAction(typ->transition(ST_NAME_READY,ST_NAME_RUNNING),     Callback::make(this,&TestAutoTrans::stop));
  m->setCompletionAction(typ->transition(ST_NAME_RUNNING,ST_NAME_READY),     Callback::make(this,&TestAutoTrans::reset));
  m->setCompletionAction(typ->transition(ST_NAME_READY,ST_NAME_NOT_READY),   Callback::make(this,&TestAutoTrans::unload));
  m->setCompletionAction(typ->transition(ST_NAME_NOT_READY,ST_NAME_OFFLINE), Callback::make(this,&TestAutoTrans::load));
  return *this;
}

void TestAutoTrans::print() const  {
  char txt[32]; 
  ::snprintf(txt,sizeof(txt),"%-8d",cnt);
  PrintObject(string(txt))(MachineHandle(m_machine)); 
}

/// Sleep arbitrary time during transition
TestAutoTrans& TestAutoTrans::do_sleep(const string& transition)   {
  map<string,int>::const_iterator i = this->sleep.find(transition);
  if ( i != this->sleep.end() )  {
    ::lib_rtl_sleep(1000*(*i).second);
  }
  return *this;
}

ErrCond TestAutoTrans::go_to(const char* s)  {
  IocSensor::instance().send(this,int(0),(void*)m_machine->type()->state(s)); 
  return FSM::SUCCESS; 
}
ErrCond TestAutoTrans::configure() {
  ++cnt;
  return do_sleep("configure").go_to(ST_NAME_READY);     
}
ErrCond TestAutoTrans::start()     {
  return do_sleep("start").go_to(ST_NAME_RUNNING);   
}
ErrCond TestAutoTrans::stop()      {
  print(); 
  return do_sleep("stop").go_to(ST_NAME_READY);     
}
ErrCond TestAutoTrans::reset()     { 
  return do_sleep("reset").go_to(ST_NAME_NOT_READY); 
}
ErrCond TestAutoTrans::load()      {
  print(); 
  if ( rounds > 0 && cnt >= rounds ) {
    TypedObject::display(TypedObject::ALWAYS,m_machine->c_name(),"Number of rounds reached limit: %d",cnt);
    ::exit(0);
  }
  return do_sleep("load").go_to(ST_NAME_NOT_READY); 
}
ErrCond TestAutoTrans::unload()    {
  return do_sleep("unload").go_to(ST_NAME_OFFLINE);   
}

void TestAutoTrans::handle(const CPP::Event& event) {
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type)   {
    case Slave::SLAVE_TRANSITION:
      go_to(ST_NAME_ERROR);
      break;
    case Slave::SLAVE_FAILED:
      go_to(ST_NAME_ERROR);
      break;
    default:
      m_machine->invokeTransition((const State*)event.data);
      break;
    }
    break;
  default:
    break;
  }
}
