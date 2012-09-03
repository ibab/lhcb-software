// $Id: StorageDisplay.cpp,v 1.17 2010-09-17 09:47:12 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/StorageDisplay.cpp,v 1.17 2010-09-17 09:47:12 frankb Exp $

// C++ include files
#include <ctime>
#include <string>
#include <cstdlib>
#include <iostream>

// Framework include files
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/StorageDisplay.h"
#include "dic.hxx"

namespace ROMon {
  struct DataWriterInfo {
    struct _RawItem {
      int v[2];
      short s[2];
      char  c[4];
    };
    struct _Item {
      int           id;
      int           value, val2;
      int           val3, val4;
      time_t        stamp;
      unsigned char hasData;
    };
    char name[32];
    _Item mbWritten;
    _Item openFiles;
    _Item totalFiles;
    _Item numSockets;
    _Item numThreads;
    _Item abnormals;
  };
}

using namespace ROMon;
using namespace std;
static const char *sstat[17] = {" nl","   ","*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac","SPR","WER","   "};
static const size_t NUM_STORE_NODES = 4;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;

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
  cout <<"  romon_storage -option [-option]" << endl
       <<"       -h[eaderheight]=<number>     Height of the header        display.                      " << endl
       <<"       -s[treamheight]=<number>     Height of the Streams       display.                      " << endl
       <<"       -b[ufferheight]=<number>     Height of the Buffers       display.                      " << endl
       <<"       -l[oggerheight]=<number>     Height of the Logger        display.                      " << endl
       <<"       -f[ilesheight]=<number>      Height of the Files         display.                      " << endl
       <<"       -w[idthhltrec]=<number>      Height of the HLT receivers display.                      " << endl
       <<"       -d[elay]=<number>            Time delay in millisecond between 2 updates.              " << endl
       <<"       -s[ervicename]=<name>        Name of the DIM service  providing monitoring information." << endl
       <<"       -p[partition]=<name>         Name of the partition to be displayed.                    " << endl
       << endl;

}

/// DimInfoHandler overload
static void dataWrHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0) {
    DataWriterInfo::_Item*    v = *(DataWriterInfo::_Item**)tag;
    DataWriterInfo::_RawItem* r = (DataWriterInfo::_RawItem*)address;
    v->value   = r->v[0];
    v->val2    = r->v[1];
    v->val3    = r->s[0];
    v->val4    = r->s[1];
    v->stamp   = ::time(0);
    v->hasData = 1;
  }
}

/// Static abstract object creator.
ClusterDisplay* ROMon::createStorageDisplay(int width, int height, int posx, int posy, int argc, char** argv) {
  return new StorageDisplay(width,height,posx,posy,argc,argv);
}

/// Initializing constructor for using display as sub-display
StorageDisplay::StorageDisplay(int width, int height, int posx, int posy, int argc, char** argv)
  : ClusterDisplay(width, height), m_wrInfo(0)
{
  m_position = Position(posx,posy);
  init(1, argc, argv);
}

/// Standard constructor
StorageDisplay::StorageDisplay(int argc, char** argv) : m_wrInfo(0) {
  init(0, argc,argv);
}

/// Initialize the display
void StorageDisplay::init(int flag, int argc, char** argv)   {
  RTL::CLI cli(argc,argv,help);
  int hdr_height, hlt_width, hlt_height, hlt_posy, buff_height, strm_height, logg_height;
  int right, width, height, posx, posy;
  cli.getopt("headerheight",  1, hdr_height    =    5);
  cli.getopt("streamheight",  1, strm_height   =   12);
  cli.getopt("loggerheight",  1, logg_height   =    9);
  cli.getopt("widthhltrec",   1, hlt_width     =   27);
  cli.getopt("bufferheight",  1, buff_height   =   10);
  cli.getopt("delay",         1, m_delay       = 1000);
  cli.getopt("servicename",   1, m_svcName     = "/storectl01/ROpublish");
  cli.getopt("partitionname", 1, m_partName    = "LHCb");

  m_select = 0;
  setup_window();
  posx  = m_position.x-2;
  posy  = m_position.y-2;
  hlt_posy = posy+hdr_height;
  hlt_height = m_area.height-hdr_height;
  m_select = 0;
  if ( flag ) {
    m_select  = createSubDisplay(Position(posx,posy+hdr_height),Area(hlt_width, 6),"  Node Selector");
    hlt_posy += 7;
    hlt_height -= 7;
  }
  m_hltRec  = createSubDisplay(Position(posx,hlt_posy), Area(hlt_width,hlt_height),"Farm receivers");
  right = m_hltRec->right();
  width = m_area.width-hlt_width-2;
  height= m_area.height-hdr_height-buff_height-strm_height-logg_height-4;
  m_buffers = createSubDisplay(Position(right,posy+hdr_height), Area(width,buff_height),      "Buffer Monitor");
  m_streams = createSubDisplay(Position(right,m_buffers->bottom()-1),Area(width,strm_height), "Stream Information");
  m_logging = createSubDisplay(Position(right,m_streams->bottom()-1),Area(width,logg_height), "Logger Summary");
  m_files   = createSubDisplay(Position(right,m_logging->bottom()-1),Area(width,height+1),    "File Information");
  end_update();
  m_wrInfo = new DataWriterInfo[NUM_STORE_NODES];
  ::memset(m_wrInfo,0,sizeof(DataWriterInfo)*NUM_STORE_NODES);
  for(size_t i=0; i<NUM_STORE_NODES; ++i) {
    char text[256];
    std::string svc;
    DataWriterInfo& w = m_wrInfo[i];
    ::sprintf(w.name,"store%02d",int(i+1));
    ::sprintf(text,"PUBLISHERD_%s/Counters/MBWritten",w.name);
    w.mbWritten.id  = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.mbWritten,0,0);
    ::sprintf(text,"PUBLISHERD_%s/Counters/OpenFiles",w.name);
    w.openFiles.id  = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.openFiles,0,0);
    ::sprintf(text,"PUBLISHERD_%s/Counters/TotalFiles",w.name);
    w.totalFiles.id = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.totalFiles,0,0);
    ::sprintf(text,"PUBLISHERD_%s/Counters/NumSockets",w.name);
    w.numSockets.id = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.numSockets,0,0);
    ::sprintf(text,"PUBLISHERD_%s/Counters/NumThreads",w.name);
    w.numThreads.id = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.numThreads,0,0);
    ::sprintf(text,"PUBLISHERD_%s/Counters/Abnormals",w.name);
    w.abnormals.id  = ::dic_info_service(text,MONITORED,0,0,0,dataWrHandler,(long)&w.abnormals,0,0);
  }
}

/// Standard destructor
StorageDisplay::~StorageDisplay()   {
  if ( m_wrInfo ) {
    for(size_t j=0; j<NUM_STORE_NODES; ++j) {
      DataWriterInfo& i = m_wrInfo[j];
      if ( i.mbWritten.id  ) ::dic_release_service(i.mbWritten.id);
      if ( i.openFiles.id  ) ::dic_release_service(i.openFiles.id);
      if ( i.totalFiles.id ) ::dic_release_service(i.totalFiles.id);
      if ( i.numSockets.id ) ::dic_release_service(i.numSockets.id);
      if ( i.numThreads.id ) ::dic_release_service(i.numThreads.id);
      if ( i.abnormals.id  ) ::dic_release_service(i.abnormals.id);
    }
  }
  delete [] m_wrInfo;
  m_wrInfo = 0;
  begin_update();
  delete m_hltRec;
  delete m_buffers;
  delete m_streams;
  delete m_logging;
  delete m_files;
  if ( m_select ) delete m_select;
  end_update();
}

/// Number of nodes in the dataset
size_t StorageDisplay::numNodes() {
  return 4;
}

/// Retrieve cluster name from cluster display
std::string StorageDisplay::clusterName() const {
  return "storectl01";
}

/// Retrieve node name from cluster display by offset
std::string StorageDisplay::nodeName(size_t offset) {
  switch(offset) {
  case 0:    return "storerecv01";
  case 1:    return "storerecv02";
  case 2:    return "storestrm01";
  case 3:    return "storestrm02";
  default:   return "";
  }
}

/// Access Node display
MonitorDisplay* StorageDisplay::nodeDisplay() const {
  return m_select;
}

#define PRT(x,y)  {  ::sprintf(text,"%-15s ",#y);\
  for(size_t j=0; j<NUM_STORE_NODES; ++j) {\
    DataWriterInfo& i = m_wrInfo[j]; \
    (i.x.hasData)  ? ::sprintf(&text[strlen(text)-1],"%12d ",i.x.value)  : ::sprintf(&text[strlen(text)-1],"%12s ","----");}}\


/// Show the data logging status
void StorageDisplay::showLogging() {
  char text[256];
  MonitorDisplay* disp = m_logging;
  //DataWriterInfo  wi;
  //wi.space = wi.free_space = wi.open_files = wi.closed_files = wi.transferred_files = wi.deleted_files = 0;
  //strcpy(wi.data_dir,"/daqarea/2008/RAW/LHCb/Physics/0123456789");
  //disp->draw_line_reverse("File Information for Run %-10                                         ",0);
  //disp->draw_line_normal("Space:%9d MB [%4d TB] Free Space: %9d MB [%4d TB]", 
  //                       wi.space,wi.space/1024/1024,wi.free_space,wi.free_space/1024/1024);
  //disp->draw_line_normal("Open:%5d  Closed:%5d Transferring:%5d Deleted:%5d Total:%5d     ",
  //                       wi.open_files,wi.closed_files,wi.transferred_files,wi.deleted_files,
  //                       wi.open_files+wi.closed_files+wi.transferred_files+wi.deleted_files);
  //disp->draw_line_normal("Directory:%s", wi.data_dir);

  disp->draw_line_reverse("Writerd summary information from publishers:");
  ::sprintf(text,"%-15s ","Node");
  for(size_t j=0; j<NUM_STORE_NODES; ++j)
    ::sprintf(&text[strlen(text)-1],"%12s ",m_wrInfo[j].name);
  disp->draw_line_normal(text);
  PRT(mbWritten, MB written...);
  disp->draw_line_normal(text);
  PRT(totalFiles,Total Files..);
  disp->draw_line_normal(text);
  PRT(openFiles, Open files...);
  disp->draw_line_normal(text);
  PRT(numSockets,Num.Sockets..);
  disp->draw_line_normal(text);
  PRT(numThreads,Num.Threads..);
  disp->draw_line_normal(text);
  PRT(abnormals, Anomalies....);
  disp->draw_line_normal(text);
}

/// Show the file information
void StorageDisplay::showFiles() {
  MonitorDisplay* disp = m_files;
  disp->draw_line_reverse("File                    Destination Size[Bytes]  Events     State           ");
}

/// Display the Stream information
void StorageDisplay::showStreams(const Nodeset& ns) {
  map<string,StorageDisplay::Stream> streams;
  MonitorDisplay* disp = m_streams;
  string part = m_partName + "_";
  disp->draw_line_reverse("Stream         Source       Target              Sent    Received     Written   ");

  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        const MBMClient& c = *ic;
        if (strncmp(c.name,part.c_str(),part.length())==0) {
          char nam[BM_USER_NAME_LEN], *typ, *str, *node, *ptr;
          ::strcpy(nam,c.name);
          node = nam+part.length();
          if ( (typ=nullstr(nam,"_SND")) ){
            *(typ+3) = 0;
            ptr = nullchr(str=typ+4,'_');
            if ( !ptr ) {
              const char* s = "MONA0801";
              streams[s].node = s;
              streams[s].sent += c.events;
              streams[s].source = "Recv Layer";
              continue;
            }
            else if ( (ptr=::strstr(ptr+1,"MONA09")) ) {
              const char* s = nullchr(ptr,'_') ? ptr : "MONA0901";
              streams[s].node = s;
              streams[s].sent += c.events;
              streams[s].source = "Recv Layer";
              continue;
            }
	    streams[str].sent += c.events;
	    streams[str].source = node;
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
  for(map<string,Stream>::const_iterator i=streams.begin();i!=streams.end();++i) {
    const Stream& s = (*i).second;
    if ( ::strncmp(s.node.c_str(),"MONA0",5) != 0 )  {
      disp->draw_line_normal("%-14s%12s->%-12s %11d %11d %11d",
                             (*i).first.c_str(),s.source.c_str(),s.node.c_str(),
                             s.sent,s.received,s.written);
      total.sent += s.sent;
      total.written += s.written;
      total.received += s.received;
    }
  }
  //disp->draw_line_normal("");
  disp->draw_line_bold("%-14s%12s->%-12s %11d %11d %11d","Total","Recv Layer","Stream Layer",
                       total.sent,total.received,total.written);
  disp->draw_line_normal("");
  for(map<string,Stream>::const_iterator i=streams.begin();i!=streams.end();++i) {
    const Stream& s = (*i).second;
    if ( ::strncmp(s.node.c_str(),"MONA0",5) == 0 )  {
      disp->draw_line_normal("%-14s%12s->%-15s%9d",(*i).first.c_str(),s.source.c_str(),s.node.c_str(),s.sent);
    }
  }
}

/// Show the information about the HLT receiver tasks
void StorageDisplay::showHLT(const Nodeset& ns) {
  MonitorDisplay* disp = m_hltRec;
  string part = m_partName + "_";
  const Nodes& nodes = ns.nodes;
  long total_evts = 0;
  disp->draw_line_reverse("%-11s%8s MBM  ST","Source","Received");
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
  string part = m_partName + "_";
  disp->draw_line_reverse("Buffer                        Produced Pending Size[kB] Free[kB] Slots Users ");
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    const Buffers& buffs = *(*n).buffers();
    for (Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
      const Clients& clients = (*ib).clients;
      for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
        if (strncmp((*ic).name,part.c_str(),part.length())==0) {
          const MBMBuffer& b = *ib;
          const CONTROL& c = b.ctrl;
          disp->draw_line_normal("%10s/%-13s %12d %7d %8d %8d %5d %5d ",
                                 n->name,b.name,c.tot_produced,c.i_events,
                                 (c.bm_size*c.bytes_p_Bit)/1024,(c.i_space*c.bytes_p_Bit)/1024,
                                 c.p_emax-c.i_events,c.i_users);
          break;
        }
      }
    }
  }
}

/// Update header information
void StorageDisplay::showHeader(const Nodeset& ns)   {
  char b1[64], b2[64];
  TimeStamp frst=ns.firstUpdate(), last=ns.lastUpdate();
  time_t t1 = frst.first, t2 = last.first;
  ::strftime(b1,sizeof(b1),"%H:%M:%S",::localtime(&t1));
  ::strftime(b2,sizeof(b1),"%H:%M:%S",::localtime(&t2));
  draw_line_normal ("");
  draw_line_reverse("                     Storage Monitor for partition %s on %s   [%s]",
                    m_partName.c_str(), RTL::nodeNameShort().c_str(), ::lib_rtl_timestr());    
  draw_line_bold   ("                     Information updates date between: %s.%03d and %s.%03d",b1,frst.second,b2,last.second);
  draw_line_normal ("");
  for (Nodes::const_iterator n=ns.nodes.begin(); n!=ns.nodes.end(); n=ns.nodes.next(n))  {
    char* c = ::strchr((char*)(*n).name,'.');
    if ( c ) *c = 0;
  }
}

/// Update selector information
void StorageDisplay::showSelector(const Nodeset& /* ns */)   {
  if ( m_select )    {
    m_select->draw_line_normal("Move curser, then command");
    m_select->draw_line_normal("CTRL-H for help");
    m_select->draw_line_bold  ("  storerecv01");
    m_select->draw_line_bold  ("  storerecv02");    
    m_select->draw_line_bold  ("  storestrm01");
    m_select->draw_line_bold  ("  storestrm02");
  }
}

/// Update all displays
void StorageDisplay::updateDisplay(const Nodeset& ns)   {
  begin_update();
  if ( m_select ) m_select->begin_update();
  m_hltRec->begin_update();
  m_buffers->begin_update();
  m_streams->begin_update();
  m_logging->begin_update();
  m_files->begin_update();

  showHeader(ns);
  showHLT(ns);
  showBuffers(ns);
  showStreams(ns);
  showLogging();
  showFiles();
  showSelector(ns);

  m_files->end_update();
  m_logging->end_update();
  m_streams->end_update();
  m_buffers->end_update();
  m_hltRec->end_update();
  if ( m_select ) m_select->end_update();
  end_update();
}

extern "C" int romon_storage(int argc,char** argv) {
  StorageDisplay disp(argc,argv);
  disp.initialize();
  disp.run();
  return 1;
}
