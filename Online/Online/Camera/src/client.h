
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#endif

#include "nio.h"

class client: public nio{
  int rdyn;
#ifndef _WIN32
  fd_set rfds;
  struct timeval tv;
  struct sockaddr_in server;
  struct hostent *hp;
  
  int sock;
#endif
public:
  int m_healthy;
  void wait();
  void nowait();
  int rdend(){return rdyn;}
  int wrend(){return 0;}
  int rd(char*,int);
  int wr(const char*,int);
  void shutwr();
  void shutrd();
  void shut_close(){shutrd();shutwr();
#ifndef _WIN32
  close(sock);
#endif
}
  void new_sock(){
    
    shut_close();
#ifndef _WIN32
    sock = socket (AF_INET, SOCK_STREAM, 0);
     if (sock < 0) {
       perror ("Cannot socket()"); 
       m_healthy =0;
     } 
     else{m_healthy=1;}
#endif
  }
  client(const char * host, unsigned short int port);
  virtual ~client() {}
  int Connect();
 
  int Go();
  
  
};

client::client(const char * host, unsigned short int port){
#ifndef _WIN32
  m_healthy=1;
  signal(SIGPIPE,SIG_IGN);
  
  rdyn = 0;
  
  /* Socket verbinden mit Host/Portnumemr aus Aufrufparam. */
  
  server.sin_family = AF_INET;
  hp = gethostbyname (host);
  if (hp == NULL) {
    fprintf (stderr, "Name resolution failed: %s\n", host);
    m_healthy=0;
    return;
  } 
  memcpy (&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_port = htons (port);
  sock = socket (AF_INET, SOCK_STREAM, 0);
  
  if (sock < 0) {
    perror ("Cannot socket()"); 
    m_healthy=0;
  }

#endif
}

int client::Connect(){
 #ifndef _WIN32
  //  printf("to connect\n");

  // NM: The following messy code it has been added to overcome the 
  // absence of any timeout in connect(). If the server is busy or stopped it
  // hangs for > 10 sec. The way to overcome it is to switch back and for to non-blocking sokets.
  
  // The method followed is:
  //If you are using a non-blocking socket and then call connect, it should return -1 and errno=EINPROGRESS.
  //Then select the socket for writing with the timeout, and after select() 
  //indicates it is "writteable",
  //it has connected (or an error has occurred) so check with getsockopt() 
  //the SO_ERROR at the SOL_SOCKET level... It should be zero if the connection was successful, 
  //any other value is what errno should be if it was a blocking connect().
  //If the timeout period expires, then you should be getting a 0 from select()... 
  
  // Set non-blocking 
  long arg; 
  fd_set myset; 
  if( (arg = fcntl(sock, F_GETFL, NULL)) < 0) { 
    //fprintf(stderr, "NM: Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
    return 1; 
  } 
  arg |= O_NONBLOCK; 
  if( fcntl(sock, F_SETFL, arg) < 0) { 
    //fprintf(stderr, "NM: Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
    return 1; 
  } 
  // Trying to connect with timeout 
  int res = connect (sock, (struct sockaddr *)&server, sizeof (server));
  if (res < 0) { 
    if (errno == EINPROGRESS) { 
      // Connected to a non-blocking soket.
      //fprintf(stderr, "EINPROGRESS in connect() - selecting\n"); 
      do { 
        tv.tv_sec = 0; // Timeout in sec
        tv.tv_usec = 10000; // Timeout in micro sec.
        FD_ZERO(&myset); 
        FD_SET(sock, &myset); 
        res = select(sock+1, NULL, &myset, NULL, &tv); 
        if (res < 0 && errno != EINTR) { 
          // time out exceeded.
          //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
          return -1; 
        } 
        else if (res > 0) { 
          // Socket selected for write 
          socklen_t lon;
          int valopt; 
          lon = sizeof(int); 
          if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
            // Connection not successful
            //fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
            return -1; 
          } 
          // Check the value returned... 
          if (valopt) { 
            // Connection not successful
            //fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt) ); 
            return -1; 
          } 
          break; 
        } 
        else { 
          // Socket not ready for write
          //fprintf(stderr, "Timeout in select() - Cancelling!\n"); 
          return -1; 
        } 
      } while (1); 
    } 
    else { 
      //fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
      return -1; 
    } 
  } 
  // If it gets at this stage the connection was successful within the timeout
  // Set to blocking mode again... 
  if( (arg = fcntl(sock, F_GETFL, NULL)) < 0) { 
    // failed to get back to blocking.
    //fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
    return -1; 
  } 
  arg &= (~O_NONBLOCK); 
  if( fcntl(sock, F_SETFL, arg) < 0) { 
    // failed to get back to blocking.
    //fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
    return -1; 
  } 


  // NM: hold peace of code, it hangs forever if the servers are not responding:
//   if (connect (sock, (struct sockaddr *)&server, sizeof (server)) < 0) {
//     //perror ("connect()"); 
//     return -1;
//   }

  //  printf("from connect\n");
  rdyn=0;
  return 1;
  
  //  fcntl(sock,F_SETFL,O_NONBLOCK);  
#endif
#ifdef _WIN32
  return -1;
#endif
}
void client::nowait(){
#ifndef _WIN32
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
#endif
}

void client::wait(){
 #ifndef _WIN32
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
#endif
}

int client::rd(char * buf,int n){
   #ifndef _WIN32


  if (rdyn!=0) return -1;
  
  tv.tv_sec = 0;
  tv.tv_usec = 1000;
  int r=-1;



  FD_ZERO(&rfds);
  FD_SET(sock, &rfds);

  int ret = select(sock+1,&rfds,NULL,NULL,&tv);


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
#endif
 #ifdef _WIN32
  return -1;
#endif
}

int client::wr(const char * buf,int n){
  #ifndef _WIN32
  return write(sock,buf,n);
#endif
 #ifdef _WIN32
  return -1;
#endif
}

void client::shutwr(){
#ifndef _WIN32
  shutdown(sock,SHUT_WR );
#endif
}

void client::shutrd(){
 #ifndef _WIN32
  shutdown(sock,SHUT_RD );
#endif
}


int client::Go(){
 /*  char buf[2049]; */
/*   int r; */
/*   while ((r = read(sock,buf,2048))>0){ */
/*     buf[r] = '\0'; */
/*     //printf("%d: >%s< ",r,buf); */
/*   } */
/*   rdyn=1; */
/*   printf("Server died.%d\n",r); */
  return 0;
}

