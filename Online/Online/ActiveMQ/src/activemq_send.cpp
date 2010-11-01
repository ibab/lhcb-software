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
    bool have_msg;
    AMQHandler() : have_msg(false) {}
    virtual ~AMQHandler() {}
    virtual void handle(const Event& ev)  {
      if ( ev.eventtype == NetEvent )  {
        const char* chan = ev.device;
	const char* body = ev.buffer_ptr;
        ::lib_rtl_output(LIB_RTL_ALWAYS,"Got answer:%s -> %s\n",chan,body);
	have_msg = true;
      }
    }
  };
}

static void help()  {
  ::printf("  amq_send -option [-option]\n"
           "       -c[onnection]=<name>  AMQ connection string: failover:(tcp://ecs03.lbdaq.cern.ch:61616)\n"
           "       -t[opic]=<name>       Topic name.\n"
           "       -d[ebug]              Wait to attach debugger.\n"
	   "       -p[rint]=<value       Set print level.\n"
	   "       -m[sg]=<value         Message to be sent to web clients  \n\n  Examples:\n"
           "  amq_send -topic=clientCommands -msg=\"lhc:reload\" -connection=failover:\\(tcp://ecs03.lbdaq.cern.ch:61616\\)  \n"
           "  amq_send -topic=clientCommands -msg=\"allPages:reload\" -connection=failover:\\(tcp://ecs03.lbdaq.cern.ch:61616\\)  \n"
           "  amq_send -topic=clientCommands -msg=\"lhc:url:http://lhcb.cern.ch\" -connection=failover:\\(tcp://ecs03.lbdaq.cern.ch:61616\\)  \n"

	   );
}

extern "C" int amq_send(int argc, char** argv)    {
  int prt = LIB_RTL_INFO;
  RTL::CLI cli(argc,argv,help);
  string con = "failover:(tcp://localhost:61616)";
  string topic, msg;
  cli.getopt("connection",1,con);
  cli.getopt("topic",1, topic);
  cli.getopt("print",1, prt);
  cli.getopt("msg",  1, msg);

  install_printer(prt);
  setbuf(stdout, NULL);
  if ( msg.empty() ) {
    ::lib_rtl_output(LIB_RTL_FATAL,"Empty messages are not accepted!");
    return 0;
  }
  if (topic.find("/topic/") == 0 || topic.find("/queue/") == 0) 
    topic = topic.substr(7);
  if ( topic.empty() ) {
    ::lib_rtl_output(LIB_RTL_FATAL,"Empty topic names are not accepted!");
    return 0;
  }
  if ( cli.getopt("debug",1) != 0 )  {
#ifdef WIN32
    _asm int 3
#else
    ::lib_rtl_output(LIB_RTL_INFO, "You got 10 seconds to connect a debugger...... pid: %d",::lib_rtl_pid());
    ::lib_rtl_sleep(10000);
#endif
  }

  std::auto_ptr<ActiveMQSensor> sensor(createSensor<AMQSensor>("myAMQ",con));
  AMQHandler handler;
  sensor->start();
  sensor->add(&handler,(void*)topic.c_str());

  ::lib_rtl_output(LIB_RTL_ALWAYS,"Sending to topic:%s",topic.c_str());
  if ( 1==sensor->send(topic, msg.c_str()) )
    ::lib_rtl_output(LIB_RTL_ALWAYS,"Sent message:%s",msg.c_str());
  else
    ::lib_rtl_output(LIB_RTL_FATAL,"FAILED to send message:%s",msg.c_str());

  //char c;
  //cout << "Type any key<CR> to exit" << endl;
  //fscanf(stdin,"%c",&c);
  while(!handler.have_msg) ::lib_rtl_sleep(10);
  ::lib_rtl_sleep(100);
  ::lib_rtl_output(LIB_RTL_INFO,"++ Sending Disconnect to topic:%s",topic.c_str());
  sensor->disconnect();
  ::lib_rtl_output(LIB_RTL_INFO,"++ Disconnecting from server:%s",con.c_str());
  sensor->disconnectServer();
  ::lib_rtl_output(LIB_RTL_INFO,"++ Shutdown.");
  sensor->remove(&handler,0);
  sensor->stop();
  ::lib_rtl_output(LIB_RTL_INFO,"++ Done");
  return 0;
}
