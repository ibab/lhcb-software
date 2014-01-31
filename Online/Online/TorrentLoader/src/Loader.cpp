// $Id: Monitor.h,v 1.6 2008-11-13 12:13:32 frankb Exp $
//====================================================================
//  Online
//--------------------------------------------------------------------
//
//  Package    : TorrentLoader
//
//  Structure of basic items published by the readout monitoring.
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header:$
#define WITH_SHIPPED_GEOIP_H

// This is a pure hack, but I do not know how to get out of this otherwise.
// Somehow the LCG boost libraries are not what libtorrent expects really
#ifdef  BOOST_FILESYSTEM_VERSION
//#undef  BOOST_FILESYSTEM_VERSION
//#define BOOST_FILESYSTEM_VERSION 2
#endif
//============= End-of-hack =============

#include "TorrentLoader/Loader.h"
#include "ROMon/TorrentMon.h"

#include "RTL/time.h"
#include "RTL/Lock.h"
#include "CPP/IocSensor.h"
#include "CPP/TimeSensor.h"
extern "C" {
#include "dis.h"
#include "dic.h"
}
#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/GeoIP.h"
#include "libtorrent/ip_filter.hpp"

#include <stdexcept>
#include <cstdlib>
#include <cerrno>
#ifdef _WIN32
#else
#include <libgen.h>
#endif

namespace ROMon {
  void print_startup(const char* msg);
  std::string strupper(const std::string& n);
}

using namespace std;
using namespace ROMon;
using namespace BitTorrent;
using namespace libtorrent;
template <class T> inline void deletePtr(T*& p) { if(p) delete p;p=0; }

#define WORKER_SPACE "/dev/shm"
#define MBYTE (1024*1024)

enum {
  CMD_COMMAND_REQUEST,
  CMD_COMMAND_DONE,
  CMD_COMMAND_FAILED,
  CMD_COMMAND_PARENT,
  CMD_ADD_TORRENT,
  CMD_REMOVE_TORRENT,
  CMD_MONITOR_TORRENT,
  LAST
};
namespace {
  struct DimLock {
    DimLock() { dim_lock(); }
    ~DimLock() { dim_unlock(); }
  };
}

/// Static printer (RTL overload)
static size_t printout(void* context, int level, const char* fmt, va_list args)  {
  long lvl = (long)context;
  if ( level >= lvl ) {
    size_t len = ::vfprintf(stdout,fmt, args);
    ::fflush(stdout);
    return len;
  }
  return 0;
}

static void help() {
  ::printf("usage:  TorrentLoader -opt=<value> [-opt=<value>]                                 \n"
	   "   -first_port=<number>   (optional)   First port number to be used               \n"
	   "   -last_port=<number>    (optional)   Last  port number to be used               \n"
	   "   -interface=0.0.0.0     (mandatory)  NIC interface address                      \n"
	   "   -save_to=<directory>   (mandatory)  Directory where the datafile is saved      \n"
	   "                                      for seeders: value=seeder!                  \n"
	   "   -announce=<url>        (mandatory)  Tracker url(s), separated by commas        \n"
	   "   -parent=<DIM-name>     (optional)   DIM server to support multi tier downloads \n"
	   "   -upload_max=<number>   (optional)   Upload limit in MBytes/second              \n"
	   "   -download_max=<number> (optional)   Download limit in MBytes/second            \n"
	   "   -monitoring            (optional)   Enable monitoring via DIM                  \n"
	   "   -mask=<number>         (optional)   Set error mask for printing torrent alerts.\n"
	   "   -tier=<number>         (mandatory)  Tier number (0,1,2)                        \n"
	   "   -print=<number>        (optional)   Set printout level 1=VERBOSE,6=FATAL       \n"
	   "\n");
  ::exit(0);
}

#define ERR_ENABLED(x)  ((m_alertConfig&code_##x)==code_##x)
#define ENABLE_ALERT(x)  m_alertConfig |= code_##x

namespace BitTorrent {
  struct NetworkFilter : public ip_filter {
    NetworkFilter() : ip_filter() {}
    long node_addr(const string& name)  const;
    long node_addr_net(const string& name)  const;
    string ntoa(const string& name)  const;
    void add_host(const string& addr, int flag);
    void add_range(unsigned int first, unsigned int last, int flag);
    void add_subnet(const string& host, unsigned int first, unsigned int last, int flag);
  };
}

string NetworkFilter::ntoa(const string& node)   const {
  in_addr in;
  in.s_addr = node_addr_net(node);
  return ::inet_ntoa(in);
}

long NetworkFilter::node_addr_net(const string& name)   const   {
  long add = node_addr(name);
  return htonl(add);
}

long NetworkFilter::node_addr(const string& name)   const  {
  hostent* entry = ::gethostbyname(name.c_str());
  if ( entry ) {
    in_addr* inadd = (in_addr*)entry->h_addr;
    long     host  = ntohl(inadd->s_addr);
    return host;
  }
  ::lib_rtl_output(LIB_RTL_ERROR,"Failed to resolve host address for host:%s.\n",
		   name.c_str(), RTL::errorString());
  return 0;
}

void NetworkFilter::add_host(const string& addr, int flag) {
  long host = node_addr(addr);
  if ( host != 0 ) {
    this->ip_filter::add_rule(address(address_v4(host)), address(address_v4(host)), flag);
  }
  else {
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to build networkfilter '%s' for host:%s.\n",
		     flag ? "BLOCKING" : "ALLOW", addr.c_str(),
		     RTL::errorString());
  }
}

void NetworkFilter::add_subnet(const string& host_name, unsigned int first, unsigned int last, int flag) {
  long host = node_addr(host_name);
  int lflag = last;
  if      ( first<0x000000FF ) first += 0xFFFFFF00;
  else if ( first<0x0000FFFF ) first += 0xFFFF0000;
  else if ( first<0x00FFFFFF ) first += 0xFF000000;

  if      ( lflag<0x000000FF ) lflag += 0xFFFFFF00;
  else if ( lflag<0x0000FFFF ) lflag += 0xFFFF0000;
  else if ( lflag<0x00FFFFFF ) lflag += 0xFF000000;
  if ( host )   {
    in_addr in;
    string sf , sl;
    long f = host&first,l = (host&lflag) | last;
    in.s_addr = htonl(f);
    sf = inet_ntoa(in);
    in.s_addr = htonl(l);
    sl = inet_ntoa(in);
    ::lib_rtl_output(LIB_RTL_INFO,"Add subnet: %s -- %08X (%s) -> %08X (%s)\n",
		     host_name.c_str(), f, sf.c_str(), l, sl.c_str());
    this->ip_filter::add_rule(address(address_v4(f)), address(address_v4(l)), flag);
  }
}

void NetworkFilter::add_range(unsigned int first, unsigned int last, int flag) {
  this->ip_filter::add_rule(address(address_v4(first)), address(address_v4(last)), flag);
}

Main::Main(int argc, char** argv)
: m_session(0), m_publish(0)
{
  enum { ALLOWED=0, BLOCKED=ip_filter::blocked };
  NetworkFilter filter;
  string node = RTL::nodeNameShort(), controls_pc = node;
  string interface("0.0.0.0"), dir(WORKER_SPACE), publish("ecs04");
  string farm_node, seeder, tracker("plus02"), trk, msk;
  unsigned short first_port = 6881, last_port = 6889;
  int upload = 0, download = 0, tier=2;
  long print = LIB_RTL_WARNING;
  unsigned long mask = 0;
  vector<string> trackers, seeders;
  RTL::CLI cli(argc,argv,help);
  bool monitoring = cli.getopt("monitoring",1);
  cli.getopt("first_port",   2,first_port);
  cli.getopt("interface",    2,interface);
  cli.getopt("last_port",    2,last_port);
  cli.getopt("tracker",      2,tracker);
  cli.getopt("seeder",       2,seeder  = "plus10");
  cli.getopt("upload_max",   2,upload);
  cli.getopt("download_max", 2,download);
  cli.getopt("save_to",      2,dir);
  cli.getopt("print",        2,print);
  cli.getopt("publish",      2,publish);
  cli.getopt("tier",         2,tier);
  cli.getopt("mask",         2,msk);
  ::lib_rtl_install_printer(printout,(void*)print);

  if ( !msk.empty() ) {
    if ( 1 != ::sscanf(msk.c_str(),"0x%lX",&mask) ) {
      ::lib_rtl_output(LIB_RTL_ERROR,"Failed to read alert mask: %s\n",RTL::errorString());
    }
  }

  // Define DIM connectivity
  if ( tier == 0 ) {
    ::dis_set_dns_node((char*)publish.c_str());
    ::dic_set_dns_node((char*)publish.c_str());
  }
  else if ( tier == 1 ) {  // SF controls PC
    controls_pc = node;
    farm_node   = node+"01";
    ::dis_set_dns_node((char*)controls_pc.c_str());
    ::dic_set_dns_node((char*)publish.c_str());
  }
  else if ( tier == 2 ) {  // Worker node
    farm_node = node;
    controls_pc = ::getenv("DIM_DNS_NODE");
    ::dis_set_dns_node((char*)controls_pc.c_str());
    ::dic_set_dns_node((char*)controls_pc.c_str());
  }

  // Setup IP filter
  filter.add_range (0,0xFFFFFFFF, BLOCKED);
  trk = seeder;
  for(size_t idx=trk.find(','); idx != string::npos; idx=trk.find(','))   {
    string tmp = trk;
    trk = tmp.substr(idx+1);
    tmp = tmp.substr(0,idx);
    seeders.push_back(tmp);
    filter.add_host  (tmp, ALLOWED);
    ::lib_rtl_output(LIB_RTL_INFO,"Seeder: Allow access to:%s - %s\n",tmp.c_str(),filter.ntoa(tmp).c_str());
  }
  if ( !trk.empty() )  {
    seeders.push_back(trk);
    filter.add_host  (trk, ALLOWED);
    ::lib_rtl_output(LIB_RTL_INFO,"Seeder: Allow access to:%s - %s\n",trk.c_str(),filter.ntoa(trk).c_str());
  }
  //filter.add_host  (seeder,  ALLOWED);
  //::lib_rtl_output(LIB_RTL_INFO,"Seeder:  Allow access to:%s - %s\n",seeder.c_str(),filter.ntoa(seeder).c_str());

  // Setup tracker list and fill IP filter
  trk = tracker;
  for(size_t idx=trk.find(','); idx != string::npos; idx=trk.find(','))   {
    string tmp = trk;
    trk = tmp.substr(idx+1);
    tmp = tmp.substr(0,idx);
    trackers.push_back("http://"+tmp+":6969/announce");
    filter.add_host  (tmp, ALLOWED);
    ::lib_rtl_output(LIB_RTL_INFO,"Tracker: Allow access to:%s - %s\n",tmp.c_str(),filter.ntoa(tmp).c_str());
  }
  if ( !trk.empty() )  {
    trackers.push_back("http://"+trk+":6969/announce");
    filter.add_host  (trk, ALLOWED);
    ::lib_rtl_output(LIB_RTL_INFO,"Tracker: Allow access to:%s - %s\n",trk.c_str(),filter.ntoa(trk).c_str());
  }

  if ( tier <= 1 ) {  // Main seeder and SF controls PC
    char host_name[64], text[64];
    ::gethostname(host_name,sizeof(host_name));
    if ( tier == 0 || ::strncasecmp(host_name,"hlt",3) == 0 )   {
      const char rows[] = {'a','b','c','d','e','f',0};
      if ( strncasecmp(node.c_str(),"hlt",3) == 0 || tier==0 ) {
	// Add all farm controls pc:
	for(const char* r=rows; *r; ++r)  {
	  for(int j=1; j<12; ++j) {
	    if ( j==5 ) continue;
	    if ( *r=='f' && j>7 ) continue;
	    ::sprintf(text,"hlt%c%02d",*r,j);
	    filter.add_host(text, ALLOWED);
	  }
	}
      }
    }
    if ( tier == 0 )   {
      filter.add_subnet("mona0901", 0x01, 0xFE, ALLOWED);
    }
  }

  if ( tier >= 1 ) {  // Subfarm 
    filter.add_subnet(farm_node, 0x01, 0xFE, ALLOWED);
  }
  // allow the workers also to talk to neighboring subfarm controllers
  if ( tier == 2 && ::toupper(farm_node[0])=='H' ) {
    const char rows[] = {'a','b','c','d','e','f',0};
    string tmp = farm_node;
    for(const char* r=rows; *r; ++r)  {
      tmp[3] = *r;
      filter.add_host(tmp,ALLOWED);
    }
#if 0
    char c = ::tolower(tmp[3]);
    tmp[3] = char(c < 'f' ? c+1 : 'a');
    filter.add_host(tmp,ALLOWED);
    tmp[3] = char(c < 'e' ? c+2 : c=='e' ? 'a' : 'b');
    filter.add_host(tmp,ALLOWED);
#endif
  }

  // Now build the main objects
  string   nam = "/" + strupper(node) + "/" + RTL::processName();
  Session* session = new Session  (this, tier, monitoring, dir, interface, first_port, last_port, trackers);
  for(vector<string>::iterator i=seeders.begin();i!=seeders.end();++i)
    m_command.push_back(new Command  (this, "/"+(*i)+"/TorrentLoader", nam));
  m_publish = new Publisher(this, nam);
  m_session = session;
  session->setDownloadLimit(download);
  session->setUploadLimit(upload);
  session->setIPFilter(filter);
  session->setAlertMask(mask);
  session->start();
}

/// Default destructor
Main::~Main()   {
  deletePtr(m_session);
  deletePtr(m_publish);
  for(vector<Interactor*>::iterator i=m_command.begin();i!=m_command.end();++i)
    delete (*i);
  m_command.clear();
}

int Main::run() {
  string n = "/" + ROMon::strupper(RTL::nodeNameShort()) + "/" + RTL::processName();
  ::dis_start_serving((char*)n.c_str());
  IocSensor::instance().run();
  return 1;
}

/// Interactor overload: Handle external interrupts
void Main::handle(const Event& event) {
  IocSensor& ioc = IocSensor::instance();
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type) {
    case CMD_COMMAND_DONE:
    case CMD_COMMAND_FAILED:
    case CMD_COMMAND_PARENT:
      // Forward the request to the Command handler
      for(vector<Interactor*>::iterator i=m_command.begin();i!=m_command.end();++i)
	ioc.send(*i,event.type,new string(*event.iocPtr<string>()));
      // Remove the torrent
      if ( event.type == CMD_COMMAND_DONE ) {
	ioc.send(this,CMD_REMOVE_TORRENT,new string("REMOVE:"+(*event.iocPtr<string>())));
      }
      delete event.iocPtr<string>();
      return;
    case CMD_MONITOR_TORRENT:
      ioc.send(m_publish,event.type,event.iocPtr<SessionStatus>());
      return;
    case CMD_REMOVE_TORRENT:
    case CMD_ADD_TORRENT:
      ioc.send(m_session,event.type,event.iocPtr<string>());
      return;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/// Standard constructor
Publisher::Publisher(Interactor* parent, const std::string& nam)
  : m_parent(parent), m_status(0), m_svcID(0)
{
  m_svcID = ::dis_add_service((char*)(nam+"/monitor").c_str(),(char*)"C",0,0,feedMonitor,(long)this);
}

/// Default destructor
Publisher::~Publisher() {
  if ( m_svcID ) ::dis_remove_service(m_svcID);
  if ( m_status ) m_status->free();
  m_status = 0;
  m_svcID = 0;
}

/// Dim callback to place information datablocks
void Publisher::feedMonitor(void* tag, void** address, int* size, int* first) {
  static const char* empty = "";
  if ( !(*first) ) {
    Publisher* cmd = *(Publisher**)tag;
    if ( cmd && cmd->m_status ) {
      *address = (void*)cmd->m_status;
      *size    = cmd->m_status->length();
      return;
    }
  }
  *size = 0;
  *address = (void*)empty;
}

/// Monitor single torrent data
void Publisher::monitorTorrent(const stats_alert& alert) {
  if ( m_svcID && alert.handle.is_valid() )    {
    // build the monitoring block for this torrent
    
  }
}

/// Interactor overload: Handle external interrupts
void Publisher::handle(const Event& event) {
  auto_ptr<string> str;
  switch(event.eventtype) {
  case ScrMouseEvent:
    return;
  case IocEvent:
    switch(event.type) {
    case CMD_MONITOR_TORRENT:
      ::dim_lock();
      if ( m_status ) m_status->free();
      m_status = event.iocPtr<SessionStatus>();
      ::dim_unlock();
      ::dis_update_service(m_svcID);
      return;
    default:
      ::lib_rtl_output(LIB_RTL_ERROR,"Got spurious monitoring request:%d\n",event.type);
      break;
    }
    break;
  default:
    break;
  }
}

/// Standard constructor
Command::Command(Interactor* handler, const string& parent, const std::string& nam)
  : m_parent(parent), m_svcID(0), m_cmdID(0), m_handler(handler)
{
  static int mm_svcID=0, mm_cmdID=0;
  if ( !mm_svcID ) mm_svcID = ::dis_add_service((char*)(nam+"/status").c_str(),(char*)"C",0,0,feedStatus,(long)this);
  if ( !mm_cmdID ) mm_cmdID = ::dis_add_cmnd((char*)(nam+"/command").c_str(),(char*)"C",handleCommand,(long)this);
  m_svcID = mm_svcID;
  m_cmdID = mm_cmdID;
}

/// Default destructor
Command::~Command() {
  if ( m_svcID ) ::dis_remove_service(m_svcID);
  if ( m_cmdID ) ::dis_remove_service(m_cmdID);
  m_cmdID = m_svcID = 0;
}

/// DIM command service callback
void Command::handleCommand(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    Command* cmd = *(Command**)tag;
    ((char*)address)[*size] = 0;
    IocSensor::instance().send(cmd,CMD_COMMAND_REQUEST,new string((const char*)address));
  }
}

/// DIM service handler callback
void Command::feedStatus(void* tag, void** address, int* size, int* first) {
  static const char* empty = "";
  if ( !(*first) ) {
    Command* cmd = *(Command**)tag;
    *address = (void*)cmd->m_reply.c_str();
    *size    = cmd->m_reply.length()+1;
    return;
  }
  *size = 0;
  *address = (void*)empty;
}

/// Interactor overload: Handle external interrupts
void Command::handle(const Event& event) {
  string s;
  auto_ptr<string> str;
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type) {

    case CMD_COMMAND_REQUEST:
      str = auto_ptr<string>(event.iocPtr<string>());
      s = *str;
      if ( toupper(s[0])=='A' )
	IocSensor::instance().send(m_handler,CMD_ADD_TORRENT,new string(s.substr(s.find(':')+1)));
      else if ( toupper(s[0])=='R' )
	IocSensor::instance().send(m_handler,CMD_REMOVE_TORRENT,new string(s.substr(s.find(':')+1)));
      else
	::lib_rtl_output(LIB_RTL_ERROR,"Received unknown IOC command: %s\n",s.c_str());
      return;

    case CMD_COMMAND_DONE:
      str = auto_ptr<string>(event.iocPtr<string>());
      m_reply = "done:"+ (*str);
      ::dis_update_service(m_svcID);
      return;

    case CMD_COMMAND_FAILED:
      str = auto_ptr<string>(event.iocPtr<string>());
      m_reply = "failed:"+ (*str);
      ::dis_update_service(m_svcID);
      return;

    case CMD_COMMAND_PARENT:
      if ( !m_parent.empty() ) {
	str = auto_ptr<string>(event.iocPtr<string>());
	string cmd = "add:"+*str;
	int res = ::dic_cmnd_service((char*)(m_parent+"/command").c_str(),(void*)cmd.c_str(),cmd.length()+1);
	if ( res != 1 ) {
	  ::lib_rtl_output(LIB_RTL_FATAL,"Failed to notify parent '%s' to load torrent %s\n",
			   m_parent.c_str(),cmd.c_str());
	}
      }
      return;

    default:
      ::lib_rtl_output(LIB_RTL_ERROR,"Received unknown IOC command: type= %d\n",event.type);
      break;
    }
    break;
  default:
    ::lib_rtl_output(LIB_RTL_ERROR,"Received unknown SENSOR interrupt: event type=%08X\n",event.eventtype);
    break;
  }
}

/// Initializing constructor
Session::Session(Interactor*     parent,
		 int             tier,
		 bool            enable_monitoring,
		 const string&   save_to,
		 const string&   interface, 
		 unsigned short  first_port, 
		 unsigned short  last_port,
		 const Trackers& trackers)
  : m_parent(parent), m_session(0), m_thread(0), m_lock(0), m_tier(tier), m_saveDir(save_to), m_trackers(trackers)
{
  m_alertConfig = 0;
  m_NUM_hash_failed_alert = 0;
  //ENABLE_ALERT(torrent_finished_alert);
  ENABLE_ALERT(listen_failed_alert);
  ENABLE_ALERT(tracker_warning_alert);
  ENABLE_ALERT(torrent_delete_failed_alert);
  ENABLE_ALERT(file_rename_failed_alert);
  ENABLE_ALERT(file_renamed_alert);
  ENABLE_ALERT(peer_connect_alert);
  //ENABLE_ALERT(peer_blocked_alert);
  ENABLE_ALERT(hash_failed_alert);

  //ENABLE_ALERT();
  m_monitor = enable_monitoring ? 1 : 0;
  m_status  = SessionStatus::allocate(0);
  m_session = new session(fingerprint("LT",LIBTORRENT_VERSION_MAJOR,LIBTORRENT_VERSION_MINOR,0,0),
			  make_pair(first_port,last_port), 
			  interface.c_str(),
			  session::start_default_features | 
			  session::add_default_plugins,
			  alert::error_notification |
			  alert::peer_notification |
			  alert::tracker_notification |
			  alert::status_notification |
			  alert::ip_block_notification |
			  alert::progress_notification );

  session_settings settings = high_performance_seed();
  settings.local_service_announce_interval = 5;
  settings.unchoke_interval = 5;

  m_session->set_settings(settings);
  m_session->start_lsd();
  setSessionInfo();
  int sc = ::lib_rtl_create_lock(0,&m_lock);
  if ( !lib_rtl_is_success(sc) ) {
    ::lib_rtl_output(LIB_RTL_FATAL,"Session> Failed to create lock:%s\n",RTL::errorString());
    throw runtime_error("Session> Failed to create lock.");
  }
}

/// Default destructor
Session::~Session() {
  if ( m_thread ) {
    ::lib_rtl_delete_thread(m_thread);
    m_thread = 0;
  }
  if ( m_lock ) {
    ::lib_rtl_delete_lock(m_lock);
    m_lock = 0;
  }
  removeAllTorrents();
  deletePtr(m_session);
}

/// Start execution
void Session::start() {
  if ( 0 == m_thread ) {
    int sc = ::lib_rtl_start_thread(eventPoll,this,&m_thread);
    if ( !lib_rtl_is_success(sc) ) {
      ::lib_rtl_output(LIB_RTL_FATAL,
		       "Session> Failed to start session event poll thread:%s\n",
		       RTL::errorString());
      throw runtime_error("Session> Failed to start session event poll thread.");
    }
    ::lib_rtl_output(LIB_RTL_INFO,"Session with %d tracker(s) started. Data directory:%s Tier:%d\n",
		     int(m_trackers.size()),m_saveDir.c_str(),m_tier);
    updateMonitoring();
    return;
  }
  ::lib_rtl_output(LIB_RTL_FATAL,"This Session was already started. Don't start it twice bugger!  -->exit\n");
  delete this;
  ::exit(0xDEAD);
}

/// Set IP filter
void Session::setIPFilter(const NetworkFilter& filter) {
  m_session->set_ip_filter(filter);
}

void Session::updateMonitoring()  const {
  IocSensor::instance().send((Interactor*)this,CMD_MONITOR_TORRENT,(void*)0);
}

/// Update the session monitoring block. Does 
void Session::setSessionInfo()   const   {
  timeval tv={0,0};
  ::gettimeofday(&tv,0);
  session_status s = m_session->status();
  cache_status   c = m_session->get_cache_status();
  m_status->num_peers        = m_session->num_connections();
  m_status->blocks_written   = c.blocks_written;
  m_status->blocks_read      = c.blocks_read;
  m_status->blocks_read_hit  = c.blocks_read_hit;
  m_status->upload_rate      = s.upload_rate;
  m_status->download_rate    = s.download_rate;
  m_status->total_upload     = s.total_upload;
  m_status->total_download   = s.total_download;
  m_status->time             = tv.tv_sec;
  m_status->millitm          = tv.tv_usec/1000;
  ::memset(m_status->name,0,sizeof(m_status->name));
  ::strncpy(m_status->name,RTL::nodeNameShort().c_str(),sizeof(m_status->name));
  m_status->name[sizeof(m_status->name)-1]=0;
}

/// Update the session monitoring block. Does reallocate the memory block if necessary
void Session::setTorrentInfo()   const   {
  if ( int(m_torrents.size()) > m_status->torrents.size() ) {
    m_status = m_status->reallocate(m_torrents.size());
  }
  SessionStatus::Torrents::iterator it = m_status->torrents.reset();
  for(Torrents::const_iterator i = m_torrents.begin(); i != m_torrents.end(); ++i)  {
    torrent_status t = (*i).second.status();
    it->setName((*i).first);
    it->setMessage(t.error);
    it->setFile((*i).second.name());
    it->state      = t.state;
    it->progress   = t.progress;

    it->num_seeds  = t.num_seeds;
    it->num_pieces_total = t.pieces.size();
    it->num_pieces_done  = t.num_pieces;
    it->total_done       = t.total_done;
    it->total_wanted     = t.total_wanted;
    
    it->num_peers        = t.num_peers;
    it->num_uploads      = t.num_uploads;
    it->total_upload     = t.total_upload;
    it->total_download   = t.total_download;
    it = m_status->torrents.add(it);
  }
}

/// Set the upload limit for this session
void Session::setUploadLimit(int mb_per_second) {
  m_session->set_upload_rate_limit(mb_per_second*MBYTE);
  m_session->set_local_upload_rate_limit(mb_per_second*MBYTE);
}

/// Set the download limit for this session
void Session::setDownloadLimit(int mb_per_second) {
  m_session->set_download_rate_limit(mb_per_second*MBYTE);
  m_session->set_local_download_rate_limit(mb_per_second*MBYTE);
}

/// Remove all torrents and optionally clear caches
void Session::removeAllTorrents(bool flush) {
  RTL::Lock lock(m_lock);
  for(Torrents::iterator i = m_torrents.begin(); i != m_torrents.end(); ++i)  {
    char out[41];    
    torrent_handle& handle = (*i).second;
    to_hex((char const*)&handle.info_hash()[0], big_number::size, out);
    if ( flush ) {
      handle.pause();
      handle.flush_cache();
      handle.get_storage_impl()->delete_files();
    }
    m_session->remove_torrent(handle);
    ::lib_rtl_output(LIB_RTL_DEBUG,"Removing torrent: %s  Hash:%s\n",(*i).first.c_str(),out);
  }
  m_torrents.clear();
}

/// Add a new torrent
void Session::addTorrent(const std::string& nam) {
  RTL::Lock lock(m_lock);
  Torrents::const_iterator i = m_torrents.find(nam);
  if ( m_tier> 1 && i == m_torrents.end() ) {
    RTL::Lock lock_reverse(m_lock,true);
    removeAllTorrents(true);
  }
  if ( i == m_torrents.end() ) {
    char out[41];
    string path;
    error_code ec;
    add_torrent_params params;
    if ( m_tier == 0 ) {
      char file_path[PATH_MAX];
      ::strncpy(file_path,nam.c_str(),sizeof(file_path));
      path = ::dirname(file_path);
      int ret = ::chdir(path.c_str());
      if ( ret == 0 ) {
	::lib_rtl_output(LIB_RTL_FATAL,"Seeder path is:%s\n",path.c_str());
      }
      else {
	::lib_rtl_output(LIB_RTL_FATAL,"Failed to switch to seeder path is:%s - %s\n",path.c_str(),RTL::errorString());
	return;
      }
    }
    else {
      IocSensor::instance().send(m_parent,CMD_COMMAND_PARENT,new string(nam));
      path = m_saveDir;
    }
    params.save_path = path;
    params.ti = new torrent_info(nam, ec);
    if (ec) {
      IocSensor::instance().send(m_parent,CMD_COMMAND_FAILED,new string(nam));
      ::lib_rtl_output(LIB_RTL_FATAL,"Failed to create torrent '%s':%s\n",
		       nam.c_str(),ec.message().c_str());
      return;
    }
    torrent_handle handle = m_session->add_torrent(params, ec);
    if (ec)  {
      IocSensor::instance().send(m_parent,CMD_COMMAND_FAILED,new string(nam));
      ::lib_rtl_output(LIB_RTL_FATAL,"Failed to add torrent '%s':%s\n",
		       nam.c_str(),ec.message().c_str());
      return;
    }

    int cnt=0;
    for(Trackers::const_iterator j=m_trackers.begin(); j!=m_trackers.end(); ++j) {
      announce_entry e(*j);
      e.tier = ++cnt;
      handle.add_tracker(e);
    }
    to_hex((char const*)&handle.info_hash()[0], big_number::size, out);
    ::lib_rtl_output(LIB_RTL_INFO,"Added    torrent: %s hash:%s Directory:%s Tier:%d\n",
		     nam.c_str(),out,path.c_str(),m_tier);
    m_torrents.insert(make_pair(nam, handle));
    updateMonitoring();
  }
  else {
    (*i).second.force_recheck();
    updateMonitoring();
  }
}

/// Remove an existing torrent
void Session::removeTorrent(const std::string& nam) {
  RTL::Lock lock(m_lock);
  Torrents::iterator i = m_torrents.find(nam);
  if ( i != m_torrents.end() ) {
    char out[41];
    torrent_handle& handle = (*i).second;
    to_hex((char const*)&handle.info_hash()[0], big_number::size, out);
    ::lib_rtl_output(LIB_RTL_DEBUG,"Removing   torrent: %s hash:%s\n",nam.c_str(),out);
    handle.pause();
    handle.flush_cache();
    handle.get_storage_impl()->delete_files();
    m_session->remove_torrent(handle);
    m_torrents.erase(i);
    updateMonitoring();
    return;
  }
}

/// Reannounce trackers in reverse order
void Session::reannounce(torrent_handle handle)   const   {
  if ( m_trackers.size()>1 )   {
    int cnt = 0;
    string s;
    vector<announce_entry> v;
    for(Trackers::const_reverse_iterator i=m_trackers.rbegin(); i!=m_trackers.rend(); ++i) {
      v.push_back(*i);
      v.back().tier = ++cnt;
      s += (*i) + " ";
    }
    handle.replace_trackers(v);
    handle.force_reannounce();
    ::lib_rtl_output(LIB_RTL_DEBUG,"Reannouncing %d trackers:%s\n",
		     int(m_trackers.size()),s.c_str());
  }
}

/// Event poll thread routine
int Session::eventPoll(void* arg) {
  Session* s = (Session*)arg;
  return s->handleAlerts();
}

static bool handle_alerts1(const auto_ptr<alert>& alert, const Session& session) {
  try {
    handle_alert<
      state_changed_alert,

      peer_error_alert,
      peer_blocked_alert,
      peer_connect_alert,
      peer_disconnected_alert,
	    
      read_piece_alert,
      piece_finished_alert,
      hash_failed_alert, //  hash for piece XY failed
	    
      block_downloading_alert,
      block_finished_alert,
      unwanted_block_alert,

      tracker_warning_alert,
      tracker_announce_alert,
      tracker_reply_alert
      >  handler(alert,session);
    return true;
  }
  catch(const unhandled_alert& h) {
    return false;
  }
}

static bool handle_alerts2(const auto_ptr<alert>& alert, const Session& session) {
  try {
    handle_alert<
      dht_reply_alert,
      portmap_error_alert,
       
      listen_failed_alert,
      listen_succeeded_alert,

      torrent_finished_alert,
      torrent_deleted_alert,
      torrent_delete_failed_alert,
      torrent_resumed_alert,
      torrent_paused_alert,
      torrent_checked_alert,

      file_error_alert,
      file_completed_alert,
      file_renamed_alert
      >  handler(alert,session);
    return true;
  }
  catch(const unhandled_alert& h) {
    return false;
  }
}

/// Event poll callback
int Session::handleAlerts() {
  auto_ptr<alert> alert;
  while (1) {
    while ( (alert=m_session->pop_alert()).get() ) {
      try {
	bool handled = handle_alerts1(alert,*this);
	if ( !handled ) handled = handle_alerts2(alert,*this);
	if ( !handled )  {
	  ::lib_rtl_output(LIB_RTL_WARNING,"Unhandled alert:%s %s : %s\n",
			   typeid(*alert).name(),alert->what(),alert->message().c_str());
	}
      }
      catch(const exception& e) {
	::lib_rtl_output(LIB_RTL_WARNING,"Unhandled exception:%s %s : %s\n",
			 e.what(),alert->what(),alert->message().c_str());
      }
    }
    ::lib_rtl_sleep(100);
  }
  return 1;
}

void Session::operator()(libtorrent::stats_alert const& /* a */) const {
  updateMonitoring();
}

void Session::operator()(block_downloading_alert const& a) const {
  if ( ERR_ENABLED(block_downloading_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : Blk:%d/Piece:%d -- %s\n",
		     a.what(),a.block_index,a.piece_index,a.message().c_str());
  }
}

void Session::operator()(block_finished_alert const& a) const {
  if ( ERR_ENABLED(block_finished_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : Blk:%d/Piece:%d -- %s\n",
		     a.what(),a.block_index,a.piece_index,a.message().c_str());
  }
}

void Session::operator()(unwanted_block_alert const& a) const {
  if ( ERR_ENABLED(unwanted_block_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : Blk:%d/Piece:%d -- %s\n",
		     a.what(),a.block_index,a.piece_index,a.message().c_str());
  }
}

void Session::operator()(dht_reply_alert const& a) const {
  if ( ERR_ENABLED(dht_reply_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(),a.message().c_str());
  }
}

void Session::operator()(listen_succeeded_alert const& a) const  {
  if ( ERR_ENABLED(listen_succeeded_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(),a.message().c_str());
  }
}

void Session::operator()(listen_failed_alert const& a) const  {
  if ( ERR_ENABLED(listen_failed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(),a.message().c_str());
  }
}

void Session::operator()(portmap_error_alert const& a) const  {
  if ( ERR_ENABLED(portmap_error_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(),a.msg.c_str());
  }
}

void Session::operator()(tracker_announce_alert const& a) const {
  if ( ERR_ENABLED(tracker_announce_alert) ) {
    static const char* state[]={"None","Completed","Started","Stopped"};
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s - %s\n",a.what(),state[a.event],a.message().c_str());
  }
}

void Session::operator()(tracker_reply_alert const& a) const {
  if ( ERR_ENABLED(tracker_reply_alert) ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"%-24s : Peers:%d - %s\n",a.what(),a.num_peers,a.message().c_str());
  }
}

void Session::operator()(tracker_warning_alert const& a) const    {
  if ( ERR_ENABLED(tracker_warning_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s: %s\n",a.what(),a.msg.c_str());
  }
}

void Session::operator()(torrent_finished_alert const& a) const    {
  if ( ERR_ENABLED(torrent_finished_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
    updateMonitoring();
  }
}

void Session::operator()(torrent_deleted_alert const& a) const    {
  if ( ERR_ENABLED(torrent_deleted_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
  updateMonitoring();
}

void Session::operator()(torrent_delete_failed_alert const& a) const    {
  if ( ERR_ENABLED(torrent_delete_failed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(torrent_checked_alert const& a) const {
  if ( ERR_ENABLED(torrent_checked_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
    updateMonitoring();
  }
}

void Session::operator()(torrent_paused_alert const& a) const {
  ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  updateMonitoring();
}

void Session::operator()(torrent_resumed_alert const& a) const {
  if ( ERR_ENABLED(torrent_resumed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
    updateMonitoring();
  }
}

void Session::operator()(read_piece_alert const& a) const {
  ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
}

void Session::operator()(peer_error_alert const& a) const {
  if ( ERR_ENABLED(peer_error_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(peer_blocked_alert const& a) const {
  if ( ERR_ENABLED(peer_error_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(peer_connect_alert const& a) const {
  if ( ERR_ENABLED(peer_connect_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(peer_disconnected_alert const& a) const {
  if ( ERR_ENABLED(peer_disconnected_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

/// Callback once piece download was done.
void Session::operator()(piece_finished_alert const& a) const {
  if ( ERR_ENABLED(piece_finished_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

/// Callback once piece download was done.
void Session::operator()(hash_failed_alert const& a) const {
  if ( ERR_ENABLED(hash_failed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
  ++m_NUM_hash_failed_alert;
  if ( m_NUM_hash_failed_alert > 100 ) exit(10);
}

void Session::operator()(file_error_alert const& a) const {
  ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
}

void Session::operator()(file_completed_alert const& a) const {
  if ( ERR_ENABLED(file_completed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(file_renamed_alert const& a) const {
  if ( ERR_ENABLED(file_renamed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(file_rename_failed_alert const& a) const {
  if ( ERR_ENABLED(file_rename_failed_alert) ) {
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s\n",a.what(), a.message().c_str());
  }
}

void Session::operator()(state_changed_alert const& a) const {
  static const char* states[] = {
    "queued_for_checking",
    "checking_files",
    "downloading_metadata",
    "downloading",
    "finished",
    "seeding",
    "allocating",
    "checking_resume_data"};
  torrent_handle handle = a.handle;
  if ( a.state == torrent_status::downloading ) {
    if ( handle.is_valid() ) {
      //handle.force_reannounce();
      //reannounce(handle);
    }
  }
  else if ( a.state == torrent_status::finished ) {
    if ( handle.is_valid() ) {
      //handle.force_reannounce();
      //reannounce(handle);
    }
    ::lib_rtl_output(LIB_RTL_INFO,"%-24s : %s -> %s -- %s\n",
		     a.what(),states[a.prev_state],states[a.state],a.message().c_str());
    RTL::Lock lock(m_lock);
    for(Torrents::const_iterator i=m_torrents.begin(); i!=m_torrents.end();++i) {
      if ( handle == (*i).second ) {
	IocSensor::instance().send(m_parent,CMD_COMMAND_DONE,new string((*i).first));
	break;
      }
    }
  }
  updateMonitoring();
}

/// Interactor overload: Handle external interrupts
void Session::handle(const Event& event) {
  auto_ptr<string> str;
  switch(event.eventtype) {
  case IocEvent:
    switch(event.type) {
    case CMD_REMOVE_TORRENT:
      str = auto_ptr<string>(event.iocPtr<string>());
      removeTorrent(*str.get());
      return;
    case CMD_ADD_TORRENT:
      str = auto_ptr<string>(event.iocPtr<string>());
      addTorrent(*str.get());
      return;
    case CMD_MONITOR_TORRENT: {
      RTL::Lock lock(m_lock);
      setSessionInfo();
      setTorrentInfo();
      IocSensor::instance().send(m_parent,CMD_MONITOR_TORRENT,m_status->clone());
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
}



extern "C" int torrent_runserver(int argc, char** argv) {
  Main m(argc, argv);
  print_startup("START: Bittorrent load server");
  return m.run();
}

static void help_dimget() {
  ::printf("usage: dimget.exe -opt=<value> [-opt=<value>]                                     \n"
	   "   -dns=<name>            (optional)   Set DIM DNS node.                          \n"
	   "                                       Default: environ[DIM_DNS_NODE]             \n"
	   "   -torrent=<name>        (mandatory)  Get torrent by name.                       \n"
	   "   -timeout=<number>      (optional)   Set timeout to retrieve torrent.           \n"
	   "                                       Default: 20 seconds                        \n"
	   "                                       if <  100 : seconds                        \n"
	   "                                       if >= 100 : milliseconds                   \n"
	   "   -print=<number>        (optional)   Set printout level 1=VERBOSE,6=FATAL       \n"
	   "                                       Default: WARNING (3)                       \n"
	   "\n");
  ::exit(0);
}
 
static void torrent_reply(void* tag, void* buffer, int* size) {
  string* torrent = *(string**)tag;
  if ( torrent && buffer && size && *size>0 ) {
    string reply;
    char* data = (char*)buffer;
    data[*size] = 0;
    reply = data;
    if ( reply.find(*torrent) != string::npos ) {
      if ( reply.find("done:") == 0 )
	*torrent = "OK";
      if ( reply.find("failed:") == 0 )
	*torrent = "FAILED";
    }
    ::lib_rtl_output(LIB_RTL_INFO,"Torrent status:%s <-> %s\n",data,torrent->c_str());
  }
}

extern "C" int torrent_dimget(int argc, char** argv)  {
  int timeout = 20000, retry=2;
  long print = LIB_RTL_WARNING;
  string torrent, dns, copy;

  RTL::CLI cli(argc,argv,help_dimget);
  cli.getopt("dns",1,dns);
  cli.getopt("print",1,print);
  cli.getopt("torrent",2,torrent);
  cli.getopt("timeout",2,timeout);
  cli.getopt("retry",  2,retry);

  string name   = "/" + strupper(RTL::nodeNameShort()) + "/TorrentLoader/command";
  string status = "/" + strupper(RTL::nodeNameShort()) + "/TorrentLoader/status";
  string cmd    = "add:"+torrent;

  ::lib_rtl_install_printer(printout,(void*)print);
  ::lib_rtl_output(LIB_RTL_INFO,"Service:%s cmd:%s\n",name.c_str(),cmd.c_str());

  if ( !dns.empty() ) {
    ::dim_set_dns_node((char*)dns.c_str());
  }
  copy = torrent;
  if ( timeout < 100 ) timeout *= 1000;
  while( retry>0 ) {
    int res = ::dic_cmnd_service((char*)name.c_str(),(void*)cmd.c_str(),cmd.length()+1);
    if ( res != 1 ) {
      ::lib_rtl_output(LIB_RTL_FATAL,"Failed to notify parent '%s' to load torrent %s\n",
		       name.c_str(),cmd.c_str());
    }
    ::lib_rtl_sleep(500);
    unsigned int id = ::dic_info_service ((char*)status.c_str(),MONITORED,0,0,0,torrent_reply,(long)&torrent,0,0);
    for(int delta = timeout+1; torrent != "OK" && delta>0; delta -= 1000) {
      if ( torrent == "OK"     ) break;
      if ( torrent == "FAILED" ) break;
      ::lib_rtl_sleep(1000);
    }
    ::dic_release_service(id);
    if ( torrent == "OK"     )  {
      return 0;
    }
    --retry;
  }
  if ( torrent == "FAILED" )   {
    ::lib_rtl_output(LIB_RTL_FATAL,"Download FAILED:%s\n",copy.c_str());
    return EIO;
  }
  ::lib_rtl_output(LIB_RTL_FATAL,"Download TIMEOUT:%s\n",copy.c_str());
  return ETIMEDOUT;
}
