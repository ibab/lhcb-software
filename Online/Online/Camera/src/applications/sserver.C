#ifdef _WIN32
#include <stdio.h>
int main(){
  printf("I will not work at all under windows\nNot one bit...\n");
}
#endif


#ifndef _WIN32

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string>

#include <signal.h>
#include <iostream>
#include <sstream>

#include "nio.h"
#include "mapconfig.h"
#include "getnsave.h"

mapconfig mapc;//("etc/camserv.conf");
const int MAXTH=10;
int BUSYTHREAD;
outstack OutStack;

std::string CONFIG_STR()
{
  std::stringstream s;
  s <<"SERVER/0/status: "<<mapc.get("ID")<<" "<<mapc.get("IP")
    << mapc.get("inport")<< " "<<mapc.get("outport")<< " "
    <<BUSYTHREAD<<"/"<<MAXTH;
  return s.str();
}

class blockingIO : public nio
{
protected:
  int rdyn;
  int sock;

public:

  blockingIO(int s){sock=s;rdyn=0;}

  void reset(int csock){ sock = csock; rdyn=0;}

  int rdend(){return rdyn;}
  int wrend(){return 0;}
  int rd(char*,int);
  int wr(const char*,int);
  void shutwr();
  void shutrd();
  void shut_close();
};


int blockingIO::rd(char * buf,int n){

  if (rdyn!=0) return -1;

  int r = read(sock,buf,n);

  //buf[r] = '\0';
  //printf("rd: %d %s\n",r,buf);

  if (r==0) rdyn= 1;
  if (r<0 ) rdyn=-1;

  return r;

}

int blockingIO::wr(const char * buf,int n){
  return write(sock,buf,n);
}

void blockingIO::shutwr(){
  shutdown(sock,SHUT_WR);
}

void blockingIO::shutrd(){
  shutdown(sock,SHUT_RD);
}

void blockingIO::shut_close(){
  shutdown(sock,SHUT_RDWR);
  close(sock);
}

class NONblockingIO:public blockingIO{

  fd_set rfds;
  struct timeval tv;

public:

  NONblockingIO(int s):blockingIO(s)
  {}

  int rd(char*,int);

};


int NONblockingIO::rd(char * buf,int n){


  if (rdyn!=0) return -1;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  int r(0);



  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);

  int ret = select(sock+1,&rfds,NULL,NULL,NULL);


  //printf("select = %d\n",ret);

  if (ret == 0) {return -2;}
  if (ret == -1){perror("select");return -1;}
  if (ret > 0)
    r = read(sock,buf,n);

  buf[r] = '\0';
  //  printf("rd: %d %s\n",r,buf);

  if (r==0) rdyn= 1;
  if (r<0 ) rdyn=-1;

  return r;

}
#include <unistd.h>
#include <fcntl.h>

class server{
protected:
  int setNonblocking(int fd)
  {
    int flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */

    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
      flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);

#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
  }

  fd_set rfds;
  struct timeval tv;
  int usecs;
  int secs;
public:
  void settimeout(int s,int us){
    usecs = us;
    secs =s;
  }
  int sock;
  void wait();
  void nowait();
  unsigned short int port;

  struct sockaddr_in AdrMySock;
  struct sockaddr_in AdrHisSock;
  server(short unsigned int p);

  int Listen();

  virtual int Go(int csock);
  virtual int TimeOut(){return 0;}

  int End();

};

void server::nowait(){
  int flag = 1;
  int result = setsockopt(sock,            /* socket affected */
                          IPPROTO_TCP,     /* set option at TCP level */
                          TCP_NODELAY,     /* name of option */
                          (char *) &flag,  /* the cast is historical
                                              cruft */
                          sizeof(int));    /* length of option value */
  if (result < 0)
    //... handle the error ...
    perror("Could not set TCP_NODELAY");
}

void server::wait(){
  int flag = 0;
  int result = setsockopt(sock,            /* socket affected */
                          IPPROTO_TCP,     /* set option at TCP level */
                          TCP_NODELAY,     /* name of option */
                          (char *) &flag,  /* the cast is historical
                                              cruft */
                          sizeof(int));    /* length of option value */
  if (result < 0)
    //... handle the error ...
    perror("Could not unset TCP_NODELAY");
}

server::server(unsigned short int p){
  // ignore SIGPIPE signal?
  port = p;
  signal (SIGPIPE,SIG_IGN);
  secs =0;
  usecs=0;
  //nowait();
}

int server::Go(int csock){
  // printf("server::Go()\n");
  write(csock,"Go away\n",8);

  shutdown(csock , SHUT_RDWR);
  close(csock);

  return 0;
}


int server::Listen(){

  sock = socket(PF_INET, SOCK_STREAM,0);
  if (sock<0) {
    perror("Cannot make socket:");
    return -1;
  }
  else{
    printf("Socket: %d\n",sock);
  }

  AdrMySock.sin_family = AF_INET;
  AdrMySock.sin_addr.s_addr = INADDR_ANY;
  AdrMySock.sin_port = htons(port);
  int b = bind(sock, (const sockaddr*)&AdrMySock, sizeof(AdrMySock));
  if (b<0) {
    std::cerr << "AF_INET, INADDR_ANY, port "<<port<<std::endl;
    perror("Cannot bind:");
    return -1;
  }
  int r = listen(sock, 100);
  if (r<0) {
    perror("Cannot listen:");
    return -1;
  }
  else{
    printf("Done listening: %d\n",r);
  }

  socklen_t len = sizeof (AdrHisSock);

  while (1) {

    int ret=1;

    if (secs+usecs>0){

      FD_ZERO(&rfds);
      FD_SET(sock, &rfds);

      tv.tv_sec = secs;
      tv.tv_usec = usecs;

      ret = select(sock+1,&rfds,NULL,NULL,&tv);
    }
    if (ret == 1){

      int csock = accept(sock, (sockaddr*)&AdrHisSock, &len);
      //close(csock);
      //printf("accept\n");
      //continue;
      if (csock<0){
        printf("%d %d\n",sock,csock);
        perror("Cannot accept:");
        return -1;
      }

      Go(csock);
    }
    if (ret<0){
      perror("select");
    }
    if (ret==0){

      TimeOut();
      //      printf("Still alive: timeout %d,%d\n",secs,usecs);
      //  OutStack.all_out();
    }

  }
  return 0;
}


class worker{

public:

  pthread_t thread;
  pthread_mutex_t  mtx;
  int csock;
  int ready;

  int unique;

  worker(int n){ready =0;unique = n;}
  worker(){ready =0;}

};



extern "C" {
  void * incomingthread(void * vgns)
  {
    BUSYTHREAD++;
    worker * wrk = (worker *)vgns;
    NONblockingIO * bio = new NONblockingIO(wrk->csock);
    getnsave *gns = new getnsave( bio,wrk->unique,mapc );
    gns->setoutstack(&OutStack);
    while (true)
    {

      //  delete gns;
      //gns = new getnsave( bio );

      BUSYTHREAD--;


      //      printf("wait ... %d\n",wrk->unique);

      wrk->ready=1;
      pthread_mutex_lock(&(wrk->mtx));
      //printf("self locked\n");


      bio->reset(wrk->csock);
      gns->reset();
      int flag = 1;
      int result = setsockopt(wrk->csock,            /* socket affected */
                              IPPROTO_TCP,     /* set option at TCP level */
                              TCP_NODELAY,     /* name of option */
                              (char *) &flag,  /* the cast is historical
                                                  cruft */
                              sizeof(int));    /* length of option value */
      if (result < 0)
        //... handle the error ...
        perror("Could not set TCP_NODELAY");


      // blockingIO * bio = new blockingIO(wrk->csock);
      // getnsave *gns = new getnsave( bio );
      // do sth

      if (bio->wr("GO",2)==2){
        // std::cout << "sent GO"<< std::endl;
        bio->shutwr();
        gns->start();

      }
      gns->end();
      gns->reset();
      //   std::cout << "End of Connection "<< wrk->unique<<std::endl;

    }
    return NULL; // CRJ - No idea what to return here, but needed for gcc 4.6
  }
}

#include <pthread.h>
#include <sys/types.h>



class WarnIn: public server{
  int cnt;
  worker wrk[MAXTH];
  int which ;



public:
  WarnIn(int p):server (p){
    cnt =0;
    which =0;
    pthread_attr_t pthread_custom_attr;
    pthread_attr_init(&pthread_custom_attr);


    for (int i=0;i<MAXTH;++i){
      pthread_mutex_init(&(wrk[i].mtx),NULL);
      pthread_mutex_lock(&(wrk[i].mtx));
      wrk[i].unique = i;

      pthread_create(&(wrk[i].thread), &pthread_custom_attr, incomingthread, (void*) &wrk[i]);

    }
  }
  int Go(int csock);
  int TimeOut(){
    //    printf("busy: %d\n",BUSYTHREAD);
    return 0;
  }
};


int WarnIn::Go(int csock){

  // printf("New Connection ...\n");
  if (BUSYTHREAD==MAXTH)  {
    write(csock,"BZ",2);
    close(csock);
    //    fprintf(stderr,"All threads busy.\n");
    //which = (which+1) % MAXTH;
    return -1;
  }

  while (wrk[which].ready !=1) {

    //close(csock);
    //printf("%d busy\n",which);
    which = (which+1) % MAXTH;
  }

  //  printf("found free\n");
  BUSYTHREAD++;
  wrk[which].ready =0;
  wrk[which].csock = csock;
  //printf ("allocated Thread Nr. %d\n",which);
  pthread_mutex_unlock(&(wrk[which].mtx));
  //printf("unlocked\n");

  which = (which+1) % MAXTH;

  return 0;
}




class WarnOut: public server{

  int dirnum;

public:

  WarnOut(int p):server (p){

    dirnum = 0;

    std::string fname = "";

    if (mapc.get("datadir")!="")
      fname += mapc.get("datadir");
    else
      fname += (string)"/tmp";

    fname += (string)"/";

    if (mapc.get("logfile")!="")
      fname += mapc.get("logfile");
    else
      fname += (string)"warnings";


    OutStack.addfile(fname.c_str());

    settimeout(0,200000);

  }

  int Go(int csock);

  int TimeOut();

};

int WarnOut::Go(int csock)
{
  //int flag = 1;
  //  int result = setsockopt(csock,            /* socket affected */
  //      IPPROTO_TCP,     /* set option at TCP level */
  //      TCP_NODELAY,     /* name of option */
  //      (char *) &flag,  /* the cast is historical
  //            cruft */
  //      sizeof(int));    /* length of option value */
  //   if (result < 0)
  //     //... handle the error ...
  //     perror("Could not set TCP_NODELAY");
  //close(csock);
  setNonblocking(csock);
  OutStack.addsock(csock);
  //nowait();
  OutStack.all_out();
  return 0;
}

int WarnOut::TimeOut()
{
  OutStack.all_out();
  //  std::cout << OutStack.MessageCnt<<std::endl;

  return 0;
}


void * OutThread(void *){

  int op = 12346;

  if (mapc.getint("outport") !=0)
    op = mapc.getint("outport");

  WarnOut S(op);

  //S.settimeout(1,0);
  if (S.Listen()<0) exit(1); /// cpb exit on fail
  //  S.Listen();

  return 0;
}

int main(int argc, char ** argv){
  std::string configfile = "etc/camserv.conf";

  for (int i = 0; i<argc;i++){
    if (strncmp(argv[i],"-C",2) == 0){
      if (i+1 < argc){
        configfile = (std::string) argv[i+1];
      }
      else{
        std::cerr<<"ERROR: Usage: "<<argv[0]<<" -C <filename>"<<std::endl;
        std::cerr<<"ERROR: -C needs an argument: the config file localtion"<<std::endl;
      }
    }
  }

  mapc.setfile(configfile.c_str());

  if (mapc.get("ID")!="")
    cout << "Welcome to Cam Server: "<< mapc.get("ID")<<endl;

  pthread_t pt;
  pthread_attr_t pthread_custom_attr;
  pthread_attr_init(&pthread_custom_attr);

  pthread_create(&pt, &pthread_custom_attr, OutThread, NULL);

  int ip = 12345;

  if (mapc.getint("inport") !=0)
    ip = mapc.getint("inport");

  OutStack.ldir = "data";

  OutStack.wdir = "data";

  std::string ws = mapc.get("webserver");
  if (ws =="") ws = "localhost";
  ws += (string) ":";


  std::string wp = mapc.get("webport");
  if (wp =="") wp = "8888";
  wp += (string) "/";

  ws += wp;

  std::string wd = mapc.get("webdir");
  wd += (string) "/";
  ws+=wd;

  if (mapc.get("datadir") != "")
    OutStack.ldir  = mapc.get("datadir");

  OutStack.wdir  = ws;


  OutStack.MaxFiles = -1;
  OutStack.MaxTime = -1;

  if (mapc.getint("maxfiles") != 0)
    OutStack.MaxFiles  = mapc.getint("maxfiles");


  if (mapc.getint("maxtime") != 0)
    OutStack.MaxTime  = mapc.getint("maxtime");



  WarnIn S(ip);
  S.settimeout(1,00000);
  if (S.Listen()<0) exit(1); // cpb exit on fail
  //S.Listen();

  return 0;
}
#endif

