#include "ActiveMQ/Log.h"
#include "ActiveMQ/ActiveMQSensor.h"
#include "ActiveMQ/ActiveMQHandler.h"
#include "ActiveMQ/SubscribeHandler.h"
#include "RTL/rtl.h"
#include <memory>

namespace ActiveMQ { class AMQSensor; }
using namespace ActiveMQ;
using namespace std;

static void help()  {
  ::printf("  amqp_service -option [-option]\n" \
           "       -n[ode]=<name>        Hostname where ActiveMQ sevice is running.\n" \
           "       -s[ervice]=<name>     ActiveMQ service name (unused).\n" \
           "       -t[opic]=<name>       Topic name to listen for additional services.\n" \
           "       -i[ni]=<name>         Initialization file for default services.\n" \
           "       -d[ebug]              Start debugger.\n" \
	   "       -p[rint]=<value       Set print level.\n" \
	   "       -o[output]=<value>    Protocol type SIMPLE=0, XML=1 (Ajax).\n" \
	   "\n" \
	   "    Stomp    protocol:  failover:(tcp://localhost:61613?wireFormat=stomp)\n" \
	   "    Openwire protocol:  failover:(tcp://localhost:61616?wireFormat=openwire)\n" \
	   "                 dto.:  failover:(tcp://localhost:61616)\n" \
	   );
}

extern "C" int activemq_service(int argc, char *argv[])    {
  int prt = LIB_RTL_WARNING, proto=ActiveMQHandler::PROTO_SIMPLE;
  string ini     = "";
  string topic   = "status";
  string conn    = "failover:(tcp://localhost:61616)";
  string service = "MyActiveMQ";
  RTL::CLI cli(argc,argv,help);

  cli.getopt("ini",        2, ini);
  cli.getopt("connection", 2, conn);
  cli.getopt("service",    2, service);
  cli.getopt("topic",      2, topic);
  cli.getopt("print",      2, prt);
  cli.getopt("output",     2, proto);
  if ( cli.getopt("debug",1) != 0 )  {
    ::lib_rtl_start_debugger();
  }
  if (topic.find("/topic/") == 0 || topic.find("/queue/") == 0) 
    topic = topic.substr(7);
  install_printer(prt);
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Listening to connection: %s",conn.c_str());
  ::lib_rtl_output(LIB_RTL_ALWAYS,"+++  Incoming topic is:%s",topic.c_str());

  std::auto_ptr<ActiveMQSensor> sensor(createSensor<AMQSensor>(service,conn));
  ActiveMQHandler delivery(sensor.get(), proto);
  SubscribeHandler handler(ini,&delivery);

  sensor->start();
  sensor->add(&handler,(void*)topic.c_str());
  sensor->run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}
