// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpSet.cpp,v 1.3 2007-03-02 19:53:46 frankb Exp $
//  ====================================================================
//  DpSet.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpSet.cpp,v 1.3 2007-03-02 19:53:46 frankb Exp $
#include "PVSSManager/SyncWaitForAnswer.h"
#include "PVSS/Environment.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Lock.h"

// PVSS include files
#include "Manager.hxx"

namespace PVSS {
  static bool pvss_exec_dpset_W(DpIdValueList* list, DevAnswer* a)  {
    SyncWaitForAnswer answer(a);
    if ( !list )
      answer.addError("pvss_exec_dpset> Invalid transaction handle: Transaction failed.");
    else if ( PVSS_TRUE == Manager::dpSet(*list,&answer,PVSS_FALSE) )
      return Environment::instance().waitForAnswer(a);
    else
      answer.addError("pvss_exec_dpset> Transaction failed.");
    return false;
  }

  static bool pvss_exec_dpset_A(DpIdValueList* list, DevAnswer* a)  {
    SyncWaitForAnswer *answer = new SyncWaitForAnswer(a);
    if ( !list )
      answer->addError("pvss_exec_dpset> Invalid transaction handle: Transaction failed.");
    else if ( PVSS_TRUE == Manager::dpSet(*list,answer,PVSS_TRUE) )
      return true;
    else
      answer->addError("pvss_exec_dpset> Transaction failed.");
    delete answer;
    return false;
  }
}
bool PVSS::pvss_exec_dpset(void*& context,DevAnswer* a, bool keep_list)  {
  Lock lock(pvss_global_lock());
  DpIdValueList* list = (DpIdValueList*)context;
  bool res = false;
  try  {
    res = a ? pvss_exec_dpset_W(list,a) : pvss_exec_dpset_A(list,a);
  }
  catch(const char* err)  {
    printf("Exception:%s\n",err);
  }
  catch(...) {
  }
  if ( a ) a->print();
  if ( !keep_list )  {
    delete list;
    context = 0;
  }
  return false;
}
