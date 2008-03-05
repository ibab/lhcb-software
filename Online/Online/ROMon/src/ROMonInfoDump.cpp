// $Id: ROMonInfoDump.cpp,v 1.2 2008-03-05 07:40:20 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/ROMonInfoDump.cpp,v 1.2 2008-03-05 07:40:20 frankb Exp $
// C++ include files
#include <iostream>
#include <iomanip>

// Framework includes
#include "dic.hxx"
#include "RTL/rtl.h"
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"
#include "ROMon/ROMonOstream.h"
#include "ROMon/ROMonPublish.h"
#include "ROMon/ROMonInfoDump.h"
#include "ROMonDefs.h"

using namespace ROMon;

/// Standard constructor
ROMonInfoDump::ROMonInfoDump(int argc, char** argv) : m_info(0)  {
  char def[32];
  std::string svc = "RONodePublish_"+RTL::nodeNameShort();
  RTL::CLI cli(argc, argv, ROMonPublish::help);
  cli.getopt("service",1,svc);
  ::memset(def,0xFF,sizeof(def));
  m_info = new DimInfo(svc.c_str(),(void*)def,sizeof(def),this);
}
	 
/// Default destructor
ROMonInfoDump::~ROMonInfoDump() {
  if ( m_info ) delete m_info;
}

/// Help printout in case of -h /? or wrong arguments
void ROMonInfoDump::help() {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"romon_dim_dump -opt [-opt]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -s[ervice]=<name> Name of DIM service providing the information\n");
}

/// DimInfo overload to process messages
void ROMonInfoDump::infoHandler() {
  const char* nam = getInfo()->getName();
  if ( nam ) {
    ROMonData data(getInfo()->getData());
    if ( data.ptr && data.type() != -1 ) {
      log() << "========================" << lib_rtl_timestr() << "========================" << std::endl;
      switch(data.type())  {
      case Node::TYPE:
	log() << *data.node << std::endl;
	break;
      case Nodeset::TYPE:
	log() << *data.nodeset << std::endl;
	break;
      default:
	log() << "Unknown data type -- cannot be published." << std::endl;
	break;
      }
    }
  }
}


extern "C" int romon_info_dump(int argc, char** argv) {
  ROMonInfoDump romon(argc,argv);
  print_startup("DUMP");
  while(1) ::lib_rtl_sleep(1000);
  return 1;
}

