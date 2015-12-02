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
#ifndef ONLINE_CONTROLLER_TESTAUTOTRANS_H
#define ONLINE_CONTROLLER_TESTAUTOTRANS_H

// Framework include files
#include "FiniteStateMachine/FSM.h"
#include "CPP/Interactor.h"

// C/C++ include files
#include <map>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  class Machine;

  /**@class TestAutoTrans  TestAutoTrans.h FiniteStateMachine/TestAutoTrans.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct TestAutoTrans : public CPP::Interactor {
  public:
    typedef FSM::ErrCond ErrCond;
  protected:
    Machine* m_machine;
    int cnt, rounds;

  public:
    std::map<std::string,int> sleep;

  public:
    /// Default constructor
    TestAutoTrans(Machine* m, int rounds);
    /// Default destructior
    virtual ~TestAutoTrans();

    /// Configure the transition objects for the DAQ type machine
    TestAutoTrans& configDAQ();
    /// Sleep arbitrary time during transition
    TestAutoTrans& do_sleep(const std::string& transition);

    void    print() const;
    ErrCond go_to(const char* s);
    ErrCond configure();
    ErrCond start();
    ErrCond stop();
    ErrCond reset();
    ErrCond load();
    ErrCond unload();
    void    handle(const CPP::Event& event);
  };   //  End class TestAutoTrans
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_TESTAUTOTRANS_H

