// $Id: ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
//====================================================================
//
//  Package    : OnlineBase
//  Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $

#define MBM_IMPLEMENTATION
#include "MBM/BufferInfo.h"
#include "MBM/bmstruct.h"
#include "MBM/mepdef.h"
#include "RTL/rtl.h"

// C/C++ include files
#include <iostream>
#include <stdexcept>

using namespace MBM;
using namespace std;

namespace  {
  struct ManagerImp: public Manager  {
    virtual int optparse(const char*)       { return 1; }
    ManagerImp()                            {           }
    ManagerImp(ServerBMID bm) : Manager(bm) {           }
    virtual ~ManagerImp()                   {           }
  };
}

/// Default constructor
BufferInfo::BufferInfo() : m_mgr(0)   {
  m_mgr = new ManagerImp();
}

/// Constructor taking already mapped buffers
BufferInfo::BufferInfo(ServerBMID bm) : m_mgr(0)   {
  m_mgr = new ManagerImp(bm);
}

/// Standard destructor
BufferInfo::~BufferInfo() {
  if ( m_mgr->m_bm->ctrl ) detach();
  delete m_mgr;
  m_mgr = 0;
}

/// Attach to buffer
int BufferInfo::attach(const char* buffer_name)    {
  if ( buffer_name )   {
    m_mgr->setup(buffer_name);
    int sc = m_mgr->mapMonitorSections();
    if (!lib_rtl_is_success(sc))   {
      return sc;
    }
    return MBM_NORMAL;
  }
  throw std::runtime_error("Invalid MBM buffer name supplied [NULL-string]");
}

/// Detach from buffer
int BufferInfo::detach()   {
  if ( m_mgr->m_bm->ctrl )   {
    int sc = m_mgr->unmapSections();
    if (!lib_rtl_is_success(sc))   {
      return sc;
    }
    return MBM_NORMAL;
  }
  return MBM_NORMAL;
}

/// Buffer name
std::string BufferInfo::name() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? m_mgr->m_bm->bm_name : "<Unknown>";
}

/// Accessor for the total number of events produces
long BufferInfo::tot_produced()  const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->tot_produced : -1;
}

/// Accessor for the total number of events actual
long BufferInfo::tot_actual()  const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->tot_actual : -1;
}

/// Accessor for the total number of events seen
long BufferInfo::tot_seen()  const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->tot_seen : -1;
}

/// Accessor for the number of event slots
long BufferInfo::max_events() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->p_emax : -1;
}

/// Accessor for the number of user slots
long BufferInfo::max_users() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->p_umax : -1;
}

/// Accessor for the buffer space in bytes
long BufferInfo::max_space() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->buff_size : -1;
}

/// Accessor for the number of the event slots used
long BufferInfo::num_events() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->i_events : -1;
}

/// Accessor for the number of the user slots used
long BufferInfo::num_users() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->i_users : -1;
}

/// Accessor for the number of the free space in bytes
long BufferInfo::num_space() const   {
  const CONTROL* c = m_mgr->m_bm->ctrl;
  return c ? c->i_space * c->bytes_p_Bit : -1;
}

template <typename FUNCTOR> long BufferInfo::loop_users(FUNCTOR& fcn, int cons, int prod, int other) const   {
  BufferMemory* dsc = m_mgr->m_bm;
  if ( dsc )   {
    int j;
    USER *us, *utst = (USER*) ~0x0;
    CONTROL* ctr = dsc->ctrl;
    for (j = 0, us = dsc->user; j < ctr->p_umax; j++, us++)    {
      if (us == utst || us == 0)
        break;
      if (us->block_id == BID_USER && us->busy == 1 )  {
        if (us->ev_actual > 0 || us->get_ev_calls > 0 || us->n_req > 0)  {
          if ( cons ) fcn(us);
        }
        else if (us->ev_produced > 0 || us->get_sp_calls > 0)  {
          if ( prod ) fcn(us);
        }
        else if ( other )  {
          fcn(us);
        }
      }
    }
    return MBM_NORMAL;
  }
  return MBM_ERROR;
}

namespace  {
  template <typename ARG> struct Counter  {
    long count;
    ARG  arg;
    Counter(ARG a=0) : count(0), arg(a) {}
    void operator()(const USER* us);
  };

  template<> void Counter<void*>::operator()(const USER* )   { ++count;  }

  struct event_type { int ev_type; event_type(int e) : ev_type(e) {} };
  template<> void Counter<event_type>::operator()(const USER* us)   {
    for (int k = 0; k < us->n_req; ++k)   {
      if ( us->req[k].ev_type == arg.ev_type )   {
        if (us->req[k].user_type == BM_REQ_ALL || us->req[k].user_type == BM_REQ_ONE )  {
          ++count;
          return;
        }
      }
    }
  }
  struct partition_id { int partid; partition_id(int p) : partid(p) {} };
  template<> void Counter<partition_id>::operator()(const USER* us)   {
    if ( us->partid == arg.partid ) {
      ++count;
    }
  }

  template<> void Counter<pair<int,int> >::operator()(const USER* us)   {
    if ( us->partid == arg.first ) {
      for (int k = 0; k < us->n_req; ++k)   {
        if ( us->req[k].ev_type == arg.second )   {
          if (us->req[k].user_type == BM_REQ_ALL || us->req[k].user_type == BM_REQ_ONE )  {
            ++count;
            return;
          }
        }
      }
    }
  }
}

/// Count the number of identified consumers in this buffer
long BufferInfo::num_consumers()  const   {
  Counter<void*> fcn;
  return MBM_NORMAL == loop_users(fcn,1,0,0) ? fcn.count : -1;
}

/// Count the number of identified consumers in this buffer with requests pending of type 'ev_type'
long BufferInfo::num_consumers_evtype(int ev_type)  const   {
  Counter<event_type> fcn(ev_type);
  return MBM_NORMAL == loop_users(fcn,1,0,0) ? fcn.count : -1;
}

/// Count the number of identified consumers in this buffer with requests pending
long BufferInfo::num_consumers_partid(int part_id)  const   {
  Counter<partition_id> fcn(part_id);
  return MBM_NORMAL == loop_users(fcn,1,0,0) ? fcn.count : -1;
}

/// Count the number of identified consumers in this buffer with requests pending
long BufferInfo::num_consumers_partid_evtype(int partid, int ev_type)  const   {
  Counter<pair<int,int> > fcn(make_pair(partid,ev_type));
  return MBM_NORMAL == loop_users(fcn,1,0,0) ? fcn.count : -1;
}

/// Count the number of identified producers in this buffer
long BufferInfo::num_producers()  const   {
  Counter<void*> fcn;
  return MBM_NORMAL == loop_users(fcn,0,1,0) ? fcn.count : -1;
}
/// Count the number of identified producers in this buffer
long BufferInfo::num_producers_partid(int part_id)  const   {
  Counter<partition_id> fcn(part_id);
  return MBM_NORMAL == loop_users(fcn,0,1,0) ? fcn.count : -1;
}

static void help() {
  cout << "mbm_info -opt [-opt]  \n"
    "         -name=<string>   Buffer name to dump information." << endl;
  ::exit(0);
}
extern "C" int mbm_bufferinfo(int argc , char** argv) {
  RTL::CLI cli(argc,argv,help);
  std::string name;
  cli.getopt("name",1,name);
  if ( name.empty() ) help();
  {
    BufferInfo info;
    if ( MBM_NORMAL == info.attach(name.c_str()) )   {
      cout << " Buffer:                         " << info.name()          << endl;
      cout << "---------------------------------------------------------" << endl;
      cout << "The total number of events produces"        << endl;
      cout << "tot_produced:                             " << info.tot_produced()  << endl;
      cout << "The total number of events actual"          << endl;
      cout << "tot_actual:                               " << info.tot_actual()    << endl;
      cout << "The total number of events seen"            << endl;
      cout << "tot_seen:                                 " << info.tot_seen()      << endl;
      cout << "The number of event slots"                  << endl;
      cout << "max_events:                               " << info.max_events()    << endl;
      cout << "The number of user slots"                   << endl;
      cout << "max_users:                                " << info.max_users()     << endl;
      cout << "The buffer space in bytes"                  << endl;
      cout << "max_space:                                " << info.max_space()     << endl;
      cout << "The number of the event slots used"         << endl;
      cout << "num_events:                               " << info.num_events()    << endl;
      cout << "The number of the user slots used"          << endl;
      cout << "num_users:                                " << info.num_users()     << endl;
      cout << "The number of the free space in bytes"      << endl;
      cout << "num_space:                                " << info.num_space()     << endl;
      cout << "The number of identified consumers in this buffer" << endl;
      cout << "num_consumers:                            " << info.num_consumers() << endl;
      cout << "num_consumers(Partition=1):               " << info.num_consumers_partid(1) << endl;
      cout << "num_consumers(EVENT_TYPE_EVENT):          " << info.num_consumers_evtype(EVENT_TYPE_EVENT) << endl;
      cout << "num_consumers(EVENT_TYPE_EVENT,1):        " << info.num_consumers_partid_evtype(1,EVENT_TYPE_EVENT) << endl;
      cout << "num_consumers(EVENT_TYPE_MEP):            " << info.num_consumers_evtype(EVENT_TYPE_MEP) << endl;
      cout << "num_consumers(EVENT_TYPE_MEP,1):          " << info.num_consumers_partid_evtype(1,EVENT_TYPE_MEP) << endl;
      cout << "The number of identified producers in this buffer" << endl;
      cout << "num_producers:                            " << info.num_producers() << endl;
      cout << "num_producers(Partition=1)                " << info.num_producers_partid(1) << endl;
    }
    else  {
      cout << " Buffer:                   " << info.name() << endl;
      cout << "--------- FAILED to attach to this buffer!" << endl;
    }
  }
  return 1;
}

