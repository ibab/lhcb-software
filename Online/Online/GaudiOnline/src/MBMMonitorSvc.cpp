// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMMonitorSvc.cpp,v 1.9 2010-09-21 14:28:25 frankb Exp $
//  ====================================================================
//  MBMMonitorSvc.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiOnline/MBMMonitorSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
#include "MBM/bmstruct.h"
#include "MBM/bmserver.h"
#include "MBM/bmdef.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"
#include <stdexcept>
#include <cstdio>

void LHCb::MBMMonitorSvc::Client::publish(MBMDescriptor* dsc, const std::string& user)   {
  MBMMonitorSvc* svc = dsc->svc;
  std::string u = dsc->name + std::string("/") + user;
  strncpy(name, user.c_str(), sizeof(name));
  if ( svc->deviceType() == MBMMonitorSvc::NO_DEVICES )  {
    svc->declareInfo(u+"/PID",    proc.pid,   "Process identifier");
    svc->declareInfo(u+"/Type",   proc.type,  "Process type");
    svc->declareInfo(u+"/State",  proc.state, "MBM Process state");
    svc->declareInfo(u+"/Reqs",   proc.reqs,  "Requirememnt summary");
    svc->declareInfo(u+"/PartID", partid,     "Partition identifier");
    svc->declareInfo(u+"/Events", events,     "Number of events processed");
  }
  else  {
#ifdef _USE_FAT_DEVICES
    const std::string fmt_Client = "C:4;I:4;F:2;C:32";
#else
    const std::string fmt_Client = "I:3;F:2;C:32";
#endif
    throw std::runtime_error("Cannto publish structures of type"+fmt_Client+". Not implemented in IMonitorSvc!");
    /// svc->declareInfo(u,  this,       "MBM Client information");
  }
  proc.active = 1;
  proc.inuse = 1;
  dsc->clients.insert(std::make_pair(user,this));
}

void LHCb::MBMMonitorSvc::Client::revoke(MBMDescriptor* dsc)   {
  MBMMonitorSvc* svc = dsc->svc;
  std::string u = dsc->name + std::string("/") + name;
  if ( svc->deviceType() == MBMMonitorSvc::NO_DEVICES )  {
    svc->undeclareInfo(u+"/PID");
    svc->undeclareInfo(u+"/PartID");
    svc->undeclareInfo(u+"/Type");
    svc->undeclareInfo(u+"/State");
    svc->undeclareInfo(u+"/Events");
    svc->undeclareInfo(u+"/Reqs");
  }
  else  {
    throw std::runtime_error("Cannto revoke structures. Not implemented in IMonitorSvc!");
    /// svc->undeclareInfo(u);
  }
  proc.active = 0;
  proc.inuse = 0;
}

void LHCb::MBMMonitorSvc::Client::init()   {
  proc.active = 0;
  proc.inuse  = 0;
}

void LHCb::MBMMonitorSvc::Client::read(MBMDescriptor* /* dsc */ , const USER& us)   {
  proc.reqs   = 0;
  proc.type   = 0;
  proc.active = 1;
  proc.pid    = us.pid;
  partid      = us.partid;
  if ( us.ev_produced>0 || us.get_sp_calls>0 )   {
    proc.type  = 1<<0;
    proc.state = char(us.state);
    events     = us.ev_produced;
  }
  else if ( us.ev_actual>0 || us.get_ev_calls>0 || us.n_req>0 ) {
    proc.type  = 1<<1;
    proc.state = char(us.state);
    events = us.ev_seen;
    for (int k=0; k<us.n_req; ++k )  {
      if      ( us.req[k].user_type == BM_REQ_ONE  ) proc.reqs |= 1<<0;
      else if ( us.req[k].user_type == BM_REQ_USER ) proc.reqs |= 1<<1;
      else if ( us.req[k].user_type == BM_REQ_ALL  ) proc.reqs |= 1<<2;
      else if ( us.req[k].freq < 100.0             ) proc.reqs |= 1<<3;
    }
  }
}

void LHCb::MBMMonitorSvc::Info::publish(const std::string& us)   {
  if ( dsc->clients.find(us) == dsc->clients.end() )  {
    for(size_t i=0; i<sizeof(dsc->info.clients)/sizeof(dsc->info.clients[0]); ++i)  {
      if ( dsc->info.clients[i].proc.active == 0 )   {
        dsc->info.clients[i].publish(dsc,us);
        return;
      }
    }
  }
  /// Error - no client slot availible
}

void LHCb::MBMMonitorSvc::Info::publish()   {
  MBMMonitorSvc* svc = dsc->svc;
  std::string b = dsc->name;
  std::string d = "Buffer "+b+": ";
  ::strncpy(bmid, b.c_str(), sizeof(bmid));
  if ( svc->deviceType() == MBMMonitorSvc::NO_DEVICES )  {
    svc->declareInfo(b+"/EvtProduced", evtProduced, b+"Number of events produced");
    svc->declareInfo(b+"/EvtPending",  evtPending,  d+"Number of events pending");
    svc->declareInfo(b+"/EvtSeen",     evtSeen,     d+"Number of events seen");
    svc->declareInfo(b+"/EvtMax",      evtMax,      d+"Maximal Number of events");
    svc->declareInfo(b+"/SpaceTotal",  spaceTotal,  d+"Amount of free buffer space");
    svc->declareInfo(b+"/SpaceFree",   spaceFree,   d+"Amount of total buffer space");
    svc->declareInfo(b+"/UserTotal",   userTotal,   d+"Total number of users");
    svc->declareInfo(b+"/userMax",     userMax,     d+"Maximal number of users");
    //svc->declareInfo(b+"/BMID",        bmid,        d+"Buffer identifier");
  }
  else  {
    const std::string fmt_BM = "I:8;C:32";
    throw std::runtime_error("Cannto publish structures of type"+fmt_BM+". Not implemented in IMonitorSvc!");
    /// svc->declareInfo(u,  this,       "MBM Client information");
  }
}

void LHCb::MBMMonitorSvc::Info::revoke()   {
  std::string b = dsc->name;
  MBMMonitorSvc* svc = dsc->svc;
  svc->undeclareInfo(b+"/EvtProduced");
  svc->undeclareInfo(b+"/EvtPending");
  svc->undeclareInfo(b+"/EvtSeen");
  svc->undeclareInfo(b+"/EvtMax");
  svc->undeclareInfo(b+"/SpaceTotal");
  svc->undeclareInfo(b+"/SpaceFree");
  svc->undeclareInfo(b+"/UserTotal");
  svc->undeclareInfo(b+"/userMax");
  svc->undeclareInfo(b+"/BMID");
}

void LHCb::MBMMonitorSvc::Info::revoke(const char* us)   {
  MBMClients::iterator ic = dsc->clients.find(us);
  if ( ic != dsc->clients.end() )  {
    (*ic).second->revoke(dsc);
    dsc->clients.erase(ic);
  }
}

void LHCb::MBMMonitorSvc::Info::read()   {
  CONTROL* ctr = dsc->id->ctrl;
  evtProduced = ctr->tot_produced;
  evtPending  = ctr->i_events;
  evtSeen     = ctr->tot_seen;
  evtMax      = ctr->p_emax;
  spaceTotal  = (ctr->bm_size*ctr->bytes_p_Bit)/1024;
  spaceFree   = (ctr->i_space*ctr->bytes_p_Bit)/1024;
  userTotal   = ctr->i_users;
  userMax     = ctr->p_umax;
  dsc->active = 1;
}

void LHCb::MBMMonitorSvc::Info::resetClients()   {
  for(size_t k=0,nc=sizeof(clients)/sizeof(clients[0]); k<nc; ++k)
    clients[k].proc.active = 0;
}

void LHCb::MBMMonitorSvc::Info::checkClients()   {
  for(size_t k=0,nc=sizeof(clients)/sizeof(clients[0]); k<nc; ++k)   {
    Client& c = clients[k];
    if ( c.proc.inuse == 1 && c.proc.active == 0 )   {
      MBMClients::iterator ic = dsc->clients.find(c.name);
      if ( ic != dsc->clients.end() )  {
        dsc->clients.erase(ic);
      }
      c.revoke(dsc);
    }
  }
}

LHCb::MBMMonitorSvc::MBMDescriptor::MBMDescriptor() : active(0), name(0), svc(0) {
  info.dsc = this;
  for(size_t k=0,nc=sizeof(info.clients)/sizeof(info.clients[0]); k<nc; ++k)
    info.clients[k].init();
}

int LHCb::MBMMonitorSvc::MBMDescriptor::map()  {
  if ( id == 0 ) {
    id = new ServerBMID_t();
    if ( MBM_NORMAL == ::mbmsrv_map_mon_memory(name,id) ) {
      active = 1;
      return 1;   
    }
  }
  return 0;
}

void LHCb::MBMMonitorSvc::MBMDescriptor::unmap()  {
  if ( id )   {
    revoke();
    ::mbmsrv_unmap_memory(id);
    delete id;
    id = 0;
    active  = 0;
    name = 0;
  }
}

void LHCb::MBMMonitorSvc::MBMDescriptor::publish()   {
  info.publish();
  for(USER::iterator i(id->user,id->ctrl->p_umax); i; ++i) {
    if ( clients.find((*i).name) == clients.end() )
      info.publish((*i).name);
  }
}

void LHCb::MBMMonitorSvc::MBMDescriptor::revoke()   {
  info.revoke();
  for(USER::iterator i(id->user,id->ctrl->p_umax); i; ++i) {
    MBMClients::iterator ic = clients.find((*i).name);
    if ( ic != clients.end() )
      info.revoke((*i).name);
  }
  clients.clear();
}

void LHCb::MBMMonitorSvc::MBMDescriptor::read()   {
  MsgStream log(svc->msgSvc(), svc->name());
  log << MSG::DEBUG << "Reading clients of buffer:" << name << endmsg;
  info.read();
  info.resetClients();
  for(USER::iterator i(id->user,id->ctrl->p_umax); i; ++i)  {
    MBMClients::iterator j=clients.find((*i).name);
    if(j == clients.end()) {
      info.publish((*i).name);
      j=clients.find((*i).name);
    }
    log << "Reading client info of   :" << (*i).name << endmsg;
    (*j).second->read(this,*i);
  }
  log << "Finished reading clients of buffer:" << name << endmsg;
  info.checkClients();
}

/// Standard service constructor
LHCb::MBMMonitorSvc::MBMMonitorSvc(const std::string& nam, ISvcLocator* loc)    
: OnlineService(nam, loc), m_bm_all(0), m_alarmID(0)
{
  declareProperty("DeviceType",       m_deviceType  = NO_DEVICES);
  declareProperty("TimerFacility",    m_timerFac    = WT_FACILITY_TIMER1);
}

/// Default destructor
LHCb::MBMMonitorSvc::~MBMMonitorSvc()    {
}

/// Query interfaces of Interface
StatusCode LHCb::MBMMonitorSvc::queryInterface(const InterfaceID& riid,
                                            void** ppvInterface) 
{
  if ( IMBMMonitorSvc::interfaceID().versionMatch(riid) )   {
    *ppvInterface = (IMBMMonitorSvc*)this;
    addRef();
    return SUCCESS;
  }
  return OnlineService::queryInterface(riid,ppvInterface);
}

int LHCb::MBMMonitorSvc::timerAst(void* par)  {
  MBMMonitorSvc* svc = (MBMMonitorSvc*)par;
  svc->m_alarmID = 0;
  return ::wtc_insert(svc->m_timerFac, par);
}

int LHCb::MBMMonitorSvc::timerRearm(unsigned int /* fac */, void* par)  {
  MBMMonitorSvc* svc = (MBMMonitorSvc*)par;
  int sc = ::lib_rtl_set_timer(3000, timerAst, par, &svc->m_alarmID);
  return lib_rtl_is_success(sc) ? WT_SUCCESS : WT_ERROR;
}

int LHCb::MBMMonitorSvc::timerAction(unsigned int /* fac */, void* par) {
  MBMMonitorSvc* svc = (MBMMonitorSvc*)par;
  svc->mapBuffers();
  svc->read();
  return WT_SUCCESS;
}

void LHCb::MBMMonitorSvc::publish()   {
  for(MBMBuffers::iterator i=m_buffers.begin(); i!=m_buffers.end(); ++i)
    (*i).second->publish();
}

void LHCb::MBMMonitorSvc::revoke()   {
  for(MBMBuffers::iterator i=m_buffers.begin(); i!=m_buffers.end(); ++i)
    (*i).second->revoke();
}

void LHCb::MBMMonitorSvc::read()   {
  for(MBMBuffers::iterator i=m_buffers.begin(); i!=m_buffers.end(); ++i)
    (*i).second->read();
}

void LHCb::MBMMonitorSvc::mapBuffers()   {
  BUFFERS* b = (BUFFERS*)m_bm_all->address;
  for (int i = 0; i < b->p_bmax; ++i)  {
    const char* nam = b->buffers[i].name;
    if ( b->buffers[i].used == 1 )  {
      if ( m_bms[i].map() != MBM_NORMAL )
        continue;
      if ( m_buffers.find(nam) != m_buffers.end() )
        continue;
      m_buffers.insert(std::make_pair(nam,&m_bms[i]));
    }
    else if ( m_bms[i].id != 0 && m_buffers.find(nam) != m_buffers.end() )  {
      m_bms[i].revoke();
      m_bms[i].unmap();
      m_buffers.erase(m_buffers.find(nam));
    }
  }
}

void LHCb::MBMMonitorSvc::unmapBuffers()   {
  for(MBMBuffers::iterator i=m_buffers.begin(); i!=m_buffers.end(); ++i)  {
    (*i).second->unmap();
  }
  m_buffers.clear();
}

StatusCode LHCb::MBMMonitorSvc::initialize()  {
  StatusCode sc = OnlineService::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class OnlineService.");
  }
  int status = ::mbm_map_global_buffer_info(&m_bm_all);
  if(!lib_rtl_is_success(status))   {   
    return error("Cannot map global buffer information....");
  }
  m_buf = (BUFFERS*)m_bm_all->address;
  m_bms = new MBMDescriptor[m_buf->p_bmax];
  for(int i=0; i<m_buf->p_bmax; ++i)  {
    m_bms[i].name = m_buf->buffers[i].name;
    m_bms[i].svc  = this;
  }
  mapBuffers();
  publish();
  status = ::wtc_subscribe( m_timerFac, timerRearm, timerAction, this);
  if( status != WT_SUCCESS )  {
    return error("Cannot subscribe to WT timer facility.");
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MBMMonitorSvc::finalize()  {
  int status = ::wtc_flush(m_timerFac);
  status = ::wtc_remove(m_timerFac);
  revoke();
  unmapBuffers();
  status = ::mbm_unmap_global_buffer_info(m_bm_all);
  if(!lib_rtl_is_success(status))   {   
    error("Cannot unmap global buffer information....");
  }
  if ( m_alarmID != 0 )  {
    status = ::lib_rtl_kill_timer(m_alarmID);
    if(!lib_rtl_is_success(status))   {   
      error("Cannot unsubscribe timer.");
    }
  }
  return OnlineService::finalize();
}
