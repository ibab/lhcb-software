// $Id: StorageDisplay.cpp,v 1.1 2008-02-08 17:29:16 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/StorageDisplay.cpp,v 1.1 2008-02-08 17:29:16 frankm Exp $

// C++ include files
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/StorageDisplay.h"
#include "dic.hxx"

using namespace ROMon;
static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};


struct DataWriterInfo {
  int space;
  int free_space;
  int open_files;
  int closed_files;
  int transferred_files;
  int deleted_files;
  char data_dir[256];
};

namespace {
  struct Stream {
    std::string node, source;
    int sent, written, received;
    Stream() : sent(0), written(0), received(0) {}
    Stream(const Stream& s) : node(s.node), source(s.source), sent(s.sent), written(s.written), received(s.received) {}
    Stream& operator=(const Stream& s) { node = s.node; source = s.source; sent=s.sent; written=s.written; received=s.received; return *this;}
  };
}

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
typedef std::map<std::string,Stream> Streams;

static char* nullchr(char* src,char match) {
  char* p = strchr(src,match);
  if(p) *p = 0;
  return p;
}
static char* nullstr(char* src,const char* match) {
  char* p = strstr(src,match);
  if(p) *p = 0;
  return p;
}

static void help() {
  std::cout <<"  romon_storage -option [-option]" << std::endl
	    <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << std::endl
	    <<"       -s[treamheight]=<number>     Height of the Streams       display.                      " << std::endl
	    <<"       -b[ufferheight]=<number>     Height of the Buffers       display.                      " << std::endl
	    <<"       -l[oggerheight]=<number>     Height of the Logger        display.                      " << std::endl
	    <<"       -f[ilesheight]=<number>      Height of the Files         display.                      " << std::endl
	    <<"       -w[idthhltrec]=<number>      Height of the HLT receivers display.                      " << std::endl
	    <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << std::endl
	    <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << std::endl
	    <<"       -p[partition]=<name>         Name of the partition to be displayed.                    " << std::endl
	    << std::endl;

}

/// Standard constructor
StorageDisplay::StorageDisplay(int argc, char** argv) : ROMonDisplay(), m_partName("LHCb")
{
  int hdr_height = 5;
  int hlt_width = 27;
  int buff_height = 8;
  int strm_height = 12;
  int logg_height = 4;
  RTL::CLI cli(argc,argv,help);
  cli.getopt("headerheight",  1, hdr_height);
  cli.getopt("streamheight",  1, strm_height);
  cli.getopt("loggerheight",  1, logg_height);
  cli.getopt("widthhltrec",   1, hlt_width);
  cli.getopt("bufferheight",  1, buff_height);
  cli.getopt("delay",         1, m_delay);
  cli.getopt("servicename",   1, m_svcName);
  cli.getopt("partitionname", 1, m_partName);

  setup_window();
  m_hltRec  = createSubDisplay(Position(0,hdr_height), Area(hlt_width,m_area.height-hdr_height),"Farm receivers");
  int right = m_hltRec->right();
  int width = m_area.width-right;
  m_buffers = createSubDisplay(Position(right,hdr_height), Area(width,buff_height),
			       "Buffer Monitor");
  m_streams = createSubDisplay(Position(right,m_buffers->bottom()-1),Area(width,strm_height),
			       "Stream Information");
  m_logging = createSubDisplay(Position(right,m_streams->bottom()-1),Area(width,logg_height),
			       "Logger Summary");
  m_files   = createSubDisplay(Position(right,m_logging->bottom()-1),Area(width,m_area.height-m_logging->bottom()+1),
			       "File Information");
  end_update();
}

/// Standard destructor
StorageDisplay::~StorageDisplay()   {
}

/// Show the data logging status
void StorageDisplay::showLogging() {
  MonitorDisplay* disp = m_logging;
  DataWriterInfo  wi;
  wi.space = wi.free_space = wi.open_files = wi.closed_files = wi.transferred_files = wi.deleted_files = 0;
  strcpy(wi.data_dir,"/daqarea/2008/RAW/LHCb/Physics/0123456789");

  disp->draw_line_bold(  "  File Information for Run %-10                                         ",0);
  disp->draw_line_normal("  Space:%9d MB [%4d TB] Free Space: %9d MB [%4d TB]", 
			 wi.space,wi.space/1024/1024,wi.free_space,wi.free_space/1024/1024);
  disp->draw_line_normal("  Open:%5d  Closed:%5d Transferring:%5d Deleted:%5d Total:%5d     ",
			 wi.open_files,wi.closed_files,wi.transferred_files,wi.deleted_files,
			 wi.open_files+wi.closed_files+wi.transferred_files+wi.deleted_files);
  disp->draw_line_normal("  Directory:%s", wi.data_dir);
}

/// Show the file information
void StorageDisplay::showFiles() {
  MonitorDisplay* disp = m_files;
  disp->draw_line_bold("  File                    Destination Size[Bytes]  Events     State           ");
}

/// Display the Stream information
void StorageDisplay::showStreams(const Nodeset& ns) {
  Streams streams;
  MonitorDisplay* disp = m_streams;
  std::string part = m_partName + "_";
  disp->draw_line_bold("  Stream Information [# of Events]              Sent    Received     Written   ");

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	const MBMClient& c = *ic;
	if (strncmp(c.name,part.c_str(),part.length())==0) {
	  char nam[BM_USER_NAME_LEN], *typ, *str, *node, *ptr;
	  strcpy(nam,c.name);
	  typ = nullstr(nam,"_SND");
	  node = nam+part.length();
	  if ( typ ){
	    *(typ+3) = 0;
	    ptr = nullchr(str=typ+4,'_');
	    if ( !ptr ) streams[str].node = "Monitoring";
	    streams[str].sent += c.events;
	    streams[str].source  = node;
	  }
	  else if ( (typ=nullstr(nam,"_RCV")) ) {
	    *(typ+3) = 0;
	    nullchr(str=typ+4,'_');
	    streams[str].received+=c.events;
	    streams[str].node = node;
	  }
	  else if ( (typ=nullstr(nam,"_WRT")) ) {
	    *(typ+3) = 0;
	    nullchr(str=typ+4,'_');
	    streams[str].written+=c.events;
	    streams[str].node = node;
	  }
	}
      }
    }
  }
  Stream total;
  for(std::map<std::string,Stream>::const_iterator i=streams.begin();i!=streams.end();++i) {
    const Stream& s = (*i).second;
    if ( s.node != "Monitoring" ) {
      disp->draw_line_normal(" %-12s %12s->%-12s %11d %11d %11d",
			     (*i).first.c_str(),s.source.c_str(),s.node.c_str(),
			     s.sent,s.received,s.written);
      total.sent += s.sent;
      total.written += s.written;
      total.received += s.received;
    }
  }
  disp->draw_line_bold(" %-12s %12s->%-12s %11d %11d %11d","Total","Recv Layer","Stream Layer",
		       total.sent,total.received,total.written);
  disp->draw_line_normal("");
  for(std::map<std::string,Stream>::const_iterator i=streams.begin();i!=streams.end();++i) {
    const Stream& s = (*i).second;
    if ( s.node == "Monitoring" ) {
      disp->draw_line_normal(" %-12s %12s->%-12s %11d",(*i).first.c_str(),s.source.c_str(),s.node.c_str(),s.sent);
    }
  }
}

/// Show the information about the HLT receiver tasks
void StorageDisplay::showHLT(const Nodeset& ns) {
  MonitorDisplay* disp = m_hltRec;
  std::string part = m_partName + "_";
  const Nodes& nodes = ns.nodes;
  long total_evts = 0;
  disp->draw_line_bold("  Buffer Information:         Produced  Pending    Size[kB]   Free[kB]  Users ");
  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    const Tasks* tasks = (*n).tasks();
    const Buffers& buffs = *(*n).buffers();
    for (Tasks::const_iterator i=tasks->begin(); i!=tasks->end(); i=tasks->next(i))  {
      const FSMTask& t = *i;
      size_t len = strlen(t.name);
      if ( strcmp(t.name+len-4,"_HLT")==0 && strncmp(t.name,part.c_str(),part.length())==0) {
	for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
	  const Clients& clients = (*ib).clients;
	  for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
	    if ( (*ic).processID == t.processID ) {
	      total_evts += (*ic).events;
	      disp->draw_line_normal("%s%11d %s %c%c%c",t.name+len-8,
				     (*ic).events,sstat[size_t((*ic).state)],
				     t.state,t.targetState,t.metaState);
	      goto Next;
	    }
	  }
	}
	disp->draw_line_normal("%s",t.name+len-8);
      Next:
	continue;
      }
    }
  }
  disp->draw_line_normal("");
  disp->draw_line_bold("%-8s%11d","Total",total_evts);
}

/// Show the information about MBM buffers on the storage nodes
void StorageDisplay::showBuffers(const Nodeset& ns) {
  MonitorDisplay* disp = m_buffers;
  const Nodes& nodes = ns.nodes;
  disp->draw_line_bold("  Buffer Information:         Produced Pending Size[kB] Free[kB] Slots Users ");
  for (Nodes::const_iterator n=nodes.begin(); n!=nodes.end(); n=nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    // Fixme: This must go into the publishing....
    char* p = strchr(n->name,'.');
    if ( p ) *p = 0;
    for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const MBMBuffer& b = *ib;
      const CONTROL& c = b.ctrl;
      disp->draw_line_normal("%10s::%-12s %12d %7d %8d %8d %5d %5d ",
			     n->name,b.name,c.tot_produced,c.i_events,
			     (c.bm_size*c.bytes_p_Bit)/1024,(c.i_space*c.bytes_p_Bit)/1024,
			     c.p_emax-c.i_events,c.i_users);
    }
  }
}


/// Update all displays
void StorageDisplay::updateDisplay(const Nodeset& /* ns */)   {
  begin_update();
  m_hltRec->begin_update();
  m_buffers->begin_update();
  m_streams->begin_update();
  m_logging->begin_update();
  m_files->begin_update();
  /*
  updateHeader();
  showHLT(ns);
  showBuffers(ns);
  showStreams(ns);
  showLogging();
  showFiles();
  */
  m_files->end_update();
  m_logging->end_update();
  m_streams->end_update();
  m_buffers->end_update();
  m_hltRec->end_update();
}

extern "C" int romon_storage(int argc,char** argv) {
  StorageDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
