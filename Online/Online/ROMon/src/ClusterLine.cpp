// $Id: FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/FarmDisplayBase.cpp,v 1.51 2010-10-21 06:04:22 frankb Exp $

// Framework include files
#include "ROMon/ClusterLine.h"
#include "ROMon/FarmLineDisplay.h"
#include "CPP/IocSensor.h"
#include "RTL/Lock.h"
#include "SCR/scr.h"
#include "ROMonDefs.h"
extern "C" {
#include "dic.h"
}

#include <map>

using namespace ROMon;
using namespace SCR;
using namespace std;
#define CLUSTERLINE_START      2

namespace {  
  map<string,ClusterLineCreator_t>& creators() {
    static map<string,ClusterLineCreator_t> s_creators;
    return s_creators;
  }
}

/// Factory method: create a cluster line according to a given type
ClusterLine* ROMon::createClusterLine(const std::string& type, 
                                      FarmLineDisplay* parent, 
                                      const std::string& partition, 
                                      const std::string& title) 
{
  map<string,ClusterLineCreator_t>::const_iterator i=creators().find(type);
  if ( i != creators().end() )  {
    ClusterLineCreator_t func = (*i).second;
    return (*func)(parent,partition,title);
  }
  return 0;
}

/// Registry function
void ROMon::_registerCreator(const char* name,ClusterLineCreator_t func) {
  creators().insert(make_pair(name,func));
}

/// Standard constructor
ClusterLine::ClusterLine(FarmLineDisplay* p, const std::string& partition, const std::string& nam)
  : m_name(nam), m_partition(partition), m_svc(0), m_position(0), m_parent(p), m_data(0), m_ptr(0)
{
}

ClusterLine::~ClusterLine() {
  if ( m_svc ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

void ClusterLine::check(time_t /* now */) {
}

void ClusterLine::set_cursor()  {
  string val = (" "+strlower(m_name)+"   ").substr(0,10);
  ::scrc_put_chars(m_parent->display(),val.c_str(),
                   (this==m_parent->currentDisplay()?BLUE|INVERSE:NORMAL)|BOLD,position(),1,0);
}

/// Connect to data service
void ClusterLine::connect(const std::string& nam) {
  string svc = InternalDisplay::svcPrefix()+nam;
  if ( m_svc ) ::dic_release_service(m_svc);
  m_svc = ::dic_info_service((char*)svc.c_str(),MONITORED,0,0,0,dataHandler,(long)this,0,0);
}

/// Display function drawing on pasteboard the initial display
void ClusterLine::initialDisplay() {
}

void ClusterLine::display() {
  char text[256];
  time_t   t1  = time(0);
  size_t   pos = position();
  int      xp  = 42+CLUSTERLINE_START;
  Display* dis = m_parent->display();
  const Nodeset* c = data<Nodeset>();

  RTL::Lock lock(InternalDisplay::screenLock());
  ::strftime(text,sizeof(text)," %H:%M:%S ",::localtime(&t1));
  ::sprintf(text+strlen(text),"%20s   ","");
  begin_update(text);
  for(Nodeset::Nodes::const_iterator ci=c->nodes.begin(); ci != c->nodes.end(); ci = c->nodes.next(ci) ) {
    const char* n = (*ci).name;
    ::sprintf(text," %s ",n+((::strncmp(n,c->name,::strlen(c->name)+2) == 0) ? 0 : ::strlen(n)-2));
    ::scrc_put_chars(dis,text,RED|INVERSE,pos,xp,0);
    xp += ::strlen(text);
  }
  ::scrc_put_chars(dis," ",NORMAL,pos,xp,1);
  end_update();
}

void ClusterLine::begin_update(const char* text) {
  Pasteboard* pb  = m_parent->pasteboard();
  Display*    dis = m_parent->display();

  ::scrc_begin_pasteboard_update(pb);
  ::scrc_put_chars(dis," ", NORMAL,position(),1,1);
  if ( text ) {
    ::scrc_put_chars(dis,text,NORMAL,position(),12,0);    
  }
}

void ClusterLine::end_update() {
  ClusterDisplay* sfdis = m_parent->subfarmDisplay();
  Pasteboard*        pb = m_parent->pasteboard();
  const Nodeset*      c = data<Nodeset>();
  if ( sfdis ) {
    m_parent->set_cursor();
    ::scrc_cursor_on(pb);
  }
  else {
    ::scrc_cursor_off(pb);
  }
  set_cursor();
  ::scrc_end_pasteboard_update(pb);
  if ( sfdis && sfdis->clusterName() == c->name )  {
    IocSensor::instance().send(m_parent,CMD_CHECK,this);
  }
}

/// DIM command service callback
void ClusterLine::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    ClusterLine* l = *(ClusterLine**)tag;
    char* ptr = new char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    if ( l->m_ptr ) delete [] l->m_ptr;
    l->m_ptr = ptr;
    l->m_data = ((char*)l->m_ptr + sizeof(int));
    l->display();
    if ( l->m_parent )   {
      ptr = new char[*size+sizeof(int)];
      ::memcpy(ptr,l->m_ptr,*size+sizeof(int));
      IocSensor::instance().send(l->m_parent,CMD_NOTIFY,ptr);
    }
  }
  else if ( tag ) {
    ClusterLine* l = *(ClusterLine**)tag;
    l->initialDisplay();
  }
}
