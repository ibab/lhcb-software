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
#ifndef ONLINE_FINITESTATEMACHINE_CONFIG_H
#define ONLINE_FINITESTATEMACHINE_CONFIG_H

#include "CPP/Callback.h"
#include "FiniteStateMachine/FSM.h"
/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {
  typedef int Status;

  /**@class FsmHandle
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  template <typename T> struct FsmHandle {
    const T* ptr;
    /** Class Constructor
     *
     * @arg s   [pointer, read-only] Fsm reference
     */
    FsmHandle(const T* s) : ptr(s) {}
    const T* operator->() const { return  ptr; }
    const T& operator*() const  { return *ptr; }
  };

  /// Utility function to access the package ROOT directory
  std::string controller_root();
  /// Utility function to access the package binary directory
  std::string controller_bindir();
  /// Utility function to access the package library directory
  std::string controller_libdir();
  /// Path to gentest.exe file
  std::string gentest_path();
}      //  End namespace 

#endif // ONLINE_FINITESTATEMACHINE_CONFIG_H
