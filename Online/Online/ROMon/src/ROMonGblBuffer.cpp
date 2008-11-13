// $Id: ROMonGblBuffer.cpp,v 1.6 2008-11-13 12:13:33 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonGblBuffer.cpp,v 1.6 2008-11-13 12:13:33 frankb Exp $

// Framework includes
#include "ROMonDefs.h"
#include "ROMon/ROMonGblBuffer.h"
#include <stdexcept>

using namespace ROMon;

/// Standard constructor
ROMonGblBuffer::ROMonGblBuffer() 
  : m_gbl(0), m_delay(2000), m_print(0), m_mapOnly(false), m_lock(0),
    m_section_name("ROMonitor"), m_section_size(64)
{
}

/// Initializing constructor
ROMonGblBuffer::ROMonGblBuffer(int argc, char** argv, void (*help_fun)()) 
  : m_gbl(0), m_delay(2000), m_print(0), m_lock(0),
    m_section_name("ROMonitor"), m_section_size(64)
{
  RTL::CLI cli(argc, argv, help_fun);
  cli.getopt("gbl",1,m_section_name);
  cli.getopt("size",1,m_section_size);
  cli.getopt("delay",1,m_delay);
  m_print = cli.getopt("print",1) != 0;
  m_mapOnly = cli.getopt("maponly",3) != 0;
  m_verbose = cli.getopt("verbose",1) != 0;
  m_section_size *= 1024;
  initialize();
}

/// Initialize the global section
int ROMonGblBuffer::initialize() {
  int sc = m_mapOnly 
    ? ::lib_rtl_map_section(m_section_name.c_str(),m_section_size,&m_gbl)
    : ::lib_rtl_create_section(m_section_name.c_str(),m_section_size,&m_gbl);
  if ( !lib_rtl_is_success(sc) ) {
    std::string typ = (const char*)(m_mapOnly ? "map" : "create");
    std::string err = "Failed to "+typ+" ROMON global section:"+m_section_name;
    ::lib_rtl_output(LIB_RTL_ERROR,"%s of size:%d bytes\n",err.c_str(), m_section_size);
    throw std::runtime_error(err);
    return 0;
  }
  if ( m_verbose )    {
    log() << "Global section " << m_section_name << ((const char*)m_mapOnly ? " mapped" : " created")
          << " successfully:" << m_gbl << std::endl;
  }
#ifdef _WIN32
  sc = ::lib_rtl_create_lock(m_section_name.c_str(),&m_lock);
#else
  sc = ::lib_rtl_create_lock2(&((ROMonGBLData*)m_gbl->address)->lock,&m_lock);
#endif
  if ( !lib_rtl_is_success(sc) ) {
    ::lib_rtl_output(LIB_RTL_ERROR,"Failed to create ROMON global lock:%s\n",m_section_name.c_str());
    return 0;
  }
  if ( m_verbose )    {
    log() << "Global section lock " << m_section_name << " created successfully." << std::endl;
  }
  return 1;
}

/// Default destructor
ROMonGblBuffer::~ROMonGblBuffer() {
  if ( m_lock ) {
    ::lib_rtl_cancel_lock(m_lock);
    ::lib_rtl_delete_lock(m_lock);
    m_lock = 0;
  }
  if ( m_gbl ) {
    m_mapOnly ? ::lib_rtl_unmap_section(m_gbl) : ::lib_rtl_delete_section(m_gbl);
  }
  m_gbl = 0;
}

/// Access to shared memory data buffer
void* ROMonGblBuffer::data() const {
  if ( m_gbl ) {
    return ((ROMonGBLData*)m_gbl->address)->data;
  }
  return 0;
}

/// Help printout in case of -h /? or wrong arguments
void ROMonGblBuffer::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -g[bl]=<name>     Name of shared global section\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s[ize]=<number>  Size of the global section in kBytes\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -d[elay]=<number> Delay in milliseconds between two updates of the information\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -p[rint]=<number> Make printout to stdout\n");  
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -map[only         Only map the global section; do not create it.\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -v[erbose]        Verbose mode.\n");
}
