// $Id: InternalDisplay.cpp,v 1.3 2010-10-12 17:47:05 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/InternalDisplay.cpp,v 1.3 2010-10-12 17:47:05 frankb Exp $

#include "ROMon/InternalDisplay.h"
#include "ROMon/Constants.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include "RTL/Lock.h"
#include "SCR/scr.h"
extern "C" {
#include "dic.h"
}
#include <cstring>

using namespace ROMon;
using namespace SCR;
using namespace std;

static lib_rtl_lock_t    s_lock = 0;


/// Global screen lock shared by all displays
lib_rtl_lock_t InternalDisplay::screenLock() {
  if ( 0 == s_lock ) {
    ::lib_rtl_create_lock(0,&s_lock);
  }
  return s_lock;
}

InternalDisplay::InternalDisplay(InternalDisplay* parent, const string& title) 
: m_pasteboard(0), m_display(0), m_parent(parent), m_name(title), m_title(title), m_svc(0)
{
  m_pasteboard = m_parent ? m_parent->pasteboard() : 0;
  m_lastUpdate = time(0);
  screenLock();
}

InternalDisplay::~InternalDisplay() {
  disconnect();
  close();
}

void InternalDisplay::close() {
  if ( m_display ) {
    ::scrc_unpaste_display(m_display,m_pasteboard);
    ::scrc_delete_display(m_display);
    m_display = 0;
  }
}

void InternalDisplay::disconnect() {
  if ( m_svc != 0 ) {
    ::dic_release_service(m_svc);
    m_svc = 0;
  }
}

void InternalDisplay::show(int row, int col) {
  ::scrc_paste_display (m_display, m_pasteboard, row, col);
}

void InternalDisplay::hide() {
  ::scrc_unpaste_display(m_display,m_pasteboard);
}

/// Draw bar to show occupancies
size_t InternalDisplay::draw_bar(int x, int y, float f1, int scale)  {
  size_t l = size_t(float(scale)*f1);
  char txt[1024];
  memset(txt,' ',l);
  txt[l] = 0;
  ::scrc_put_chars(m_display,txt,INVERSE,y,x,0);
  memset(txt,'.',scale-l);
  txt[scale-l] = 0;
  ::scrc_put_chars(m_display,txt,NORMAL,y,l+x,1);
  return 1;
}

/// Update display content
void InternalDisplay::update(const void* data, size_t /* len */) {
  return update(data);
}

/// DIM command service callback
void InternalDisplay::dataHandler(void* tag, void* address, int* size) {
  if ( address && tag && *size > 0 ) {
    InternalDisplay* disp = *(InternalDisplay**)tag;
    unsigned char* ptr = new unsigned char[*size+sizeof(int)];
    *(int*)ptr = *size;
    ::memcpy(ptr+sizeof(int),address,*size);
    IocSensor::instance().send(disp,CMD_UPDATE,ptr);
  }
}

/// Interactor overload: Display callback handler
void InternalDisplay::handle(const Event& ev)    {
  switch(ev.eventtype) {
  case IocEvent: {
    switch(ev.type) {
    case CMD_UPDATE: {
      RTL::Lock lock(s_lock);
      Pasteboard* pb = pasteboard();
      unsigned char* ptr = (unsigned char*)ev.data;
      if ( pb ) {
	::scrc_cursor_off(pb);
	::scrc_begin_pasteboard_update (pb);
      }
      update(ptr + sizeof(int), *(int*)ptr);
      if ( pb ) ::scrc_end_pasteboard_update(pb);
      if ( parent() ) parent()->set_cursor(this);
      if ( pb ) ::scrc_cursor_on(pb);
      delete [] ptr;
      break;
    }
    default:
      break;
    }
    break;
  }
  default:
    break;
  }
}
