
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include <TSocket.h>
#include <iostream>
#include "nio.h"

class client: public nio{
  int rdyn;
  std::string m_host;
  int m_port;
  TSocket * sock;
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
  void shut_close(){shutrd();shutwr();if(sock!=NULL)sock->Close("force");}
  
  void new_sock(){
    
    shut_close();
    if (sock !=NULL) {
      delete sock;
      sock =NULL;
    }
  }
  
  client(const char * host, unsigned short int port);
  int Connect();
  
  int Go();
  
  
};

client::client(const char * host, unsigned short int port){
  m_host = (std::string)host;
  m_port = port;
  sock = NULL;
  rdyn = 1;
}

int client::Connect(){
  sock = new TSocket(m_host.c_str(),m_port,64000);
  if (sock->IsValid()){
    m_healthy = 1;
  }
  else {
    std::cerr << "socket is not valid"<<std::endl;
    m_healthy = 0;
    return -1;
  }

  // sock->SetOption(kNoBlock,1);  

  rdyn=0;
  return 1;
  
}

void client::nowait(){
  sock->SetOption(kNoBlock,1);  

}

void client::wait(){
  sock->SetOption(kNoBlock,0); 
}

int client::rd(char * buf,int n){

  if (sock==NULL) return -1;

  if (rdyn!=0) return -1;
  

  int ret = sock->Select(0,1);
  //if (ret>0)std::cerr<<"Select " << ret<<std::endl;
  int r;
  
  if (ret == 0) {return -2;}
  if (ret <0){perror("select");return -1;}
  if (ret > 0){
    r = sock->RecvRaw(buf,1);
  }
  if (r>=0)
    buf[r] = '\0';
  //  printf("rd: %d %s %d\n",r,buf,n);

  if (r==0) rdyn= 1;
  if (r<0 ) rdyn=-1;
  
  return r;

}

int client::wr(const char * buf,int n){
  if (sock==NULL) return -1;
  sock->SendRaw(buf,n);
}

void client::shutwr(){
  if (sock!=NULL) sock->Close();
}

void client::shutrd(){
  if (sock!=NULL) sock->Close();
}


int client::Go(){

  return 0;
}

