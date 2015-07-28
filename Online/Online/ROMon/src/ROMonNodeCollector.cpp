// $Id: ROMonNodeCollector.cpp,v 1.6 2010-10-14 13:30:09 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonNodeCollector.cpp,v 1.6 2010-10-14 13:30:09 frankb Exp $

// C++ include files
#include <iostream>

// Framework includes
#include "dic.hxx"
#include "RTL/rtl.h"
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/ROMonNodeCollector.h"

using namespace ROMon;

/// Standard constructor
ROMonNodeCollector::ROMonNodeCollector(int argc, char** argv) 
  : ROMonGblBuffer(argc,argv,ROMonNodeCollector::help)
{
  std::string match = "*";
  RTL::CLI cli(argc, argv, ROMonNodeCollector::help);
  cli.getopt("match",3,match);
  m_info.setMatch(match);
  m_info.setVerbose(m_verbose);
  m_info.start();
}

/// Help printout in case of -h /? or wrong arguments
void ROMonNodeCollector::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_node_collect -opt [-opt]\n");
  ROMonGblBuffer::help();
}

/// Start monitoring activity
int ROMonNodeCollector::monitor() {
  ROMonData gbl(data());
  if ( gbl.ptr ) {
    typedef RODimNodeListener::Clients Clients;
    bool exec = true;
    while(exec)    {
      Nodeset* n = gbl.nodeset;
      if ( n ) {  // Take GBL lock while updating
        RTL::Lock lock(m_lock);
        Nodeset::Nodes& nodes = n->reset()->nodes;
        Nodeset::Nodes::iterator it = nodes.reset();
        const Clients& cl = m_info.clients();
        ro_get_node_name(n->name,sizeof(n->name));
        n->type = Nodeset::TYPE;
        if ( ((char*)it) > gbl.str+m_section_size ) {
          log() << "Global section memory too small.....exiting" << std::endl;
          break;
        }
        else {
          DimLock dim_lock;
          for(Clients::const_iterator ic = cl.begin(); ic != cl.end(); ++ic) {
            typedef RODimNodeListener::Descriptor DSC;
            DSC* d = (*ic).second->data<DSC>();
            if ( ((char*)it) > gbl.str+m_section_size ) {
              log() << "Global section memory too small.....exiting" << std::endl;
              break;
            }
            ::memcpy(it,d->data,d->actual);
            it = nodes.add(it);
          }
        }
      }
      if ( m_print ) {
        log() << "========================" << ::lib_rtl_timestr() 
              << "========================" << std::endl
              << *n << std::endl;
      }
      ::lib_rtl_sleep(m_delay);
    }
    return 1;
  }
  log() << "ROMon gbl " << sectionName() << " invalid....exiting" << std::endl;
  return 0;
}

extern "C" int romon_node_collect(int argc, char** argv) {
  ROMonNodeCollector romon(argc,argv);
  print_startup("information NODE COLLECTOR");
  return romon.monitor();
}

