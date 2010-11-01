#include "CPP/Event.h"
#include "CPP/Interactor.h"
#include "ActiveMQ/Log.h"
#include "ActiveMQ/ActiveMQSensor.h"
#include <cstdlib>
#include <cstring>
#include <memory>

namespace ActiveMQ { class AMQSensor; }

using namespace std;
using namespace ActiveMQ;

namespace {
  struct AMQHandler : public Interactor {
    AMQHandler() {}
    virtual ~AMQHandler() {}
    virtual void handle(const Event& ev)  {
      if ( ev.eventtype == NetEvent )  {
        const char* chan = ev.device;
	const char* body = ev.buffer_ptr;
	ActiveMQSensor*  sens = (ActiveMQSensor*)ev.data;
        ::fprintf(stdout, "Response: %s  %s\n",chan,body);
        if ( strstr(body,"quit") != 0 )  {
          sens->remove(this,0);
          ::fprintf(stdout, "Sending Disconnect.");
          int rc = sens->disconnect();
          ::fprintf(stdout, "OK\n");
          ::fprintf(stdout, "Disconnecting...");
          rc = sens->disconnectServer();
          ::fprintf(stdout, "OK\n");   
          sens->stop();
        }
      }
    }
  };
}

static void help()  {
  ::printf("  test_amq_sensor or test_amq -option [-option]\n" \
           "       -c[onnection]=<name>  AMQ connection string: failover:(tcp://ecs03.lbdaq.cern.ch:61616)\n" \
           "       -t[opic]=<name>       Topic name.\n" \
           "       -d[ebug]              Wait to attach debugger.\n" \
	   "       -p[rint]=<value       Set print level.\n");
}

extern "C" int test_amq_sensor(int argc, char** argv)    {
  int prt = LIB_RTL_DEBUG;
  RTL::CLI cli(argc,argv,help);
  string con   = "failover:(tcp://ecs03.lbdaq.cern.ch:61616)";
  string topic = "lbWeb.TEST";
  cli.getopt("connection",1,con);
  cli.getopt("topic",1, topic);
  cli.getopt("print",1, prt);
  setbuf(stdout, NULL);
  install_printer(prt);
  if ( cli.getopt("debug",1) != 0 )  {
#ifdef WIN32
    _asm int 3
#else
    fprintf(stdout, "You got 10 seconds to connect a debugger...... pid: %d\n",::lib_rtl_pid());
    ::lib_rtl_sleep(10000);
#endif
  }
  std::auto_ptr<ActiveMQSensor> sensor(createSensor<AMQSensor>("myAMQ",con));
  AMQHandler handler;
  sensor->start();
  sensor->add(&handler,(void*)topic.c_str());
  sensor->run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}

extern "C" int test_amq(int argc, char** argv)    {
  int rc, prt = LIB_RTL_DEBUG;
  RTL::CLI cli(argc,argv,help);
  string con = "failover:(tcp://ecs03.lbdaq.cern.ch:61616)";
  string topic = "lbWeb.TEST";
  cli.getopt("connection",1,con);
  cli.getopt("topic",1, topic);
  cli.getopt("print",1, prt);

  install_printer(prt);
  setbuf(stdout, NULL);
  if ( cli.getopt("debug",1) != 0 )  {
#ifdef WIN32
    _asm int 3
#else
    ::lib_rtl_output(LIB_RTL_INFO, "You got 10 seconds to connect a debugger...... pid: %d",::lib_rtl_pid());
    ::lib_rtl_sleep(10000);
#endif
  }
  std::auto_ptr<ActiveMQSensor> sensor(createSensor<AMQSensor>("myAMQ",con));
  ::lib_rtl_output(LIB_RTL_INFO, "Connected");
  ::lib_rtl_output(LIB_RTL_INFO, "Sending Messages.");
  for(int i=0; i<=20;++i)  {
    char text[255], buf[12];
    memset(buf,'a'+i%('z'-'a'),sizeof(buf));
    buf[sizeof(buf)-1]=0;
    sprintf(text,"<message id=\"%s\" vid=\"%d\"><![CDATA[Message No.%d %s]]></message>",topic.c_str(),i,i,buf);
    //sprintf(text,"Message No.%d %s",i,buf);
    //sprintf(text,"<message><![CDATA[Message No.%d %s]]></message>",i,buf);
    if ( 1==sensor->send(topic, text) )
      ::lib_rtl_output(LIB_RTL_INFO,"Sent message:%s",text);
    else
      ::lib_rtl_output(LIB_RTL_INFO,"FAILED to send message:%s",text);
    ::lib_rtl_sleep(200);
  }
  ::lib_rtl_output(LIB_RTL_INFO, "Sending Disconnect.");
  rc = sensor->disconnect();
  ::lib_rtl_output(LIB_RTL_INFO, "Disconnecting...");
  rc = sensor->disconnectServer();
  ::lib_rtl_output(LIB_RTL_INFO, "Done.\n");   
  return 0;
}
