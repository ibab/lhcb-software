// $Id: ROMonDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonDisplay.cpp,v 1.11 2010-10-19 15:36:26 frankb Exp $

// C++ include files
#include <cstdlib>

// Framework include files
#include "RTL/Lock.h"
#include "CPP/IocSensor.h"

#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
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

/// Initializing constructor
ROMonDisplay::ROMonDisplay(int width, int height)
  : ScrDisplay(width,height), m_svcID(0), m_svcID2(0), m_delay(1000), m_lock(0), m_readAlways(false)
{
}

/// Standard constructor
ROMonDisplay::ROMonDisplay() : m_svcID(0), m_svcID2(0), m_delay(1000), m_lock(0), m_readAlways(false)
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
    draw_line_reverse("Failed to initialize display: %s",::lib_rtl_error_message(::lib_rtl_get_error()));
    end_update();
    return;
  }
  m_svcID = ::dic_info_service((char*)m_svcName.c_str(),MONITORED,0,0,0,infoHandler,(long)this,0,0);
}

/// Finalize data access
void ROMonDisplay::finalize()   {
  try {
    if ( m_svcID2 != 0 ) {
      ::dic_release_service(m_svcID2);
      m_svcID2 = 0;
    }
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
  catch(...) {
  }
}

/// Start the update cycle
void ROMonDisplay::update()   {
  const Nodeset* ns = m_data.data<const Nodeset>();
  if ( ns && m_data.actual>0 ) {
    if ( ns->type == Nodeset::TYPE ) {
      DimLock dim_lock;
      RTL::Lock lock(m_lock);
      updateDisplay(*ns);
    }
    else if ( ns->type == Node::TYPE ) {
      DimLock dim_lock;
      RTL::Lock lock(m_lock);
      updateDisplay(*m_data.data<const Node>());
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
    //IocSensor::instance().send(this,CMD_UPDATEDISPLAY,this);
    update();
    ::lib_rtl_sleep(m_delay);
  }
}

/// Interactor overload: Display callback handler
void ROMonDisplay::handle(const CPP::Event& ev) {
  switch(ev.eventtype) {
  case TimeEvent:
    if ( ev.timer_data == (void*)CMD_UPDATEDISPLAY ) 
      update();
    break;
  case IocEvent:
    if ( ev.type == CMD_UPDATEDISPLAY ) 
      update();
  default:
    break;
  }
}

/// DimInfoHandler overload
void ROMonDisplay::infoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0) {
    size_t len = *size;
    ROMonData data(address);
    ROMonDisplay* display = *(ROMonDisplay**)tag;
    Descriptor& d = display->m_data;
    switch(data.type())  {
    case Node::TYPE:
    case Nodeset::TYPE:
      break;
    default:
      if ( !display->m_readAlways ) return;
    }
    RTL::Lock lock(display->lock());
    if ( d.length < len+1 ) {
      d.length = len+1;
      d.reserve(size_t(1.2*len));
    }
    d.actual = len;
    ::memcpy(d.data<char>(),address,d.actual);
    d.data<char>()[len]=0;
  }
}
