#include <ctime>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include "TAN/TanDB.h"
#include "TAN/TanInterface.h"
#include "AMS/amsdef.h"

#ifdef _VMS
#define lib_signal(x) lib$signal(x)
#else
#define lib_signal(x) {printf("%s\n",strerror(x)); }
#endif

static void help()  {
  printf("Usage : inquire -opt <opts>\n");
  printf("        -t           supply task name; default:MYTASK\n");
  printf("        -n           supply host name; default:<localhost>\n");
  printf("        -c           contious mode \n");
  printf("        -q           quiet mode    \n");
  printf("        -?, -h       get this help \n");
  exit(0);
}

extern "C" int rtl_tan_alloc_test ( int argc, char* argv[] )  {
  // ----------------------------------------------------------------------------
  //
  //                                      M.Frank
  // ----------------------------------------------------------------------------
  RTL::CLI cli(argc,argv,help);
  TanInterface& interf = TanInterface::instance();
  int status = 1;
  bool continuous = false, quiet = false;
  char buff[64], host[64];
  std::string task_name = "MYTASK", host_name;
  ::lib_rtl_get_node_name(host,sizeof(host));
  //tan_host_name(host,sizeof(host));

  cli.getopt("node",1,host_name=host);
  cli.getopt("task",1,task_name);
  continuous = cli.getopt("continuous",1) != 0;
  quiet = cli.getopt("quiet",1) != 0;

#if defined(_OSK)
#define MAXTASKS 6
#else
#define MAXTASKS 20
#endif

  if ( interf.Status() == TAN_SS_SUCCESS ) {
    int alloc = 0, dealloc = 0, alias = 0;
    time_t start = ::time(0);
    do  {
      NetworkChannel::Port ports[MAXTASKS];
      for ( int i = 0; i < MAXTASKS; i++ )         {
        ports[i] = 0;
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
        status = ::tan_allocate_port_number (buff, &ports[i]);
        alloc++;
        if ( status == TAN_SS_SUCCESS )  {
          if ( !quiet ) ::printf("allocatePort: %s Port:%04X  status:%d\n", buff, ports[i], status);
        }
        else   {
          ::printf("allocatePort: %d \n", status);
          lib_signal(status);
        }
        for ( int j = 0; j < i; j++ )           {
          ::snprintf(buff,sizeof(buff),"%s::%s_%02d_%02d",host_name.c_str(),task_name.c_str(),i,j);
          status = ::tan_declare_alias(buff);
          alias++;
          if ( !quiet || status != TAN_SS_SUCCESS ) {
            ::printf("     DeclareAlias: %s   status:%d %08X errno:%d\n", 
                     buff, status, status, errno);
            if ( status != TAN_SS_SUCCESS ) lib_signal(status);
          }
        }
        if ( !quiet)  {
          ::printf("Hit any key to continue\n");
          ::getchar();
        }
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
        status = ::tan_deallocate_port_number ( buff );
#ifdef _OSK
        //       tsleep(5);
#endif
        dealloc++;
        if ( !quiet || status != TAN_SS_SUCCESS ) {
          ::printf("DeallocatePort: %s Port:%04X  status:%d\n", buff, ports[i], status);
          if ( status != TAN_SS_SUCCESS ) lib_signal(status);
        }
        if ( dealloc%50 == 0 )  {
          ::printf("->%-4ld sec<-  Allocations:%-6d  Aliases:%-8d  Deallocations:%-6d\n",
                   time(0) - start, alloc, alias, dealloc);
        }
      }
    }  while ( status == TAN_SS_SUCCESS && continuous );
  }
  else  {
    ::printf("Error! Status: 0x%08X\n",interf.Status());
  }
  return 0x1;
}
