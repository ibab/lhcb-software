#include "Stomp/StompSensor.h"
#include "Stomp/DeliveryHandler.h"
#include "Stomp/SubscribeHandler.h"
#include "RTL/rtl.h"
using namespace Stomp;
using namespace std;

static void help()  {
  ::printf("  stomp_service -option [-option]\n" \
           "       -n[ode]=<name>        Hostname where Stomp sevice is running.\n" \
           "       -p[ort]=<number>      Port of the Stomp service (default:61613).\n" \
           "       -s[ervice]=<name>     Stomp service name (unused).\n" \
           "       -t[opic]=<name>       Topic name to listen for additional services.\n" \
           "       -i[ni]=<name>         Initialization file for default services.\n" \
           "       -d[ebug]              Start debugger.\n" \
	   "       -p[rint]=<value       Set print level.\n");
}

static size_t stomp_rtl_print(void* param,int lvl,const char* fmt,va_list args) {
  if ( lvl >= long(param) ) {
    size_t result;
    string format = fmt;
    format += "\n";
    result = ::vfprintf(stdout, format.c_str(), args);
    ::fflush(stdout);
    return result;
  }
  return 1;
}

extern "C" int stomp_service(int argc, char *argv[])    {
  int port = 61613, prt = LIB_RTL_WARNING;
  string ini     = "";
  string topic   = "/topic/home";
  string host    = "localhost";
  string service = "MyStomp";
  RTL::CLI cli(argc,argv,help);

  cli.getopt("ini", 2, ini);
  cli.getopt("port", 2, port);
  cli.getopt("node", 2, host);
  cli.getopt("service", 2, service);
  cli.getopt("topic", 2, topic);
  cli.getopt("print", 2, prt);
  if ( cli.getopt("debug",1) != 0 )  {
    ::lib_rtl_start_debugger();
  }
  ::lib_rtl_install_printer(stomp_rtl_print,(void*)prt);

  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Listening to stomp port: %d on host %s",
		   port,host.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Setting incoming subscribe topic to:%s.",
		   topic.c_str());

  StompSensor sensor(service,host, port);
  DeliveryHandler  delivery(&sensor);
  SubscribeHandler handler(ini,&delivery);

  sensor.start();
  sensor.add(&handler,(void*)topic.c_str());
  sensor.run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}
