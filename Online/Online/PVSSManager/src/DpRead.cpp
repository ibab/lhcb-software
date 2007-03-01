// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpRead.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $
//  ====================================================================
//  DpRead.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpRead.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $

#include "PVSSManager/SyncWaitForAnswer.h"
#include "PVSS/HotLinkCallback.h"
#include "PVSS/Environment.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"

// PVSS include files
#include "Manager.hxx"

namespace PVSS  {

  /** @class DpReadSync DpReadSync.h PVSSManager/DpReadSync.h
    *
    *  Class to manage data point read requests
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class DpReadSync : public SyncWaitForAnswer    {
  protected:
    /// Hotlink callback object
    HotLinkCallback* m_cb;
  public:
    /// Initializing constructor
    DpReadSync(DevAnswer* a, HotLinkCallback* h) : SyncWaitForAnswer(a), m_cb(h) {}
    /// Default destructor
    virtual ~DpReadSync()                      {                      }
    /// Answer callback
    virtual void call(DpMsgAnswer &answer);
  };

  static bool pvss_exec_readW(DpIdentList* rdx,DevAnswer* a,HotLinkCallback* b)  {
    DpReadSync answer(a,b);
    if ( !rdx ) {
      answer.addError("pvss_exec_read> Invalid transaction context."); 
    }
    else if ( PVSS_TRUE == Manager::dpGet(*rdx,&answer,PVSS_FALSE) )    {
      return PVSS::Environment::instance().waitForAnswer(a);
    }
    else  {   // Post condition
      answer.addError("pvss_exec_read> Transaction failed.");
    }
    return false;
  }

  static bool pvss_exec_readA(DpIdentList* rdx, DevAnswer* a,HotLinkCallback* b)  {
    DpReadSync *answer = new PVSS::DpReadSync(a,b);
    if ( !rdx )   {
      answer->addError("pvss_exec_read> Invalid transaction context.");
    }
    else if ( PVSS_TRUE == Manager::dpGet(*rdx,answer,PVSS_TRUE) )    {
      return true;
    }
    else  {   // Post condition
      answer->addError("pvss_exec_read> Transaction failed.");
    }
    delete answer;
    return false;
  }
}

void PVSS::DpReadSync::call(DpMsgAnswer &answer)  {
  DpElementType elt_t;
  if ( m_cb )  {
    for(AnswerGroup *g=answer.getFirstGroup(); g; g=answer.getNextGroup() )  {
      for(AnswerItem* i=g->getFirstItem(); i; i=g->getNextItem())  {
        const DpIdentifier& id  = i->getDpIdentifier();
        Manager::getElementType(id,elt_t);
        m_cb->setValue(id,elt_t,i->getValuePtr());
      }
    }
  }
}

bool PVSS::pvss_exec_read(void*& context,DevAnswer* a,HotLinkCallback* b,bool keep_list)  {
  bool res = true;
  DpIdentList* list = (DpIdentList*)context;
  try {
    res =  a ? pvss_exec_readW(list,a,b) : pvss_exec_readA(list,a,b);
  }
  catch(const char* err)  {
    printf("Exception:%s\n",err);
  }
  catch(...) {
  }
  if ( !res && a ) a->print();
  if ( !keep_list )  {
    delete list;
    context = 0;
  }
  return res;
}
