#ifndef NIO_H
#define NIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/tcp.h>
#endif

class nio{
  
 public: 
  virtual ~nio() {}
  virtual int rdend()=0;
  virtual int wrend()=0;
  virtual int rd(char *,int)=0;
  virtual int wr(const char *, int)=0;
  virtual void shutwr()=0;
  virtual void shutrd()=0;
  virtual void shut_close()=0;
  
};

//#ifndef _WIN32
class proto{
  nio * m_nio;
  char m_buf[4000];
  
  int m_inl;
  int m_pos;
  
  int m_bufopos;  

public:
  nio * getnio(){return m_nio;}
  proto(nio * n){m_nio =n;m_pos=-1;m_inl=-1;m_bufopos=0;}
  
  int reget(){    
    m_inl = m_nio->rd(m_buf,3999);
    m_pos = 0;
    m_bufopos = 0;
    //    printf("reget()= %d\n",m_inl);
    return m_inl;
  }
  
 
  int getline(char *,int n);
  int getnum(char *, int n, int *r);
  int getrest(char *, int n , int * r);
  void reset(){m_pos=-1;m_inl=-1;m_bufopos=0;}
    
 
};
//#endif
#endif
