/*============================================================
**
**      Ctrl process management on a HLT worker node
**
**      Test program to execute a FSM using internal slaves
**
**  AUTHORS:
**
**      M.Frank
**
**==========================================================*/
// Framework include files
#include "FiniteStateMachine/Slave.h"
#include "FiniteStateMachine/Machine.h"
#include "FiniteStateMachine/FSMTypes.h"
#include "FiniteStateMachine/TestAutoTrans.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/rtl.h"

using namespace std;
using namespace FiniteStateMachine;

/*
 * Anyonymous namespace
 */
namespace   {

  /**@class InternalSlave
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct InternalSlave : public Slave {
    /// Sleep between transitions [milli seconds]
    int m_sleep;
  public: 
    typedef FSM::ErrCond ErrCond;
    /// Initializing constructor
    InternalSlave(const Type* typ, const string& nam, int msleep, Machine* machine) 
      : Slave(typ,nam,machine), m_sleep(msleep) {
    }
    /// Defautl constructor
    virtual ~InternalSlave() {
    }
    /// Helper to send internal interrupts
    virtual ErrCond _send(ErrCond ret, int cmd, const State* state=0)  {
      if ( m_sleep > 0 ) ::lib_rtl_sleep(m_sleep);
      send(cmd,state);
      return ret;
    }
    /// Start internal slave
    virtual ErrCond start()  {
      display(NOLOG,"Slave %s> starting....",c_name());
      return _send(FSM::WAIT_ACTION,SLAVE_ALIVE);
    }
    /// Kill internal slave
    virtual ErrCond kill()  {
      display(NOLOG,"Slave %s> killing....",c_name());
      return _send(FSM::WAIT_ACTION,SLAVE_LIMBO);
    }
    /// Send internal message
    virtual ErrCond sendRequest(const Transition* tr)  {
      return _send(FSM::SUCCESS,SLAVE_FINISHED,tr->to());
    }
  };
  static void help_fun() {}
}


extern "C" int internal_fsm_test(int argc, char** argv)  {
  int msleep = -1, print = -1, rounds = -1;
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("sleep",1,msleep);
  cli.getopt("rounds",1,rounds);
  cli.getopt("print",1,print);

  TypedObject::setPrintLevel(print);
  const Type*    t = fsm_type("DAQ");
  Machine*       m = new Machine(t,"daq++");
  TestAutoTrans* a = new TestAutoTrans(m,rounds);
  a->configDAQ();
  m->addSlave(new InternalSlave(t,"SLAVE_0",msleep,m));
  m->addSlave(new InternalSlave(t,"SLAVE_1",msleep,m));
  m->addSlave(new InternalSlave(t,"SLAVE_2",msleep,m));
  m->addSlave(new InternalSlave(t,"SLAVE_3",msleep,m));
  m->addSlave(new InternalSlave(t,"SLAVE_4",msleep,m));
  a->go_to(DAQ::ST_NAME_NOT_READY);
  IocSensor::instance().run();
  return 1;
}
