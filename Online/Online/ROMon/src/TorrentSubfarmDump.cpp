// $Id: SubfarmDump.cpp,v 1.8 2010-10-14 08:15:47 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/SubfarmDump.cpp,v 1.8 2010-10-14 08:15:47 frankb Exp $

// C++ include files
#include <iostream>
#include <stdexcept>

// Framework includes
extern "C" {
#include "dic.h"
}
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#include "ROMon/TorrentMon.h"
#include "ROMon/TorrentSubfarmDump.h"

using namespace std;
using namespace BitTorrent;
static std::string PUBLISHING_NODE = "ECS03";

namespace ROMon {
  std::string strupper(const std::string& n);
  std::string strlower(const std::string& n);
}

static void info_reply(void* tag, void* buffer, int* size) {
  TorrentSubfarmDump* torrent = *(TorrentSubfarmDump**)tag;
  if ( torrent && buffer && size && *size>0 ) {
    torrent->update(*(SubfarmTorrentStatus*)buffer);
  }
}

/// Standard constructor
TorrentSubfarmDump::TorrentSubfarmDump(int argc, char** argv) 
  : m_id(0), m_delay(2000)
{
  string dns, svc, nam, from=RTL::nodeNameShort();
  RTL::CLI cli(argc, argv, TorrentSubfarmDump::help);
  cli.getopt("dns",  3, dns);
  cli.getopt("subfarm", 3, from);
  cli.getopt("delay",4, m_delay);

  if ( !dns.empty() ) ::dic_set_dns_node((char*)dns.c_str());
  from = "/"+ROMon::strupper(from)+"/TorrentInfo";
  m_id = ::dic_info_service((char*)from.c_str(),MONITORED,0,0,0,info_reply,(long)this,0,0);
}

/// Default destructor
TorrentSubfarmDump::~TorrentSubfarmDump() {
  if ( m_id != 0 ) ::dic_release_service(m_id);
  m_id = 0;
}

/// Help printout in case of -h /? or wrong arguments
void TorrentSubfarmDump::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_syspublish -opt [-opt]\n"
                   "             -dns=<string>          DNS which offers the data service(s) \n"
                   "             -subfarm=<string>      Node which offers the data service(s)\n"
                   "             -delay=<number>        Update time in milliseconds          \n"
                   );
}

/// Update handler
void TorrentSubfarmDump::update(const SubfarmTorrentStatus& sf)  {
  typedef SubfarmTorrentStatus::Sessions Sessions;
  typedef SessionStatus::Torrents Torrents;
  static const char* states[] = {
    "queued_for_checking",
    "checking_files",
    "downloading_metadata",
    "downloading",
    "finished",
    "seeding",
    "allocating",
    "checking_resume_data"};

  fprintf(stdout,"Subfarm:%s\n",sf.name);
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
    const SessionStatus& s = *i;
    ::fprintf(stdout,"%-16s Peers:%4d Blocks:%7d/%7d/%7d Rate:^%5.0f v%5.0f kB/sec Load:^%d v%d kB\n",
              s.name,s.num_peers,s.blocks_written,s.blocks_read,s.blocks_read_hit,
              float(s.upload_rate)/1024.f,float(s.download_rate)/1024.f,
              int(s.total_upload/1024), int(s.total_download/1024));
    for(Torrents::const_iterator j=s.torrents.begin(); j!=s.torrents.end();j=s.torrents.next(j))   {
      const TorrentStatus& t = *j;
      ::fprintf(stdout, "\t-->%s\n", t.name);
      if ( t.msgLen>0 ) ::fprintf(stdout, "\t%s\n", t.message());
      ::fprintf(stdout, "\t   State:%-32s [%6.2f %%] Seeds:%-4d Peers:%-4d Pieces: %4d/%-4d kB:^%7d v%-7d Load:^%d v%d kB/sec\n",
                states[t.state], 100.f*t.progress, t.num_seeds, t.num_peers, t.num_pieces_done, t.num_pieces_total,
                int(t.total_done/1024),int(t.total_wanted/1024),int(t.total_upload/1024),int(t.total_download/1024));
    }
  }
  ::fprintf(stdout,"\n\n");
}

extern "C" int bittorrent_dump(int argc, char** argv) {
  TorrentSubfarmDump mon(argc,argv);
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}

