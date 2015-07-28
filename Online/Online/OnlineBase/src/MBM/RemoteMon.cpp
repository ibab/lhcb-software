/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  BUFFER MANAGER XML SERVER                            */
/*                                                                       */
/*  #   Date    Comments                                       By        */
/* -- -------- ----------------------------------------------- ---       */
/*  0  14/05/07  Initial version                               MF        */
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include "CPP/XMLStream.h"
#include "MBM/bmstruct.h"
#include "MBM/Manager.h"
#include "bm_internals.h"
#include "NET/TcpConnection.h"
#include "CPP/EventHandler.h"
#include "WT/wtdef.h"

#define E_MNF  221

namespace MBM {
  static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};
  typedef XML::Stream XMLStream;

  struct XMLMonitorServer : public EventReactor {
    struct ManagerImp : public Manager {
      virtual int  optparse (const char*) {
        return 1;
      }
      ManagerImp() {}
      virtual ~ManagerImp() {}
    };
    struct DISP_BMDES   {
      ManagerImp      m_mgr;
      BUFFERS::BUFF*  m_buff;
      DISP_BMDES() : m_buff(0) {}
      ~DISP_BMDES()  {}
    }             *m_bms;
    int           m_numBM;
    lib_rtl_gbl_t m_bm_all;
    BUFFERS*      m_buffers;
    /// Smart pointer to hold the connection object
    NetworkConnection*   m_connection;
    /// Service port
    NetworkChannel::Port m_port;
    /// Pointer to Accept Event handler
    EventHandler*      m_pAccepthandler;
    /// Pointer to TCP networkconnection
    TcpConnection*     m_tcp;
    /// Pointer to Network Channel
    TcpNetworkChannel* m_pNetwork;

    int monitor();
    void dumpBuffers(XMLStream& s);
    void dumpUser   (XMLStream& s, const USER* ctrl);
    void dumpControl(XMLStream& s, const CONTROL* ctrl);
    void getOptions(int argc, char** argv);
    virtual int optparse (const char* c);
    int get_bm_list();
    int drop_bm_list();
    XMLMonitorServer(int argc , char** argv) : m_bms(0), m_numBM(0), m_bm_all(0), m_buffers(0) {
      getOptions(argc, argv);
      ::wtc_init();
      m_tcp=new TcpConnection(10000);
      m_port=m_tcp->port();
      m_pAccepthandler = new EventHandler(this);
      m_pNetwork = &((TcpNetworkChannel&)m_tcp->recvChannel());
      m_pNetwork->queueAccept ( m_port, m_pAccepthandler );              // Rearm
    }
    virtual ~XMLMonitorServer() {
      m_pNetwork->_unqueueIO( m_tcp->port() );
      m_pNetwork->cancel();
      delete m_pAccepthandler;
    }
    virtual void handle();
    /// Reactor's Event dispatching overlayed method
    virtual int handle ( EventHandler* handler );
    /// handle Accept request
    virtual int handleAcceptRequest ( EventHandler* handler );
  };
}

void MBM::XMLMonitorServer::getOptions(int argc, char** argv)    {
  while (--argc > 0) {                                  /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
}

int MBM::XMLMonitorServer::monitor() {
  bool run=true;
  while(run)    {
    void* par;
    unsigned int fac;
    int sub_status, status;
    status = wtc_wait( &fac, &par, &sub_status );
    lib_rtl_output(LIB_RTL_ALWAYS,"Wait (%d,%d) -> %s\n", status, sub_status, lib_rtl_error_message(sub_status));
  }
  return 1;
}

void MBM::XMLMonitorServer::handle() {
}

int MBM::XMLMonitorServer::handle ( EventHandler* handler )  {
  if ( handler == m_pAccepthandler )
    return handleAcceptRequest ( handler );
  return WT_SUCCESS;
}

int MBM::XMLMonitorServer::handleAcceptRequest ( EventHandler* handler )  {
  int retry = 1;                                                   //
  NetworkChannel::Address address;                                 //
  NetworkChannel::Channel channel = m_pNetwork->accept(address);   // Accept
  int accept_error = m_pNetwork->error();                          //
  int status = m_pNetwork->queueAccept(m_port,handler);            // Rearm ACCEPT
  if ( !lib_rtl_is_success(status) )  {
    lib_rtl_output(LIB_RTL_ALWAYS,"handleAcceptRequest> Accept Rearm FAILED %d RetryCount:%d %s\n",
                   accept_error,retry,m_pNetwork->errMsg());
  }
  if ( channel > 0 )   {
    TcpNetworkChannel chan(channel);
    char buffer[1024];
    int num_byte = chan.recv(buffer,sizeof(buffer));
    // std::cout << buffer << std::endl;
    if ( num_byte <= 1 )  {           // Socket closed by Client
      chan._unqueueIO (m_port);
    }
    else {
      try {
        char reply[1024];
        std::stringstream buff;
        XML::Stream o(buff.rdbuf());
        o << XML::Stream::header() << std::endl;
        if ( strncmp(buffer,"GET /?",6)==0 ) {
          char* p = buffer + 6;
          char* q = strchr(p,' ');
          if ( q ) {
            *q = 0;
            o << XML::style(p);
          }
        }
        get_bm_list();
        dumpBuffers(o);
        std::string text = buff.str();
        ::snprintf(reply, sizeof(reply), "HTTP/1.0 200 OK\n"
                   "Content-Type: text/html\n"
                   "Content-Length: %d\n"
                   "\n",int(text.length()));
        std::string result = reply;
        result += text;
        num_byte = chan.send(result.c_str(),result.length());
        drop_bm_list();
      }
      catch(...) {
        std::cout << "Exception during buffer monitoring." << std::endl;
      }
    }
  }
  return WT_SUCCESS;
}

void MBM::XMLMonitorServer::dumpControl(XMLStream& o, const CONTROL* ctr)  {
  XML::Guard g0(o, "CONTROL");
  o << XML::item("TOT_PROD",   ctr->tot_produced);
  o << XML::item("TOT_ACTUAL", ctr->tot_actual);
  o << XML::item("TOT_SEEN",   ctr->tot_seen);
  o << XML::item("EVT_PEND",   ctr->i_events);
  o << XML::item("EVT_MAX",    ctr->p_emax);
  o << XML::item("SPC_TOT",   (ctr->bm_size*ctr->bytes_p_Bit)/1024);
  o << XML::item("SPC_FREE",  (ctr->i_space*ctr->bytes_p_Bit)/1024);
  o << XML::item("USR_TOT",    ctr->i_users);
  o << XML::item("USR_MAX",    ctr->p_umax);
}

void MBM::XMLMonitorServer::dumpUser(XMLStream& o, const USER* us) {
  char spy_val[5] = {' ',' ',' ',' ',0};
  for (int k=0; k<us->n_req; ++k )  {
    if      ( us->req[k].user_type == BM_REQ_ONE  ) spy_val[1] = '1';
    else if ( us->req[k].user_type == BM_REQ_USER ) spy_val[2] = 'U';
    else if ( us->req[k].user_type == BM_REQ_ALL  ) spy_val[3] = 'A';
    else if ( us->req[k].freq < 100.0             ) spy_val[0] = 'S';
  }
  o << XML::item("NAME", XML::text(us->name));
  o << XML::item("REQ", XML::text(spy_val));
  o << XML::item("PARTID",us->partid);
  o << XML::item("PID",   us->pid);

  if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
    o << XML::item("TYPE","Producer");
    o << XML::item("STATE", XML::text(sstat[us->state+1]));
    o << XML::item("NPROD", us->ev_produced);
  }
  else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
    o << XML::item("TYPE","Consumer");
    o << XML::item("STATE", XML::text(sstat[us->state+1]));
    o << XML::item("NSEEN", us->ev_seen);
    o << XML::item("NFREE", us->ev_freed);
  }
  else {
    o << XML::item("TYPE","Unknown");
  }
}

void MBM::XMLMonitorServer::dumpBuffers(XMLStream& o)   {
  XML::Guard top(o, "MBM_SUMMARY");
  o << XML::item("TIME", XML::text(::lib_rtl_timestr("%a %d %b %Y  %H:%M:%S",0)));
  o << XML::item("NODE", XML::text(RTL::nodeName()));
  for (int j, i=0;i<m_numBM && m_buffers->p_bmax;i++)  {
    if ( m_bms[i].m_buff != 0 )  {
      BufferMemory* dsc = m_bms[i].m_mgr.m_bm;
      USER *us, *utst=(USER*)~0x0;
      CONTROL*    ctr = dsc->ctrl;
      XML::Guard buff(o, "BUFFER", i);
      o << XML::item("NAME", XML::text(dsc->bm_name));
      dumpControl(o,ctr);
      for (j=0,us=dsc->user;j<ctr->p_umax;j++,us++)    {
        if ( us == utst || us == 0    ) break;
        if ( us->block_id != BID_USER ) continue;
        if ( us->busy     == 0        ) continue;
        XML::Guard user(o, "USER", j);
        dumpUser(o, us);
      }
    }
  }
}

int MBM::XMLMonitorServer::optparse (const char* c)  {
  switch (*c | 0x20)  {
  case '?':
  case 'h':
  default:
    lib_rtl_output(LIB_RTL_ALWAYS,"mbm_mon_serv - Buffer Manager XMLMonitorServer\n");
  lib_rtl_output(LIB_RTL_ALWAYS,"Options: None\n");
  exit(0);
  }
  return 1;
}

int MBM::XMLMonitorServer::get_bm_list()   {
  m_numBM = 0;
  int status = ::mbmsrv_map_global_buffer_info(&m_bm_all,false);
  if( !lib_rtl_is_success(status) )   {   
    lib_rtl_output(LIB_RTL_ERROR,"Cannot map global buffer information....\n");
    m_bm_all = 0;
    m_buffers = 0;
    m_bms = 0;
    return 0;
  }
  m_buffers = (BUFFERS*)m_bm_all->address;
  m_bms = new DISP_BMDES[m_buffers->p_bmax];
  for (int i = 0; i < m_buffers->p_bmax; ++i)  {
    if ( m_buffers->buffers[i].used == 1 )  {
      if ( m_bms[i].m_buff == 0 )  {
        m_bms[i].m_mgr.setup(m_buffers->buffers[i].name);
        int sc = m_bms[i].m_mgr.mapMonitorSections();
        if ( !lib_rtl_is_success(sc) )   {
          m_bms[i].m_mgr.unmapSections();
          continue;
        }
        m_bms[i].m_buff = &m_buffers->buffers[i];
      }
      m_numBM++;
    }
    else if ( m_bms[i].m_buff != 0 )  {
      m_bms[i].m_mgr.unmapSections();
      m_bms[i].m_buff = 0;
    }
  }
  return 1;
}

int MBM::XMLMonitorServer::drop_bm_list()   {
  if ( m_numBM > 0 ) {
    for (int i = 0; i < m_buffers->p_bmax; ++i)  {
      if ( m_bms[i].m_buff != 0 )  {
        m_bms[i].m_mgr.unmapSections();
        m_bms[i].m_buff = 0;
      }
    }
  }
  m_numBM = 0;
  if ( m_bm_all ) {
    ::mbmsrv_unmap_global_buffer_info(m_bm_all,false);
    m_bm_all = 0;
  }
  m_buffers = 0;
  if ( m_bms ) delete [] m_bms;
  m_bms = 0;
  return 1;
}

extern "C" int mbm_mon_serv(int argc , char** argv) {
  MBM::XMLMonitorServer mon(argc, argv);
  return mon.monitor();
}
