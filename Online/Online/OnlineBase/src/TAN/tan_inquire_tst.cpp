// ----------------------------------------------------------------------------
//
//                                      M.Frank
// ----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "RTL/rtl.h"
#include "AMS/amsdef.h"
#include "TAN/TanInterface.h"

#define MAXTASKS 20
#ifdef _VMS
#define lib_signal(x) lib$signal(x)
#else
#define lib_signal(x) {printf("%s\n",strerror(x)); }
#endif
static void help()  {
  printf("Usage : inquire -opt <opts>\n");
  printf("        -c           contious mode\n");
  printf("        -t           supply task name; default:MYTASK\n");
  printf("        -n           supply host name; default:<localhost>\n");
  printf("        -q           quiet mode(statistics only\n");
  printf("        -?, -h       get this help\n");
  exit(0);
}

extern "C" int rtl_tan_inquire_test ( int argc, char* argv[] )  {
  RTL::CLI cli(argc,argv,help);
  TanInterface& interf = TanInterface::instance();
  int status, num_inq = 0, tot_inq = 0, succ = 0, fail = 0, notfnd = 0;
#ifdef _WIN32
  int inc = 50;
#else
  int inc = 500;
#endif
  std::string task_name = "MYTASK", host_name;
  bool continuous = false, quiet = false;
  char buff[64], host[32];
  sockaddr_in addr;
  ::lib_rtl_get_node_name(host,sizeof(host));
  tan_host_name(host,sizeof(host));
  cli.getopt("node",1,host_name=host);
  cli.getopt("task",1,task_name);
  continuous = cli.getopt("continuous",1) != 0;
  quiet = cli.getopt("quiet",1) != 0;

#define _PRINT  succ++; if ( !quiet ) printf
#define _PRINTERROR(buff,status)  {                           \
    if ( status == AMS_TASKNOTFOUND ) notfnd++;               \
    if ( !quiet || status != AMS_TASKNOTFOUND )  {            \
      ::printf("GetAddressByName(%-24s) %4d ", buff, status); \
      lib_signal(status);                                     \
    } fail++;                                                 \
  }

  if ( interf.Status() == TAN_SS_SUCCESS ) {
    /// Check non-exisiting task
    ::snprintf(buff,sizeof(buff),"%s::IDIOTIC",host_name.c_str());
    status = ::tan_get_address_by_name(buff,&addr);
    _PRINTERROR(buff,status);
    time_t start = ::time(0);
    int nalias = 0;
    while ( 1 )    {
      if ( num_inq > inc )  {
        tot_inq += num_inq;
        num_inq = 0;
        ::printf("->%-4ld sec<-  Queries:%6d Aliases:%6d  Success:%-4d TaskNotFound:%-4d Failure:%-4d.\n",
                 time(0) - start, tot_inq, nalias, succ, notfnd, fail);
      }
      for ( int i = 0; i < MAXTASKS; i++ )         {
        if ( (i % 10)==0 && !quiet ) status = tan_dump_dbase ( host_name.c_str() );
        for ( int j = 0; j < i; j++ )           {
          // Now check the aliases
          ::snprintf(buff,sizeof(buff),"%s::%s_%02d_%02d",host_name.c_str(),task_name.c_str(),i,j);
          status = ::tan_get_address_by_name (buff,&addr);
          num_inq++;
          if ( status == AMS_SUCCESS ) {
            _PRINT ("GetAddressByName[%d]: %s Port:%04X Fam:%1X Saddr:%08X (%s)\n",
                    num_inq, buff, addr.sin_port, addr.sin_family, addr.sin_addr.s_addr,
                    inet_ntoa(addr.sin_addr) );
          }
          else {
            nalias++;
            _PRINTERROR(buff,status);
          }
        }
        ::snprintf(buff,sizeof(buff),"%s::%s_%02d",host_name.c_str(),task_name.c_str(),i);
        status = ::tan_get_address_by_name (buff,&addr);
        num_inq++;
        if ( status == AMS_SUCCESS ) {
          _PRINT ("GetAddressByName[%d]: %s Port:%04X Fam:%1X Saddr:%08X (%s)\n",
                  num_inq, buff, addr.sin_port, addr.sin_family, addr.sin_addr.s_addr,
                  inet_ntoa(addr.sin_addr) );
        }
        else {
          nalias++;
          _PRINTERROR(buff,status);
        }
        if ( !continuous ) return 0x1;   // Just stop if non continuous
      }
    }
  }
  else  {
    _PRINTERROR("Error! Status:", interf.Status());
  }
  return 0x1;
}
