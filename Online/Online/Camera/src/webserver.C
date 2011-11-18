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
#include <unistd.h>
#include <string>

#include <signal.h>

#include "nio.h"

#include "mapconfig.h"

mapconfig mapc; //("etc/webs.conf");


#include "reply.C"

class blockingIO:public nio{

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


class server{

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
  secs =5;
  usecs=0;
}

int server::Go(int csock){
  printf("server::Go()\n");
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
    perror("Cannot bind:");
    return -1;
  }
  int r = listen(sock, 5);
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

int BUSYTHREAD =0;

extern "C" {
  void * incomingthread(void * vgns){
    BUSYTHREAD++;
    worker * wrk = (worker *)vgns;
    blockingIO * bio = new blockingIO(wrk->csock);
    reply *gns = new reply( bio,wrk->unique ); 
    
    while (1){
      
      //  delete gns;
      //gns = new getnsave( bio );
    
      BUSYTHREAD--;
      wrk->ready=1;
      pthread_mutex_lock(&(wrk->mtx));

      BUSYTHREAD++;

      bio->reset(wrk->csock);
      
      wrk->ready=0;
      // blockingIO * bio = new blockingIO(wrk->csock);
      // getnsave *gns = new getnsave( bio );
      // do sth
      
      gns->start();
      gns->end();
      gns->reset();
      
    }
    return NULL; // CRJ - No idea what to return here, but needed for gcc 4.6
  }
}

#include <pthread.h>
#include <sys/types.h>

class WarnIn: public server{
  int cnt;
  static const int MAXTH = 30;
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
};


int WarnIn::Go(int csock){
  
  // printf("%d\n",cnt++);
  if (BUSYTHREAD==MAXTH)  {
    close(csock); 
    fprintf(stderr,"all threads busy %d/%d\n",BUSYTHREAD,MAXTH);
    which = (which+1) % MAXTH; 
  }
  else{
    while (wrk[which].ready !=1) {
      close(csock); 
      fprintf(stderr,"%d busy\n",which);
      which = (which+1) % MAXTH; 
    }
  }
  wrk[which].csock = csock;
  pthread_mutex_unlock(&(wrk[which].mtx));
  which = (which+1) % MAXTH;

  return 0;
}




int main(int argc, char **argv){
  std::string configfile = "etc/webs.conf";
  
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

  int p;
  
  if ((p=mapc.getint("websport"))==0)
    p = 8888;

  WarnIn S(p);
  S.settimeout(0,0);
  S.Listen();

  return 0;
}
#endif

