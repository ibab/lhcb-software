// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpConnect.cpp,v 1.4 2007-04-11 17:45:47 frankb Exp $
//  ====================================================================
//  DpConnect.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpConnect.cpp,v 1.4 2007-04-11 17:45:47 frankb Exp $
#include "PVSSManager/SyncWaitForAnswer.h"
#include "PVSS/HotLinkCallback.h"
#include "PVSS/Environment.h"
#include "PVSS/Internals.h"
#include "PVSS/Lock.h"

// PVSS include files
#include "Manager.hxx"

namespace PVSS {
  /** @class DpConnectSync DpConnectSync.h PVSSManager/DpConnectSync.h
    *
    *  Class to manage data point read requests
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class DpConnectSync : public SyncWaitForAnswer    {
  protected:
    /// Hotlink callback object
    HotLinkCallback* m_cb;
  public:
    /// Initializing constructor
    DpConnectSync(DevAnswer* a, HotLinkCallback* h) : SyncWaitForAnswer(a), m_cb(h) {}
    /// Default destructor
    virtual ~DpConnectSync()                   {                      }
    /// Access to callback
    HotLinkCallback* callback()  const         {  return m_cb;        }
    /// Hotlink callback
    virtual void hotLinkCallBack(DpHLGroup& g);
  };
}

void PVSS::DpConnectSync::hotLinkCallBack(DpHLGroup &g)   {
  if ( m_cb )   {
    DpElementType elt_t;
    for (DpVCItem *i = g.getFirstItem(); i; i = g.getNextItem())	{
      const DpIdentifier& id = i->getDpIdentifier();
      Manager::getElementType(id,elt_t);
      m_cb->setValue((DpID&)id,elt_t,i->getValuePtr());
    }
    m_cb->handleDataUpdate();
  }
}

bool PVSS::pvss_list_connect(void*& ctxt,HotLinkCallback* cb,void*& link)   {
  Lock lock(pvss_global_lock());
  DevAnswer*         a = new DevAnswer;
  SyncWaitForAnswer* h = new DpConnectSync(a,cb);
  DpIdentList*       l = (DpIdentList*)ctxt;
  if ( PVSS_TRUE == Manager::dpConnect(*l,h,PVSS_FALSE) )  {
    bool res = PVSS::Environment::instance().waitForAnswer(a);
    link = h;
    return res;
  }
  // Error
  h->addError("pvss_list_connect> Disconnect failed.\n");
  if ( h ) delete h;
  if ( a ) delete a;
  link = 0;
  return false;
}

bool PVSS::pvss_list_disconnect(void* ctxt,void* link)  {
  Lock lock(pvss_global_lock());
  DpIdentList*       l = (DpIdentList*)ctxt;
  SyncWaitForAnswer* h = (DpConnectSync*)link;
  if ( !l || !h )  {
    DevAnswer*         a = h->waitContext();
    h->setAnswerState(DevAnswer::WAITING);
    if ( PVSS_TRUE == Manager::dpDisconnect(*l,h) )  {
      bool res = PVSS::Environment::instance().waitForAnswer(a);
      if ( h ) delete h;
      if ( a ) delete a;
      return res;
    }
    if ( h ) h->addError("pvss_list_disconnect> Disconnect failed.\n");
    if ( h ) delete h;
    if ( a ) delete a;
  }
  return false;
}
