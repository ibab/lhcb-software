// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================

// System / Boost include files
#include <ctime>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

// Framework include files
#include "TAN/TanDB.h"
#include "ASIO/TanInterface.h"

static void help()  {
  printf("Usage : inquire -opt <opts>\n");
  printf("        -t[ask]=<name>        supply task name; default:MYTASK\n");
  printf("        -n[ode]=<name>        supply host name; default:<localhost>\n");
  printf("        -i[nquire]            also inquire address for each connection and alias\n");
  printf("        -a[liases]            also declare task aliases for each connection\n");
  printf("        -q[uiet]              quiet mode    \n");
  printf("        -m[axtask]=<number>   Maximal number of poart allocations without free \n");
  printf("        -c[ontinuous]         contious mode \n");
  printf("        -w[ait]               wait for terminal response between step\n");
  printf("        -?, -h                get this help \n");
  exit(0);
}

extern "C" int boost_asio_tan_alloc_test ( int argc, char* argv[] )  {
  // ----------------------------------------------------------------------------
  //
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  RTL::CLI cli(argc,argv,help);
  int status = 1, MAXTASKS = 20;
  char buff[256];
  std::string task_name = "MYTASK", host_name;
  bool continuous = cli.getopt("continuous",1) != 0;
  bool quiet      = cli.getopt("quiet",1) != 0;
  bool do_alias   = cli.getopt("alias",1) != 0;
  bool do_inquire = cli.getopt("inquire",1) != 0;
  bool do_wait    = cli.getopt("wait",1) != 0;

  cli.getopt("node",1,host_name=RTL::nodeName());
  cli.getopt("task",1,task_name);
  cli.getopt("maxtask",1,MAXTASKS);

  time_t start = ::time(0);
  int alloc = 0, dealloc = 0, alias = 0, inquire = 0;
  int host_len = host_name.find("::") == std::string::npos 
    ? host_name.length()+2 : host_name.find("::");

  TanInterface::instance();
  NetworkChannel::Port* ports = new NetworkChannel::Port[MAXTASKS];
  do  {
    memset(ports,0,MAXTASKS*sizeof(NetworkChannel::Port));
    for ( int i = 0; i < MAXTASKS; i++ )  {
      ports[i] = 0;
      ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
      status = ::tan_allocate_new_port_number (buff, &ports[i]);
      alloc++;
      if ( status == TAN_SS_SUCCESS )  {
        if ( !quiet ) ::printf("allocatePort: %s Port:%04X  status:%d  [%s]\n",
			       buff+host_len, ports[i], status, ::strerror(errno));
      }
      else   {
        ::printf("FAILED: allocatePort(%s): %d [%s]\n", buff, status, ::strerror(errno));
      }
      for ( int j = 0; do_alias && j < i; j++ )           {
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d_%02d",host_name.c_str(),task_name.c_str(),i,j);
        status = ::tan_declare_alias(buff);
        alias++;
        if ( !quiet || status != TAN_SS_SUCCESS ) {
          ::printf("     declareAlias: %s   status:%d %08X errno:%d  [%s]\n", 
                   buff+host_len, status, status, errno, ::strerror(errno));
        }
      }
      if ( !quiet && do_wait )  {
        ::printf("Hit any key to continue\n");
        ::getchar();
      }
    }

    for ( int i = 0; do_inquire && i < MAXTASKS; i++ )  {
      TanMessage::Address addr;
      ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
      status = ::tan_get_address_by_name(buff, &addr);
      ++inquire;
      if ( status == TAN_SS_SUCCESS )  {
        if ( !quiet ) ::printf("     address_by_name: %s Port:%04X  status:%d  [%s]\n", 
			       buff+host_len, addr.sin_port, status, ::strerror(errno));
      }
      else   {
        ::printf("     FAILED: address_by_name(%s): %d [%s]\n", buff+host_len, status, ::strerror(errno));
      }
      for ( int j = 0; do_alias && j < i; j++ )           {
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d_%02d",host_name.c_str(),task_name.c_str(),i,j);
        status = ::tan_get_address_by_name(buff, &addr);
        ++inquire;
        if ( status == TAN_SS_SUCCESS )  {
          if ( !quiet ) ::printf("     address_by_name: %s Port:%04X  status:%d  [%s]\n",
				 buff+host_len, addr.sin_port, status, ::strerror(errno));
        }
        else   {
          ::printf("     FAILED: address_by_name(%s): %d [%s]\n", 
		   buff+host_len, status, ::strerror(errno));
        }
      }
    }
#if 0
    for ( int i = 0; i < MAXTASKS; i++ )  {
      for ( int j = 0; do_alias && j < i; j++ )           {
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d_%02d",host_name.c_str(), task_name.c_str(),i,j);
        status = ::tan_remove_alias(buff);
        if ( status != TAN_SS_SUCCESS )  {
          ::printf("     FAILED: tan_remove_alias(%s): %d [%s]\n", 
		   host_name.c_str()+host_len, status, ::strerror(errno));
        }
      }
    }
#endif
    for ( int i = 0; i < MAXTASKS; i++ )  {
      ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
      status = ::tan_deallocate_port_number ( buff );
      dealloc++;
      if ( !quiet || status != TAN_SS_SUCCESS ) {
        ::printf("deallocatePort(%s) Port:%04X  status:%d [%s]\n", buff+host_len, ports[i], status, strerror(errno));
      }
      if ( dealloc%50 == 0 )  {
        ::printf("->%-4ld sec<-  Allocations:%-6d  Aliases:%-8d  Inquire:%-6d Deallocations:%-6d\n",
                 time(0) - start, alloc, alias, inquire, dealloc);
      }
    }
    ::lib_rtl_sleep(10);
  }  while ( status == TAN_SS_SUCCESS && continuous );
  delete [] ports;
  return 0x1;
}
