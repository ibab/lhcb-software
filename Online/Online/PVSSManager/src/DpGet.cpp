// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/DpGet.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $
//  ====================================================================
//  DpGet.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DpGet.cpp,v 1.2 2007-03-01 15:48:04 frankb Exp $
#ifdef _WIN32
  #pragma warning ( disable : 4786 )
#endif

#include "PVSSManager/SyncWaitForAnswer.h"
#include "PVSSManager/DatapointIO.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Environment.h"
#include "PVSS/Lock.h"

// PVSS include files
#include "Manager.hxx"
#include "DpIdentList.hxx"
#include <map>

namespace PVSS {
  struct DpGetContext {
    struct Entry {
      int type;
      void* value;
      void (*transform)(void*,const void*);
      Entry(const Entry& c) : type(c.type), value(c.value), transform(c.transform) {}
      Entry(int t,void* v, void (*tr)(void*,const void*)) : type(t), value(v), transform(tr) {}
    };
    typedef std::map<DpIdentifier,Entry> Entries;
    DpIdentList list;
    Entries     values;
    DpGetContext() {}
    DpGetContext(const DpGetContext& c) : list(c.list), values(c.values) {}
  };

  /** @class DpGetSync DpGetSync.h PVSSManager/DpGetSync.h
    *
    *  Class to manage data point read requests
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class DpGetSync : public SyncWaitForAnswer    {
  protected:
    /// Context object
    DpGetContext* m_ctx;
  public:
    /// Initializing constructor
    DpGetSync(DevAnswer* a, DpGetContext* h) : SyncWaitForAnswer(a), m_ctx(h) 
    { if ( !m_ctx ) throw ("pvss_exec_dpset> Invalid transaction handle."); }
    /// Default destructor
    virtual ~DpGetSync()  {  if ( m_ctx ) delete m_ctx; }
    /// Answer callback
    virtual void call(DpMsgAnswer &answer);
  };
}

void PVSS::DpGetSync::call(DpMsgAnswer &answer)  {
  if ( m_ctx )  {
    for(AnswerGroup *g=answer.getFirstGroup(); g; g=answer.getNextGroup() )  {
      for(AnswerItem* it=g->getFirstItem(); it; it=g->getNextItem())  {
        const Variable*     var = it->getValuePtr();
        const DpIdentifier& id  = it->getDpIdentifier();
        DpGetContext::Entries::iterator i=m_ctx->values.find(id);
        if ( i != m_ctx->values.end() )  {
          typedef void (*str_transform)(std::string&,const char*);
          DpGetContext::Entry& e = (*i).second;
          if ( var && e.value )  {
            switch(e.type)   {
            case DPELEMENT_DPID:
              DatapointIO::value(var,*(DpIdentifier*)e.value);
              break;
            case DPELEMENT_CHAR:
              DatapointIO::value(var,*(char*)e.value);
              break;
            case DPELEMENT_UINT:
              DatapointIO::value(var,*(unsigned int*)e.value);
              break;
            case DPELEMENT_INT:
              DatapointIO::value(var,*(int*)e.value);
              break;
            case DPELEMENT_FLOAT:
              DatapointIO::value(var,*(float*)e.value);
              break;
            case DPELEMENT_TEXT:
              DatapointIO::value(var,(str_transform)e.transform,*(std::string*)e.value);
              break;
            case DPELEMENT_TIME:
              DatapointIO::value(var,*(time_t*)e.value);
              break;
            case DPELEMENT_BIT:
              DatapointIO::value(var,*(bool*)e.value);
              break;
            case DPELEMENT_32BIT:
            default:
              ::printf("DpGetSync> Invalid data type.\n");
              break;
            }
          }
        }
      }
    }
  }
}

void PVSS::pvss_add_dpget(void*& ctxt,int data_type,void (*ldf)(void*,const void*),const DpIdentifier& id,void* value)  {
  DpGetContext* rdx = (DpGetContext*)ctxt;
  if ( 0 == ctxt ) ctxt = rdx = new DpGetContext;
  switch(data_type)   {
  case DPELEMENT_CHAR:
  case DPELEMENT_UINT:
  case DPELEMENT_INT:
  case DPELEMENT_FLOAT:
  case DPELEMENT_TEXT:
  case DPELEMENT_TIME:
  case DPELEMENT_BIT:
  case DPELEMENT_DPID:
  case DPELEMENT_NOVALUE:
    rdx->list.append(id);
    rdx->values.insert(std::make_pair(id,DpGetContext::Entry(data_type,value,ldf)));
    break;
  case DPELEMENT_32BIT:
  default:
    throw("pvss_add_dpget> Invalid data type");
    break;
  }
}

void PVSS::pvss_start_dpget(void*& context)  {
  pvss_drop_dpget(context);
  context = new DpGetContext;
}

void PVSS::pvss_drop_dpget(void*& context)  {
  if ( context ) {
    DpGetContext* list = (DpGetContext*)context;
    delete list;
  }
  context = 0;
}

static bool pvss_exec_dpgetW(PVSS::DpGetContext* rdx,PVSS::DevAnswer* a)  {
  PVSS::DpGetSync answer(a, rdx);
  if ( PVSS_TRUE == Manager::dpGet(rdx->list,&answer,PVSS_FALSE) )  {
    return PVSS::Environment::instance().waitForAnswer(a);
  }
  answer.addError("pvss_exec_dpset> Transaction failed.");
  return false;
}

static bool pvss_exec_dpgetA(PVSS::DpGetContext* rdx, PVSS::DevAnswer* a)  {
  try  {
    std::auto_ptr<PVSS::SyncWaitForAnswer> answer(new PVSS::DpGetSync(a, rdx));
    if ( PVSS_TRUE != Manager::dpGet(rdx->list,answer.release(),PVSS_TRUE) )  {
      answer->addError("pvss_exec_dpset> Transaction failed.");
      return true;
    }
    return false;
  }
  catch(const char* err)  {
    ::printf("Exception:%s\n",err);
  }
  catch(...) {
  }
  if ( a ) a->print();
  return false;
}

bool PVSS::pvss_exec_dpget(void*& context,DevAnswer* a)  {
  Lock lock(pvss_global_lock());
  DpGetContext* cx = (DpGetContext*)context;
  context = 0;
  return a ? pvss_exec_dpgetW(cx,a) 
           : pvss_exec_dpgetA(cx,a);
}
