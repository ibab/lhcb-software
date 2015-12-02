/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**      FOR HLT2 monitoring purposes
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/

// Framework include files
#include "Controller/FmcSlave.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "Controller/RunMonitor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "RTL/rtl.h"
#include <sstream>
#include <libgen.h>
#include "dis.h"

using namespace std;
using namespace FiniteStateMachine;
using namespace FiniteStateMachine::DAQ;

/*
 * Anyonymous namespace
 */
namespace {

  /// Helper class RunMonitorSlave
  /**
   * \author  M.Frank
   * \date    01/03/2013
   * \version 0.1
   */
  struct RunMonitorSlave : public Slave {
    /// Sleep between transitions [milli seconds]
    int m_sleep;
    RunMonitorController* control;
  public: 
    typedef FSM::ErrCond ErrCond;
    /// Initializing constructor
    RunMonitorSlave(RunMonitorController* ctrl) 
      : Slave(ctrl->machine()->type(),ctrl->machine()->name(),ctrl->machine(),true), 
	m_sleep(0), control(ctrl) {}
    /// Defautl constructor
    virtual ~RunMonitorSlave() {}
    /// Inquire slave state. The reply may come later!
    virtual ErrCond inquireState() { return FSM::SUCCESS; }
    /// Start slave process. Base class implementation will throw an exception
    virtual ErrCond start()      { return FSM::SUCCESS; }
    /// Kill slave process. Base class implementation will throw an exception
    virtual ErrCond kill()       {
      IocSensor::instance().send(this,Slave::SLAVE_LIMBO,this);
      return FSM::SUCCESS;
    }
    /// Kill slave process. Base class implementation will throw an exception
    virtual ErrCond forceKill()  { return FSM::SUCCESS; }
    /// Virtual method -- must be overloaded -- Send transition request to the slave
    //virtual ErrCond sendRequest(const Transition* )  { return FSM::SUCCESS; }

    /// Start the slave's transition timeout
    virtual Slave& startTimer(int, const void*) { return *this; }
    /// Stop the slave's transition timeout
    virtual Slave& stopTimer() { return *this; }
    /// Helper to send internal interrupts
    virtual ErrCond send(int cmd, const State* state=0)  const {
      if ( 0 != state )  {
	Slave* s = const_cast<Slave*>((const Slave*)this);
	if ( state->name() == "RUNNING" )   {
	  display(INFO,c_name(),"Slave state: ADD TIMER          %s",state->c_name());
	  TimeSensor::instance().add(s,2,0);
	}
	else  {
	  display(INFO,c_name(),"Slave state:          %s",state->c_name());
	  if ( m_sleep > 0 )   {
	    ::lib_rtl_sleep(m_sleep);
	  }
	}
      }
      return Slave::send(cmd,state);
    }
    /// IOC handler
    virtual void handle(const CPP::Event& event)   {
      switch(event.eventtype) {
      case TimeEvent:   {
	handleState(ST_NAME_PAUSED);
	break;
      }
      default:
	this->Slave::handle(event);
	break;
      }
    }
  };

  Slave* __create_internal_slave(RunMonitorController* controller)  {
    Slave* sl = new RunMonitorSlave(controller);
    Slave::display(Slave::ALWAYS,"CREATE","+++ Created INTERNAL slave %s of type:%s",sl->c_name(),sl->type()->c_name());
    return sl;
  }
  Slave* __create_external_slave(RunMonitorController* controller)  {
    FmcSlave* fmc = new FmcSlave(controller->machine()->type(),
				 controller->machine()->name(),
				 controller->machine(),
				 false);
    RunMonitorItem* work = controller->work();
    string opts;
    string partition = work->properties["partitionname"];
    string args = " -DUTGID="+controller->machine()->name();
    string fifo = "/tmp/logSrv.fifo";
    args += " -E "+fifo;
    args += " -O "+fifo;
    args += " -D EVENTSELECTOR_OPTIONS="+work->eventSelectorOpts;
    args += " -D WORKING_DIRECTORY="+string(::dirname((char*)(opts=work->eventSelectorOpts).c_str()));
    args += " -n online";
    fmc->setFmcArgs(args);

    args  = " -type="+controller->config()->taskType;
    args += " -partition="+partition;
    args += " -runinfo="+work->runInfo;
    fmc->setCommand("/group/online/dataflow/scripts/runFarmTask.sh");
    fmc->setArgs(args);
    fmc->setTimeout(30);
    const Type::States& states = fmc->type()->states();
    for(Type::States::const_iterator is=states.begin(); is!=states.end(); ++is)  {
      const State::Transitions& tr = (*is).second->outgoing();
      for(State::Transitions::const_iterator itr=tr.begin(); itr!=tr.end(); ++itr)  {
	const Transition* tr = *itr;
	if ( tr->name() == "configure" )  {
	  fmc->addTimeout(tr,60);
	}
      }
    }
    Slave::display(Slave::ALWAYS,"CREATE","+++ Created FMC slave %s of type:%s",fmc->c_name(),fmc->type()->c_name());
    return fmc;
  }
}

bool FiniteStateMachine::fsm_slave_anomaly(const Slave* s)   {
  Slave::SlaveState sls = s->statusState();
  bool anomaly = sls == Slave::SLAVE_FAILED;
  anomaly |= sls == Slave::SLAVE_LIMBO;
  anomaly |= sls == Slave::SLAVE_KILLED;
  anomaly |= sls == Slave::SLAVE_DEAD;
  anomaly |= sls >= Slave::SLAVE_TIMEOUT;
  return anomaly;
}

string FiniteStateMachine::fsm_slave_state_string(Machine* m, bool always)   {
  const Machine::Slaves& slaves = m->slaves();
  for(Machine::Slaves::const_iterator i=slaves.begin(); i!= slaves.end(); ++i)  {
    if ( always || fsm_slave_anomaly(*i) )  {
      Slave* s = *i;
      stringstream str;
      str << "Slave/"<< s->name() 
	  << "/state:" << s->c_state() << "-" << s->metaStateName() << "-" << s->statusName()
	  << "/status:" << yes_no(s->isAlive()) << "-" << yes_no(s->isLimbo()) << "-" << yes_no(s->answered())
	  << "/time:" << ::lib_rtl_timestr();
      return str.str();
    }
  }
  return "";
}

#include <memory>
static void help_ctrl() {
  ::fprintf(stdout,
	    " fsm_ctrl -arg=<value> [ -arg=<value> ... ]                                 \n"
	    "          -help                     Print this help message.                \n"
	    "          -print=[number]           Print level (0 for debug).              \n"
	    "          -runinfo=[string]         Path to file with python formatted      \n"
	    "                                    information from RunInfo datapoint.     \n"
	    "          -sleep=[number]           Sleep a number of seconds at startup    \n"
	    "                                    to enable debugging.                    \n");
  ::exit(EINVAL);
}

extern "C" int run_monitor_test(int argc, char** argv)  {
  RunMonitorConfig cfg;
  string runinfo, base;
  int    print = 0, secs_sleep=0;
  RTL::CLI cli(argc, argv, help_ctrl);
  cfg.name         = RTL::processName();
  cfg.numWorkers   = 10;
  cfg.scanInterval = -1;
  cfg.debugScanner = true;
  cfg.slaveType    = "FmcSlave";

  cli.getopt("sleep",2,          secs_sleep);
  cli.getopt("print",2,          print);
  cli.getopt("runinfo",2,        runinfo);

  cli.getopt("base",4,           base);
  cli.getopt("name",4,           cfg.name);
  cli.getopt("slave_type",4,     cfg.slaveType);
  cli.getopt("task_type",4,      cfg.taskType);
  cli.getopt("todo",4,           cfg.todo);
  cli.getopt("done",4,           cfg.done);
  cli.getopt("failed",4,         cfg.failed);
  cli.getopt("events",3,         cfg.numEventsPerRun);
  
  cli.getopt("avg_evt_size",3,   cfg.avgEventSize);
  cli.getopt("min_file_evt",9,   cfg.minEventsPerFile);
  cli.getopt("min_data_evt",9,   cfg.minEventsPerRun);
  cli.getopt("data_directory",4, cfg.dataDirectory);
  cli.getopt("scan_interval",4,  cfg.scanInterval);
  cli.getopt("instances",4,      cfg.numWorkers);
  cfg.internalOnly = cli.getopt("internal",3);
  cfg.messenger    = cli.getopt("diminterface",2);
  cfg.createSlave  = __create_external_slave; // __create_internal_slave
  bool reconnect   = cli.getopt("reconnect",4);

  if ( cfg.taskType.empty() ) help_ctrl();

  TypedObject::setPrintLevel(print);
  if ( secs_sleep > 0 ) {
    for(secs_sleep *= 1000; secs_sleep >= 0; secs_sleep -= 100)
      ::lib_rtl_sleep(100);
  }
  if ( !base.empty() )  {
    if ( 0 != ::chdir(base.c_str()) )  {
      ::fprintf(stdout,"Failed to change to base directory:%s [%s]\n",
		base.c_str(),RTL::errorString());
      return EINVAL;
    }
  }
  cfg.make_dirs();
  Type* typ = fsm_type("DAQ");
  RunMonitorCoordinator coordinator(typ, &cfg);
  if ( cfg.scanInterval > 0 )  {
    RunMonitorToDoScan* s = new RunMonitorToDoScan(&cfg);
    coordinator.adoptScanner(s);
  }
  if ( cfg.messenger )   {
    RunMonitorMessenger* m = new RunMonitorMessenger(&coordinator, &cfg);
    coordinator.adoptMessenger(m);
  }
  if ( print <= TypedObject::DEBUG )  {
    IocSensor::instance().send(&coordinator,CMD_DEBUG);
  }
  if ( reconnect )  {              // Reco and test are mutually exclusive!
    RunMonitorRestore* restore = new RunMonitorRestore(&coordinator, &cfg);
    coordinator.adoptRestore(restore);
  }
  else if ( cfg.internalOnly )  {  // Test setup
    cfg.createSlave  = __create_internal_slave;
    for(int i=0; i<7; ++i)   {
      IocSensor::instance().send(&coordinator,CMD_RESTART);
    }
  }
  ::dis_start_serving(cfg.name.c_str());
  coordinator.run(true);
  return 1;
}
