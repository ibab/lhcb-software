// $Id: TorrentSubfarmDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/TorrentSubfarmDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#include "ROMon/TorrentSubfarmDisplay.h"
extern "C" {
#include "dic.h"
}

using namespace std;
using namespace ROMon;
using namespace BitTorrent;

static void help() {
  cout <<"  romon_storage -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       << endl;
}

namespace ROMon {
  std::string strupper(const std::string& n);
  std::string strlower(const std::string& n);
}

/// Static abstract object creator.
ClusterDisplay* BitTorrent::createTorrentDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new TorrentSubfarmDisplay(width,height,posx,posy,argc,argv);
}

/// Standard constructor
TorrentSubfarmDisplay::TorrentSubfarmDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width,height)
{
  m_position = Position(posx,posy);
  init(argc, argv);
}

/// Standard constructor
TorrentSubfarmDisplay::TorrentSubfarmDisplay(int argc, char** argv)   {
  init(argc, argv);
}

void TorrentSubfarmDisplay::init(int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  string node;
  int hdr_height;
  cli.getopt("headerheight",  2, hdr_height    =   5);
  cli.getopt("delay",         3, m_delay       = 1000);
  cli.getopt("servicename",   2, m_svcName     = "TorrentInfo");
  cli.getopt("node",          2, m_name        = "HLTF02");

  if ( cli.getopt("debug",3) ) {
    ::lib_rtl_sleep(10000);
  }
  m_readAlways = true;
  if ( m_svcName == "TorrentInfo" )  {
    m_svcName = "/"+ROMon::strupper(m_name)+"/TorrentInfo";
  }
  m_name = ROMon::strlower(m_name);

  if ( !node.empty() ) {
    string svc = "HLT/ExcludedNodes/"+strupper(node);  
    m_svcID2   = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,excludedHandler,(long)this,0,0);
  }
  setup_window();
  int width    = m_area.width;
  int height   = m_area.height;
  int posx     = m_position.x-2;
  int posy     = m_position.y-2;
  m_nodes      = createSubDisplay(Position(posx,posy+hdr_height),Area(width,height-hdr_height),
                                  "Torrent Subfarm Information for "+m_svcName+"   ");
  m_clusterData.pointer = (char*)0;
  showHeader();
  end_update();
}

/// Standard destructor
TorrentSubfarmDisplay::~TorrentSubfarmDisplay()  {
  m_clusterData.pointer = 0;
  begin_update();
  delete m_nodes;
  end_update();
}

typedef SubfarmTorrentStatus::Sessions Sessions;
typedef SessionStatus::Torrents Torrents;

static string torrent_file(const SubfarmTorrentStatus& sf)   {
  map<string,int> tmap;
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
    const Torrents& t = (*i).torrents;
    for(Torrents::const_iterator j=t.begin(); j!=t.end();j=t.next(j))
      if ( tmap.find((*j).name) != tmap.end() ) ++tmap[(*j).name];
      else tmap[(*j).name] = 1;
  }
  int maxi = 0;
  map<string,int>::const_iterator k=tmap.begin(),mx=tmap.begin();
  for(; k != tmap.end(); ++k) {
    if ( (*k).second > maxi ) { maxi = (*k).second; mx=k; }
  }
  if ( mx != tmap.end() ) return (*mx).first;
  return "---No torrent file known---";
}

/// Display the node information
void TorrentSubfarmDisplay::showNodes(const SubfarmTorrentStatus& sf)   {
#if 0
  static const char* states[] = {
    "queued_for_checking",
    "checking_files",
    "downloading_metadata",
    "downloading",
    "finished",
    "seeding",
    "allocating",
    "checking_resume_data"};
#endif
  char text1[1024], text2[1024];
  MonitorDisplay* disp = m_nodes;
  string torrent = torrent_file(sf);
  map<string,int> tmap;

  disp->begin_update();
  /// Draw header
  disp->draw_line_reverse("%-14s %23s %17s %13s %-21s %18s %17s",
                          "SESSION info:","--------Blocks--------", "----Load [kB]----", "--Rates [kB/s]--",
                          "TORRENT information:","------Pieces------",
                          "--------------Load [kB]---------------");
  disp->draw_line_reverse("%-9s%5s %7s %7s %7s %7s %9s %7s%9s %6s%6s%6s%9s%12s %9s %9s %9s %9s",
                          "Node","Peers","written","read","hits","upload","download","upload","download",
                          "Seeds","Peers","#Torr","Progress","total done","upload","download","wanted","done");

  /// Loop over all sessions in all nodes:
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
    const SessionStatus& s = *i;
    string nam = s.name;
    if ( strncmp(s.name,"store",5) == 0 ) nam = s.name+5;
    ::snprintf(text1,sizeof(text1),"%-10s%4d%8d%8d%8d%8.0f%10.0f%8.0f%9.0f ",
               nam.c_str(),s.num_peers,s.blocks_written,s.blocks_read,s.blocks_read_hit,
               float(s.total_upload)/1024.f, float(s.total_download)/1024.0f,
               float(s.upload_rate)/1024.f,float(s.download_rate)/1024.f);
    ::sprintf(text2,"---- No explicit torrents found. ----");

    Torrents::const_iterator ti = s.torrents.end();
    for(Torrents::const_iterator j=s.torrents.begin(); j!=s.torrents.end();j=s.torrents.next(j))
      tmap[(*j).name] +=  1;

    float progress = 0.f;
    long  num_torrent=0, num_peers=0, num_seeds=0, num_pieces_done=0;
    long  num_pieces_total=0, total_upload=0, total_download=0, total_wanted=0, total_done=0;
    for(Torrents::const_iterator j=s.torrents.begin(); j!=s.torrents.end();j=s.torrents.next(j))   {
      const TorrentStatus& t = *j;
      num_seeds += t.num_seeds;
      num_peers += t.num_peers;
      num_pieces_done  += t.num_pieces_done;
      num_pieces_total += t.num_pieces_total;
      total_upload     += t.total_upload; 
      total_download   += t.total_download;
      total_wanted     += t.total_wanted;
      total_done       += t.total_done;
      progress         += t.progress;
      ++num_torrent;
    }
    if ( num_torrent>0 )  {
      progress /= num_torrent;  
      ::snprintf(text2,sizeof(text2),"%6ld%6ld%6ld%9.2f%6ld%6ld%10ld%10ld%10ld%10ld",
                 num_seeds, num_peers, num_torrent, 100.f*progress,
                 num_pieces_total, num_pieces_done, 
                 long(total_upload/1024), long(total_download/1024),
                 long(total_wanted/1024), long(total_done/1024));
    }
    disp->draw_line_normal("%s%s",text1,text2);
  }
  disp->draw_line_bold("");
  for(map<string,int>::const_iterator j=tmap.begin(); j != tmap.end(); ++j)  {
    disp->draw_line_bold(" %d nodes have %s",(*j).second, (*j).first.c_str());
  }
  disp->draw_line_bold("");
  disp->draw_line_bold("");
  disp->end_update();
}

/// Update header information
void TorrentSubfarmDisplay::showHeader()   {
  char text[256];
  text[0] = 0;
  if ( !m_excluded.empty() ) ::sprintf(text,"%d node(s) excluded",int(m_excluded.size()));    
  draw_line_normal ("");
  draw_line_reverse("         Task control monitoring on %s   [%s]  %s", 
                    m_name.c_str(), ::lib_rtl_timestr(), text);
  draw_line_reverse("         Information service:%s data size:%zd", m_svcName.c_str(),m_data.actual);
  draw_line_normal ("");
}

/// DIM command service callback
void TorrentSubfarmDisplay::excludedHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    TorrentSubfarmDisplay* disp = *(TorrentSubfarmDisplay**)tag;
    char *p = (char*)address, *end = p+*size;
    set<string> nodes;
    while(p<end) {
      nodes.insert(strlower(p));
      p += (::strlen(p)+1);
    }
    if ( nodes.size() > 0 )  {
      RTL::Lock lock(disp->m_lock);
      disp->m_excluded = nodes;
    }
  }
}

/// Update all displays
void TorrentSubfarmDisplay::update()   {
  try {
    RTL::Lock lock(m_lock);
    begin_update();
    m_nodes->begin_update();
    showHeader();
    if ( m_data.actual > 0 ) {
      const SubfarmTorrentStatus& status = *(m_data.data<SubfarmTorrentStatus>());
      showNodes(status);
    }
    else {
      m_nodes->draw_line_normal ("");
      m_nodes->draw_line_bold("   ..... No torrent data present for: %s",m_name.c_str());
      m_nodes->draw_line_normal ("");
    }
  }
  catch(const exception& e) {
    m_nodes->draw_line_normal ("");
    m_nodes->draw_line_bold("   ..... Exception during data parsing: %s",e.what());
    m_nodes->draw_line_normal ("");
  }
  catch(...) {
    m_nodes->draw_line_normal ("");
    m_nodes->draw_line_bold("   ..... Exception during data parsing .....");
    m_nodes->draw_line_normal ("");
  }
  m_nodes->end_update();
  end_update();
}

/// Retrieve cluster name from cluster display
string TorrentSubfarmDisplay::clusterName() const {
  return m_name;
}

/// Number of nodes in the dataset
size_t TorrentSubfarmDisplay::numNodes() {
  const SubfarmTorrentStatus& sf = *(m_data.data<SubfarmTorrentStatus>());
  return sf.sessions.size();
}

/// Retrieve node name from cluster display by offset
string TorrentSubfarmDisplay::nodeName(size_t offset) {
  typedef SubfarmTorrentStatus::Sessions Sessions;
  const SubfarmTorrentStatus& sf = *(m_data.data<SubfarmTorrentStatus>());
  int cnt = 0;
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i), ++cnt) {
    if ( cnt == int(offset) ) return (*i).name;
  }
  return "";
}

extern "C" int bittorrent_subfarmdisplay(int argc,char** argv) {
  TorrentSubfarmDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
