/*============================================================
**
**      Ctrl process management on a HLT worker node
**
**  AUTHORS:
**
**      M.Frank
**
**==========================================================*/
// Framework include files
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/TestAutoTrans.h"
#include "Controller/Controller.h"
#include "Controller/NativeDimSlave.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <cstdio>

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;
typedef FSM::ErrCond ErrCond;

namespace   {
  struct ExternalSlave : public NativeDimSlave {
    ExternalSlave(const Type* typ, const string& nam, Machine* machine) : NativeDimSlave(typ,nam,machine,false)   {
      m_cmd = controller_bindir() + "/external_fsm_client.exe";
      m_argv.push_back(name());
      m_argv.push_back("-name="+name());
      cloneEnv();
    }
    virtual ~ExternalSlave() {}
  };
  static string make_slave_name(int i) {
    char text[32];
    ::snprintf(text,sizeof(text),"SLAVE_%d",i);
    return text;
  }
}

extern "C" int external_fsm_client(int argc, char** argv)  {
  int num_instance=0, print = -0;
  string dim_name = "SLAVE";
  RTL::CLI cli(argc, argv, 0);
  cli.getopt("name",      1, dim_name);
  cli.getopt("print",     1, print);
  cli.getopt("instances", 1, num_instance);
  TypedObject::setPrintLevel(print);
  TypedObject::display(TypedObject::ALWAYS,dim_name.c_str(),
		       "Starting. Should fork %d children.",num_instance);
  DAQCommandTarget target(dim_name);
  target.run();
  return 1;
}

extern "C" int external_fsm_test(int argc, char** argv)  {
  string dim_name = "Ctrl";
  int print = 0, rounds = -1, num_slaves=2;
  RTL::CLI cli(argc, argv, 0);
  bool auto_start = cli.getopt("auto",2);
  cli.getopt("name",  1,dim_name);
  cli.getopt("rounds",1,rounds);
  cli.getopt("print", 1,print);
  cli.getopt("slaves",1,num_slaves);
  TypedObject::setPrintLevel(print);

  const Type*    daq = fsm_type("DAQ");
  Machine*       mac = new Machine(daq,"daq++");
  TestAutoTrans* a = new TestAutoTrans(mac,rounds);
  a->configDAQ().sleep["load"] = 2;
  for(int i=0; i<num_slaves; ++i)
    mac->addSlave(new ExternalSlave(daq,make_slave_name(i),mac));

  if ( auto_start ) a->go_to(ST_NAME_NOT_READY);
  DimServer::autoStartOn();
  DimServer::start(dim_name.c_str());
  IocSensor::instance().run();
  return 1;
}

extern "C" int controller_fsm_test(int argc, char** argv)  {
  string dim_name = "Ctrl";
  int    print = 0, num_slaves=2;
  RTL::CLI cli(argc, argv, 0);
  cli.getopt("name",  1,dim_name);
  cli.getopt("print", 1,print);
  cli.getopt("slaves",1,num_slaves);
  TypedObject::setPrintLevel(print);
  const Type* daq = fsm_type("DAQ");
  Machine*    mac = new Machine(daq,dim_name+"::daq++");
  for(int i=0; i<num_slaves; ++i)
    mac->addSlave(new ExternalSlave(daq,make_slave_name(i),mac));

  Controller* target = new Controller(dim_name,"",mac);
  target->display(target->ALWAYS,dim_name.c_str(),"Controller task started...");
  target->publish();
  target->run();
  return 1;
}
