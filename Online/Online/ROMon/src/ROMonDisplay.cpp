// $Id: ROMonDisplay.cpp,v 1.4 2008-02-08 21:20:40 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonDisplay.cpp,v 1.4 2008-02-08 21:20:40 frankm Exp $

// C++ include files
#include <cstdlib>

// Framework include files
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/ROMonDisplay.h"
#include "dic.hxx"

using namespace ROMon;

char* ROMonDisplay::Descriptor::reserve(size_t siz) {
  if ( siz > length ) {
    release();
    pointer = (char*)::malloc(length=siz);
  }
  return pointer;
}

/// Standard constructor
ROMonDisplay::ROMonDisplay() : m_svcID(0), m_delay(1000), m_lock(0)
{
}

/// Standard destructor
ROMonDisplay::~ROMonDisplay()   {
}

/// Initialize the data access
void ROMonDisplay::initialize()   {
  int sc = ::lib_rtl_create_lock(0,&m_lock);
  if ( !lib_rtl_is_success(sc) ) {
    begin_update();
    draw_line_reverse("Files to initialize display: %s",::lib_rtl_error_message(::lib_rtl_get_error()));
    end_update();
    return;
  }
  m_svcID = ::dic_info_service((char*)m_svcName.c_str(),MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Finalize data access
void ROMonDisplay::finalize()   {
  if ( m_svcID != 0 ) {
    ::dic_release_service(m_svcID);
    m_svcID = 0;
  }
  if ( m_lock ) {
    ::lib_rtl_cancel_lock(m_lock);
    ::lib_rtl_delete_lock(m_lock);
    m_lock = 0;
  }
}

/// Start the update cycle
void ROMonDisplay::update()   {
  const Nodeset* ns = m_data.data<const Nodeset>();
  if ( ns && m_data.actual>0 ) {
    if ( ns->type == Nodeset::TYPE ) {
      RTL::Lock lock(m_lock);
      dim_lock();
      updateDisplay(*ns);
      dim_unlock();
    }
    else if ( ns->type == Node::TYPE ) {
      RTL::Lock lock(m_lock);
      dim_lock();
      updateDisplay(*m_data.data<const Node>());
      dim_unlock();
    }
  }
}

/// Update all displays
void ROMonDisplay::updateDisplay(const Nodeset& /* ns */)   {
}

/// Update all displays
void ROMonDisplay::updateDisplay(const Node& /* ns */)   {
}

/// Run the interrupt loop
void ROMonDisplay::run()   {
  while(1) {
    update();
    ::lib_rtl_sleep(m_delay);
  }
}

/// DimInfoHandler overload
void ROMonDisplay::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    size_t len = *size;
    ROMonData data(address);
    ROMonDisplay* display = *(ROMonDisplay**)tag;
    Descriptor& d = display->data();
    switch(data.type())  {
    case Node::TYPE:
    case Nodeset::TYPE:
      break;
    default:
      // if ( !d.data ) d.data = (char;
      return;
    }
    RTL::Lock lock(display->lock());
   if ( d.length < len ) {
      d.length = len;
      d.reserve(size_t(1.2*len));
    }
    d.actual = len;
    ::memcpy(d.data<char>(),address,d.actual);
  }
}
