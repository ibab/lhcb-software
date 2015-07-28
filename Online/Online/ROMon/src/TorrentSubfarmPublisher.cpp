// $Id: SubfarmPublisher.cpp,v 1.8 2010-10-14 08:15:47 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/SubfarmPublisher.cpp,v 1.8 2010-10-14 08:15:47 frankb Exp $

// C++ include files
#include <iostream>
#include <stdexcept>

// Framework includes
extern "C" {
#include "dic.h"
#include "dis.h"
}
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "ROMon/TorrentMon.h"
#include "ROMon/TorrentSubfarmPublisher.h"

using namespace std;
using namespace ROMon;
using namespace BitTorrent;
typedef FMCMonListener::Descriptor DSC;
typedef RODimListener::Clients     Clients;
static std::string PUBLISHING_NODE = "ECS03";

namespace ROMon {
  void ro_get_node_name(char* name, size_t len);
  void ro_gettime(int* time, unsigned int* millitm);
  void print_startup(const char* msg);
}

namespace {
  template <class T> struct _Svc : public TorrentSubfarmPublisher::_BaseSvc {
  protected:
    int id, flag;
    size_t buffLen;
    char* buff;
    std::string name;
    FMCMonListener& info;
  private:
    /// Private copy constructor
    _Svc(const _Svc&) {}
    /// Private assignment operator
    _Svc& operator=(const _Svc&) { return *this; }
  public:
    _Svc(FMCMonListener& i, size_t l, const std::string& nam, int flg=0)
      : id(0), flag(flg), buffLen(l), buff(0), name(nam), info(i) {
      buffLen *= (1024*8); // *8 for 64 node farms
      buff = new char[buffLen];
      ((T*)buff)->reset();
    }
    virtual ~_Svc() {
      ::dis_remove_service(id);
      ::free(buff);
      buff = 0;
    }
    virtual int get_data() {  return 0; }
    virtual void start() {
      ((T*)buff)->reset();
      id = ::dis_add_service((char*)name.c_str(),(char*)"C",0,0,feed,(long)this);
    }
    virtual void print() const {
      cout  << "========================" << ::lib_rtl_timestr() 
            << "========================" << endl
        ;//<< *(T*)buff << endl;
    }
    virtual void update() {
      ::dis_update_service(id);
    }
    virtual void load() {
      for(int res=get_data(); res==2; ) {
        if ( buff ) ::free(buff);
        buffLen += buffLen/2;
        buff = (char*)::malloc(buffLen);
        res = get_data();
      }
    }
    /// Feed data to DIS when updating data
    static void feed(void* tag, void** buf, int* size, int* /* first */) {
      _Svc<T>* h = *(_Svc<T>**)tag;
      *buf = h->buff;
      *size  = ((T*)h->buff)->length();
      //printf("Publish %d bytes of data\n",*size);
    }
  };

  /// Update buffer with MBM information
  template <> int _Svc<SubfarmTorrentStatus>::get_data() {
    SubfarmTorrentStatus* n = (SubfarmTorrentStatus*)buff;
    SubfarmTorrentStatus::Sessions& sessions = n->reset()->sessions;
    SubfarmTorrentStatus::Sessions::iterator it = sessions.reset();
    const Clients& cl = info.clients();
    ro_get_node_name(n->name,sizeof(n->name));
    n->type = SubfarmTorrentStatus::TYPE;
    for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
      DSC* d = (*ic).second->data<DSC>();
      if ( d->data ) {
        if ( ((char*)it) > buff+buffLen ) return 2;
        ::memcpy(it,d->data,d->actual);
        //printf("Publish %d / %d bytes from %s\n", it->length(), d->actual, it->name);
        it = sessions.add(it);
      }
    }
    return 1;
  }
}

/// Standard constructor
TorrentSubfarmPublisher::TorrentSubfarmPublisher(int argc, char** argv) 
{
  string match = "*", svc, nam, from=RTL::nodeNameShort(), to=PUBLISHING_NODE;
  RTL::CLI cli(argc, argv, TorrentSubfarmPublisher::help);
  cli.getopt("to",   3, to);
  cli.getopt("from", 3, from);
  cli.getopt("match",3, match);
  cli.getopt("publish",   2, svc);
  m_print   = cli.getopt("print",2) != 0;
  m_verbose = cli.getopt("verbose",1) != 0;
  cli.getopt("delay",  4, m_delay=2000);

  ::dic_set_dns_node((char*)from.c_str());
  ::dis_set_dns_node((char*)to.c_str());
  m_service[0] = new _Svc<SubfarmTorrentStatus>(m_torrents,64,svc);
  if ( svc.empty() )  {
    ::lib_rtl_output(LIB_RTL_FATAL,"Unknown data type -- cannot be published.\n");
    throw std::runtime_error("Unknown data type and unknwon service name -- cannot be published.");
  }
  m_torrents.setMatch(match);
  m_torrents.setItem("monitor");
  m_torrents.setVerbose(m_verbose);
  m_torrents.setUpdateHandler(this);
  m_torrents.start();
  for(size_t i=0; i<sizeof(m_service)/sizeof(m_service[0]); ++i)
    m_service[i]->start();
  ::dis_start_serving((char*)svc.c_str());
}

/// Default destructor
TorrentSubfarmPublisher::~TorrentSubfarmPublisher() {
  for(size_t i=0; i<sizeof(m_service)/sizeof(m_service[0]); ++i)
    delete m_service[i];
}

/// Help printout in case of -h /? or wrong arguments
void TorrentSubfarmPublisher::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_syspublish -opt [-opt]\n"
                   "             -from=<string>         Node which offers the data service(s)\n"
                   "             -to=<string>           Node to publish the data to.\n"
                   "             -verbose               Switch to verbose mode.\n"
                   "             -match=<string>        String to match service names.\n"
                   "             -publish=<string>      Service name to publish results.\n"
                   );
}

/// Start monitoring activity
int TorrentSubfarmPublisher::monitor() {
  bool exec = true;
  while(exec)    {
    dim_lock();
    m_service[0]->load();
    m_service[0]->update();
    dim_unlock();
    ::lib_rtl_sleep(m_delay);
  }
  return 1;
}

void TorrentSubfarmPublisher::update(void* /* param */)   {
}

extern "C" int bittorrent_publish(int argc, char** argv) {
  TorrentSubfarmPublisher mon(argc,argv);
  print_startup("Torrent information COLLECTOR");
  return mon.monitor();
}

