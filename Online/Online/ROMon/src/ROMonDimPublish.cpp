// $Id: ROMonDimPublish.cpp,v 1.2 2008-04-22 15:50:02 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonDimPublish.cpp,v 1.2 2008-04-22 15:50:02 frankb Exp $
// C++ include files
#include <stdexcept>

// Framework includes
#include "dic.h"
#include "dis.hxx"
#include "RTL/Lock.h"
#define MBM_IMPLEMENTATION
#include "ROMonDefs.h"
#include "ROMon/ROMon.h"
#include "ROMon/CPUMon.h"
#include "ROMon/ROMonDimPublish.h"

using namespace ROMon;

/// Standard constructor
ROMonDimPublish::ROMonDimPublish(int argc , char** argv) : ROMonPublish(argc,argv) {
  RTL::CLI cli(argc, argv, ROMonPublish::help);
  std::string svc;
  cli.getopt("publish",1,svc);
  if ( !svc.empty() )  {
    m_service = ::dis_add_service((char*)svc.c_str(),"C",0,0,feedData,(long)this);
  }
  else  {
    log() << "Unknown data type -- cannot be published." << std::endl;
    throw std::runtime_error("Unknown data type and unknwon service name -- cannot be published.");
  }
}

/// Default destructor
ROMonDimPublish::~ROMonDimPublish() {
  ::dis_remove_service(m_service);
}

void ROMonDimPublish::feedData(void* tag, void** buf, int* size, int* first) {
  static const char* empty = "";
  ROMonDimPublish* h = *(ROMonDimPublish**)tag;
  if ( !(*first) ) {
    RTL::Lock lock(h->m_lock);
    ROMonData gbl(h->data());
    CPUMonData cpu(h->data());
    *buf = gbl.ptr;
    switch(gbl.type()) {
    case Node::TYPE:
      *size  = gbl.node->totalSize;
      break;
    case Nodeset::TYPE:
      *size  = gbl.nodeset->length();
      break;
    case CPUfarm::TYPE:
      *size  = cpu.farm->length();
      break;
    case ProcFarm::TYPE:
      *size  = cpu.procFarm->length();
      break;
    default:
      *size = 0;
      log() << "Unknown data type -- cannot be published." << std::endl;
      return;
    }
    if ( h->m_verbose ) {
      log() << "[ROMonDimPublish] Published " << *size 
	    << " Bytes of data @" << *buf << std::endl;
    }
    return;
  }
  *size = 0;
  *buf = (void*)empty;
}

/// Publish the collected information
void ROMonDimPublish::publish() {
  /* int nclients  = */ ::dis_update_service(m_service);
}

extern "C" int romon_dimserver(int argc, char** argv) {
  ROMonDimPublish romon(argc,argv);
  DimServer::start(RTL::processName().c_str());
  print_startup("information SERVER");
  return romon.monitor();
}

