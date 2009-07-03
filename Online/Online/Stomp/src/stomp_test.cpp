#include "Stomp/StompSensor.h"
#include <cstdlib>
#include "CPP/Event.h"
#include "CPP/Interactor.h"

using namespace Stomp;

namespace {
  struct StompHandler : public Interactor {
    StompHandler() {}
    virtual ~StompHandler() {}
    virtual void handle(const Event& ev)  {
      if ( ev.eventtype == NetEvent )  {
        const char* cmd  = (char*)ev.data;
        const char* chan = ev.device;
        const char* body = ev.buffer_ptr;
        ::fprintf(stdout, "Response: %s, %s  %s",cmd,chan,body);
        if ( strncmp(body,"quit",4)==0 ) {
          StompSensor::Message* m = (StompSensor::Message*)ev.message;
          m->sensor->remove(this,0);
          ::fprintf(stdout, "Sending Disconnect.");
          int rc = m->sensor->disconnect();
          ::fprintf(stdout, "OK\n");
          ::fprintf(stdout, "Disconnecting...");
          rc = m->sensor->disconnectServer();
          ::fprintf(stdout, "OK\n");   
          m->sensor->stop();
        }
      }
    }
  };
}

extern "C" int test_stomp_sensor(int argc, char *argv[])    {
  setbuf(stdout, NULL);
  StompSensor sensor("myStomp","localhost", 61613);
  StompHandler handler;
  //_asm int 3
  sensor.start();
  sensor.add(&handler,"/topic/home");
  sensor.run();
  fprintf(stdout, "All stopped. OK\n");
  return 0;
}

extern "C" int test_stomp(int argc, char *argv[])    {
  int rc;
   setbuf(stdout, NULL);
   StompSensor sensor("myStomp","localhost", 61613);
   fprintf(stdout, "Connecting......");
   rc = sensor.connectServer();
   fprintf(stdout, "OK\nSending connect message.");
   rc = sensor.connect();
   fprintf(stdout, "OK\nSending Message.");
   for(int i=0; i<=100;++i)  {
     char text[255], buf[64];
     memset(buf,'a'+i%('z'-'a'),sizeof(buf));
     buf[63]=0;
     sprintf(text,"This is the message  No.  %d %s\n",i,buf);
     sensor.send("/topic/home", text, strlen(text));
     ::lib_rtl_sleep(20);
   }
   fprintf(stdout, "OK\nSending Subscribe.");
   rc = sensor.subscribe("/topic/home");
   fprintf(stdout, "OK\nReading Messages.");
   //_asm int 3
   while(1)  {
     apr_pool_t *rpool = 0;
     StompSensor::Frame *frame = 0;
     rc = sensor.read(frame, rpool);
     fprintf(stdout, "Response: %s, %s", frame->command, frame->body);
     if ( strncmp(frame->body,"quit",4)==0 ) break;
     sensor.destroyPool(rpool);	   
   }     
   fprintf(stdout, "OK\nSending Disconnect.");
   rc = sensor.disconnect();
   fprintf(stdout, "OK\nDisconnecting...");
   rc = sensor.disconnectServer();
   fprintf(stdout, "OK\n");   
   return 0;
}
