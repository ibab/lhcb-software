
// $Id: TorrentNodeDisplay.cpp,v 1.7 2010-10-15 08:06:25 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/TorrentNodeDisplay.cpp,v 1.7 2010-10-15 08:06:25 frankb Exp $

// Framework include files
#include "ROMon/TorrentNodeDisplay.h"
#include "CPP/Event.h"
#include "SCR/scr.h"
#include "RTL/strdef.h"
extern "C" {
#include "dic.h"
}
// C++ include files
#include <cstdlib>

using namespace BitTorrent;
using namespace ROMon;
using namespace SCR;
using namespace std;


TorrentNodeDisplay::TorrentNodeDisplay(InternalDisplay* parent, const string& title, const string& node, int height, int width)
  : InternalDisplay(parent, title), m_node(node)
{
  string svc = "/"+RTL::str_upper(m_title);
  svc += "/TorrentInfo";
  m_node = RTL::str_lower(node);
  m_title = "Torrent monitor on "+m_node+" Service:"+svc;
  ::scrc_create_display(&m_display,height,width,NORMAL,ON,m_title.c_str());
  ::scrc_put_chars(m_display,".....waiting for data from DIM service.....",BOLD,2,10,1);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

TorrentNodeDisplay::~TorrentNodeDisplay() {
}

/// DIM command service callback
void TorrentNodeDisplay::update(const void* address) {
  const SubfarmTorrentStatus& status = *(SubfarmTorrentStatus*)address;
  updateContent(status);
}

/// Update display content
void TorrentNodeDisplay::updateContent(const SubfarmTorrentStatus& sf) {
  typedef SubfarmTorrentStatus::Sessions Sessions;
  typedef SessionStatus::Torrents Torrents;
  int cnt = 0;
  int line = 1;
  static const char* states[] = {
    "queued_for_checking",
    "checking_files",
    "downloading_metadata",
    "downloading",
    "finished",
    "seeding",
    "allocating",
    "checking_resume_data"};
  char txt[256], text[256];
  time_t t1 = ::time(0);
  size_t num_session = sf.sessions.size();
  ::scrc_set_border(m_display,m_title.c_str(),INVERSE|BLUE);
  ::scrc_put_chars(m_display,"",BOLD|RED|INVERSE,++line,1,1);
  ::strftime(text,sizeof(text),"%H:%M:%S",::localtime(&t1));
  ::snprintf(txt,sizeof(txt),"      Torrents on:%s %s",m_node.c_str(),text);
  ::scrc_put_chars(m_display,txt,BOLD|RED|INVERSE,++line,1,1);
  ::scrc_put_chars(m_display,"      Type 't' or Mouse-Left-Double-Click to close this window",BOLD|RED|INVERSE,++line,1,1);
  ::scrc_put_chars(m_display,"",BOLD|RED|INVERSE,++line,1,1);
  ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
  if ( num_session > 0 )  {
    ::snprintf(txt,sizeof(txt),"%-14s %23s %17s %13s %-31s %11s %s",
               "SESSION info:","--------Blocks--------", "----Load [kB]----", "--Rates [kB/s]--",
               "TORRENT information:","---Pieces--","-------------Load [kB]-------------");
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
    ::snprintf(txt,sizeof(txt),"%-9s%5s %7s %7s %7s %7s %9s %7s%9s %16s%6s%9s%12s %8s %8s %8s %8s",
               "Node","Peers","written","read","hits","upload","download","upload","download",
               "State","Peers","Progress","total done","upload","download","wanted","done");
    ::scrc_put_chars(m_display,txt,BOLD,++line,3,1);
    for(Sessions::const_iterator i=sf.sessions.begin(); i!=sf.sessions.end(); i=sf.sessions.next(i)) {
      const SessionStatus& s = *i;
      string nam = s.name;
      if ( m_node != nam ) continue;
      if ( num_session < 1 )  {
        ::scrc_put_chars(m_display,"No torrent information availible for this node.",NORMAL,++line,3,1);
      }

      for(Torrents::const_iterator j=s.torrents.begin(); j!=s.torrents.end();j=s.torrents.next(j))   {
        const TorrentStatus& t = *j;
        int col = t.state == 5 ? GREEN : RED;
        ::snprintf(txt,sizeof(txt),"-->%s",t.name);
        ::scrc_put_chars(m_display,txt,INVERSE|BOLD|MAGENTA,++line,3,1);
        ::snprintf(txt,sizeof(txt),"%-10s%4d%8d%8d%8d%8.0f%10.0f%8.0f%9.0f ",
                   nam.c_str(),s.num_peers,s.blocks_written,s.blocks_read,s.blocks_read_hit,
                   float(s.total_upload)/1024.f, float(s.total_download)/1024.0f,
                   float(s.upload_rate)/1024.f,float(s.download_rate)/1024.f);
        ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
        ::snprintf(txt,sizeof(txt),"%16s%6d%9.2f%6ld%6ld%9ld%9ld%9ld%9ld",
                   states[t.state], t.num_peers, 100.f*t.progress,
                   long(t.num_pieces_total), long(t.num_pieces_done), 
                   long(t.total_upload/1024), long(t.total_download/1024),
                   long(t.total_wanted/1024), long(t.total_done/1024));
        ::scrc_put_chars(m_display,txt,INVERSE|col,line,77,1);
        if ( t.msgLen>0 ) {
          ::snprintf(txt,sizeof(txt),"Message: \t%s", t.message());
          ::scrc_put_chars(m_display,txt,NORMAL,++line,3,1);
        }
        ::scrc_put_chars(m_display,"",NORMAL,++line,3,1);
        ++cnt;
      }
    }
  }
  if ( 0 == cnt )  {
    t1 = ::time(0);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::strftime(txt,sizeof(txt),"   No torrents present.         %H:%M:%S",::localtime(&t1));
    ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,5,1);
    ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
    ::scrc_set_border(m_display,m_title.c_str(),INVERSE|RED|BOLD);
  }
}
