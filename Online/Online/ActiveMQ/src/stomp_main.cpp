#include "ActiveMQ/ActiveMQSensor.h"
#include "ActiveMQ/ActiveMQHandler.h"
#include "ActiveMQ/SubscribeHandler.h"
#include "ActiveMQ/Log.h"
#include "RTL/rtl.h"
#include <memory>
namespace ActiveMQ { class StompSensor; }
using namespace ActiveMQ;
using namespace std;

static void help()  {
  ::printf("  stomp_service -option [-option]\n" \
           "       -n[ode]=<name>        Hostname where Stomp sevice is running.\n" \
           "       -p[ort]=<number>      Port of the Stomp service (default:61613).\n" \
           "       -s[ervice]=<name>     Stomp service name (unused).\n" \
           "       -t[opic]=<name>       Topic name to listen for additional services.\n" \
           "       -i[ni]=<name>         Initialization file for default services.\n" \
           "       -d[ebug]              Start debugger.\n" \
	   "       -o[output]=<value>    Protocol type SIMPLE=0, XML=1 (Ajax).\n" \
	   "       -p[rint]=<value       Set print level.\n");
}

extern "C" int stomp_service(int argc, char *argv[])    {
  int port = 61613, prt = LIB_RTL_WARNING, proto=ActiveMQHandler::PROTO_SIMPLE;
  string ini     = "";
  string topic   = "/topic/home";
  string host    = "localhost";
  string service = "MyStomp";
  RTL::CLI cli(argc,argv,help);

  cli.getopt("ini",     2, ini);
  cli.getopt("port",    2, port);
  cli.getopt("node",    2, host);
  cli.getopt("service", 2, service);
  cli.getopt("topic",   2, topic);
  cli.getopt("print",   2, prt);
  cli.getopt("output",  2, proto);
  if ( cli.getopt("debug",1) != 0 )  {
    ::lib_rtl_start_debugger();
  }
  install_printer(prt);

  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Listening to stomp port: %d on host %s",
		   port,host.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Setting incoming subscribe topic to:%s.",
		   topic.c_str());

  auto_ptr<ActiveMQSensor> sensor(createSensor<StompSensor>(service,host,port));
  ActiveMQHandler  delivery(sensor.get(), proto);
  SubscribeHandler handler(ini,&delivery);

  sensor->start();
  sensor->add(&handler,(void*)topic.c_str());
  sensor->run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}
