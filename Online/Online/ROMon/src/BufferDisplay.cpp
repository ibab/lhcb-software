// $Id: BufferDisplay.cpp,v 1.5 2010-10-15 07:42:00 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/BufferDisplay.cpp,v 1.5 2010-10-15 07:42:00 frankb Exp $

// Framework include files
#include "ROMon/FarmDisplay.h"
#include "ROMon/CPUMon.h"
#include "CPP/Event.h"
#include "SCR/scr.h"
#include "sstat.h"

// C++ include files
#include <cstdlib>

using namespace ROMon;
using namespace SCR;
using namespace std;

typedef Nodeset::Nodes               Nodes;
typedef Node::Buffers                Buffers;
typedef MBMBuffer::Clients           Clients;
typedef Node::Tasks                  Tasks;
typedef vector<string>               StringV;

BufferDisplay::BufferDisplay(InternalDisplay* parent, const string& title) 
  : InternalDisplay(parent,title), m_node(0)
{
  ::scrc_create_display(&m_display,55,130,MAGENTA,ON,"MBM Monitor display for node:");
}

void BufferDisplay::update(const void* data) {
  const Nodeset* ns = (const Nodeset*)data;
  char txt[1024], name[128];
  int line = 0, node = 0;
  if ( 0 != ns ) {
    string key;
    map<string,string> entries;
    StringV lines;
    string nam;
    char *p, *cnam;
    Nodes::const_iterator n;

    for (n=ns->nodes.begin(), line=1; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
        time_t tim = (*n).time;
        const Buffers& buffs = *(*n).buffers();
        ::strftime(name,sizeof(name),"%H:%M:%S",::localtime(&tim));
        ::sprintf(txt,"MBM Monitor display for node:%s  [%s]",(*n).name,name);
        ::scrc_set_border(m_display,txt,BLUE|INVERSE);
        for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
          const Buffers::value_type::Control& c = (*ib).ctrl;
          const char* bnam = (char*)(*ib).name;
          //if ( ::strlen(bnam)>10 ) {
          //  p = strchr(bnam,'_');
          //  if ( p ) *p = 0;
          //}
          ::sprintf(name," Buffer \"%s\"",bnam);
          ::sprintf(txt,"%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d",
                    name, c.tot_produced, c.tot_actual, c.tot_seen, c.i_events, c.p_emax);
          ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
          ::sprintf(txt,"%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]",
                    "",(c.bm_size*c.bytes_p_Bit)/1024, (c.i_space*c.bytes_p_Bit)/1024, 
                    c.i_users, c.p_umax);
          ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
          ::scrc_put_chars(m_display,"  Occupancy [Events]:",NORMAL,++line,1,1);
          draw_bar(29,line,float(c.i_events)/float(c.p_emax),95);
          ::scrc_put_chars(m_display,"            [Space]: ",NORMAL,++line,1,1);
          draw_bar(29,line,float(c.bm_size-c.i_space)/float(c.bm_size),95);
          ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
        }
        break;
      }
    }
    if ( line > 1 ) {
      ::sprintf(txt,"%-22s%5s%6s %4s%12s %-4s %s","   Name","Part","PID","State","Seen/Prod","REQ","Buffer");
      ::scrc_put_chars(m_display,txt,INVERSE,++line,1,0);
      ::scrc_put_chars(m_display,txt,INVERSE,line,3+m_display->cols/2,1);
    }
    else {
      time_t t = ::time(0);
      ::scrc_put_chars(m_display,"   Unknown Node. No buffers found.",INVERSE|BOLD,++line,1,1);
      ::strftime(txt,sizeof(txt),"           %H:%M:%S",::localtime(&t));
      ::scrc_put_chars(m_display,txt,INVERSE|BOLD,++line,1,1);
      ::scrc_set_border(m_display,"Unknown Node. No buffers found.",INVERSE|RED|BOLD);
    }
    for (n=ns->nodes.begin(), node=0; n!=ns->nodes.end(); n=ns->nodes.next(n), ++node)  {
      if ( node == m_node ) {
        const Buffers& buffs = *(*n).buffers();
        for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
          const Clients& clients = (*ib).clients;
          const char* bnam = (const char*)(*ib).name;
          if ( ::strncmp(bnam,"Events_",7)==0 ) bnam += 7;
          if ( ::strncmp(bnam,"Output_",7)==0 ) bnam = "Output";
          if ( ::strncmp(bnam,"Input_",6)==0  ) bnam = "Input";
          for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
            Clients::const_reference c = (*ic);
            cnam = (char*)c.name;
	    size_t mx_len = 20;
            if ( ::strlen(cnam)>mx_len && (p=::strchr(cnam,'_')) ) {// Cleanup name if too long to be displayed properly...
              cnam = ++p;
              if ( ::strlen(cnam)>mx_len && (p=::strchr(cnam,'_')) ) {
                cnam = ++p;
                if ( ::strlen(cnam)>mx_len && (p=::strchr(cnam,'_')) ) {
                  cnam = ++p;
                  if ( ::strlen(cnam)>mx_len && (p=::strchr(cnam,'_')) ) {
                    cnam = ++p;
                  }
                }
              }
            }
            if ( c.type == 'C' )
              ::sprintf(txt,"%-22s%5X%6d C%4s%12d %c%c%c%c %s",cnam,c.partitionID,c.processID,
                        sstat[(size_t)c.state],c.events,c.reqs[0],c.reqs[1],c.reqs[2],c.reqs[3],bnam);
            else if ( c.type == 'P' )
              ::sprintf(txt,"%-22s%5X%6d P%4s%12d %4s %s",cnam,c.partitionID,c.processID,
                        sstat[(size_t)c.state],c.events,"",bnam);
            else
              ::sprintf(txt,"%-22s%5X%6d ?%4s%12s %4s %s",cnam,c.partitionID,c.processID,"","","",bnam);
            key = bnam;
            key += c.name;
            entries[key] = txt;
          }
        }
        break;
      }
    }
    lines.clear();
    for(map<string,string>::const_iterator m=entries.begin();m!=entries.end();++m) {
      lines.push_back((*m).second);
    }
    
    for(size_t i=0,len=lines.size(),cnt=len/2+(len%2),j=cnt; i<cnt; ++i, ++j)  {
      if ( j<len ) {
        ::sprintf(name,"%%-%ds  %%-%ds",m_display->cols/2,m_display->cols/2);
        ::sprintf(txt,name,lines[i].c_str(),lines[j].c_str());
      }
      else {
        ::sprintf(name,"%%-%ds",m_display->cols);
        ::sprintf(txt,name,lines[i].c_str());
      }
      ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
    }
  }
  ::scrc_put_chars(m_display,"",NORMAL,++line,1,1);
  ::scrc_put_chars(m_display,"      << Mouse-Left-Double-Click down here to close the window >>",NORMAL,++line,1,1);
  ::memset(txt,' ',m_display->cols);
  txt[m_display->cols-1]=0;
  while(line<m_display->rows)
    ::scrc_put_chars(m_display,txt,NORMAL,++line,1,1);
}
