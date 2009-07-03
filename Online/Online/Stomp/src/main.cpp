#include "Stomp/StompSensor.h"
#include "Stomp/DeliveryHandler.h"
#include "Stomp/SubscribeHandler.h"
#include "RTL/rtl.h"
#include <iostream>
using namespace Stomp;
using namespace std;

static void help()  {
  cout << "  stomp_service -option [-option]" << endl
       << "       -h[ost]=<name>        Hostname where Stomp sevice is running." << endl
       << "       -p[ort]=<number>      Port of the Stomp service (default:61613)." << endl
       << "       -s[ervice]=<name>     Stomp service name (unused)." << endl
       << "       -t[opic]=<name>       Topic name to listen for additional services." << endl
       << "       -i[ni]=<name>         Initialization file for default services." << endl
       << "       -d[ebug]              Start debugger." << endl
       << endl;
}

extern "C" int stomp_service(int argc, char *argv[])    {
  int port = 61613;
  string ini = "";
  string topic = "/Services";
  string host = "localhost";
  string service = "MyStomp";
  RTL::CLI cli(argc,argv,help);

  cli.getopt("ini", 2, ini);
  cli.getopt("port", 2, port);
  cli.getopt("host", 2, host);
  cli.getopt("service", 2, service);
  cli.getopt("topic", 2, topic);
  if ( cli.getopt("debug",1) != 0 )  {
    ::lib_rtl_start_debugger();
  }
  StompSensor sensor(service,host, port);
  DeliveryHandler  delivery(&sensor);
  SubscribeHandler handler(ini,&delivery);

  sensor.start();
  sensor.add(&handler,"/topic/home");
  sensor.run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}
