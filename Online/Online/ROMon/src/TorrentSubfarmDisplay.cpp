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

string torrent_file(const SubfarmTorrentStatus& sf)   {
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
    const Torrents& t = (*i).torrents;
    for(Torrents::const_iterator j=t.begin(); j!=t.end();j=t.next(j))
      return (*j).name;
  }
  return "---No torrent file known---";
}

/// Display the node information
void TorrentSubfarmDisplay::showNodes(const SubfarmTorrentStatus& sf)   {
  char text1[1024], text2[1024];
  MonitorDisplay* disp = m_nodes;
  string torrent = torrent_file(sf);
  static const char* states[] = {
    "queued_for_checking",
    "checking_files",
    "downloading_metadata",
    "downloading",
    "finished",
    "seeding",
    "allocating",
    "checking_resume_data"};

  disp->begin_update();
  disp->draw_line_bold("Torrent:%s",torrent.c_str());
  disp->draw_line_bold("");
  disp->draw_line_reverse("%-14s %23s %17s %13s %-31s %11s %17s",
			  "SESSION info:","--------Blocks--------", "----Load [kB]----", "--Rates [kB/s]--",
			  "TORRENT information:","---Pieces--","-----Load [kB]-----");
  disp->draw_line_reverse("%-9s%5s %7s %7s %7s %7s %9s %7s%9s %16s%6s%9s%12s %9s %9s",
			  "Node","Peers","written","read","hits","upload","download","upload","download",
			  "State","Peers","Progress","done total","upload","download");
  size_t num_session = sf.sessions.size();
  for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
    int cnt_torrents = 0;
    const char* file = 0;
    const char* msg  = 0;
    const SessionStatus& s = *i;
    ::snprintf(text1,sizeof(text1),"%-10s%4d%8d%8d%8d%8.0f%10.0f%8.0f%9.0f ",
	       s.name,s.num_peers,s.blocks_written,s.blocks_read,s.blocks_read_hit,
	       float(s.total_upload)/1024.f, float(s.total_download)/1024.0f,
	       float(s.upload_rate)/1024.f,float(s.download_rate)/1024.f);
    ::sprintf(text2,"No torrent information availible for this node.");
    for(Torrents::const_iterator j=s.torrents.begin(); j!=s.torrents.end();j=s.torrents.next(j))   {
      const TorrentStatus& t = *j;
      ++cnt_torrents;
      ::snprintf(text2,sizeof(text2),"%16s%6d%9.2f%6d%6d%10ld%10ld",
		 states[t.state], t.num_peers, 100.f*t.progress,
		 t.num_pieces_done, t.num_pieces_total,
		 t.total_upload/1024, t.total_download/1024);
      if ( t.name != torrent ) file = t.name;
      if ( t.msgLen>0 ) disp->draw_line_normal("\t%s", t.message());
      if ( num_session == 1 ) {
	disp->draw_line_reverse("-->%s",t.name);
	disp->draw_line_normal("%s%s",text1,text2);
	if ( msg  ) disp->draw_line_reverse("Message: %s",msg);
      }
      else     {
	disp->draw_line_normal("%s%s",text1,text2);
	if ( file ) disp->draw_line_reverse("-->%s",file);
	if ( msg  ) disp->draw_line_reverse("Message: %s",msg);
	break;
      }
    }
    if ( cnt_torrents == 0 ) {
      disp->draw_line_normal("%s%s",text1,"---- No explicit torrents found. ----");
    }
  }
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
  int cnt = -2;
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
