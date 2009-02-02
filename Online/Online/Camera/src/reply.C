

#include "nio.h"

#include <string>
#include <iostream>
#include <list>
#include <vector>

#include <pthread.h>

#include <limits.h>
#include <stdlib.h>

using namespace std;

class reply : public proto{
  
private:
  char allowed[512];
  char webdir[3000];
  char buf[3000];
  char nbuf[3000];
  int u;

public:
  reply(nio * n,int unique):proto(n){
    u = unique;
    
    std::string wdir = mapc.get("websdir");
    std::string rdir = mapc.get("httproot");
    
    if (rdir != "")
      strcpy(allowed,rdir.c_str());
    else {
      strcpy(allowed,getenv("PWD"));
      strcat(allowed,"/data/");
    }
    if (wdir != ""){
      strcpy(webdir,wdir.c_str());
    } 
    else {
      strcpy(webdir,getenv("PWD"));
    }
  };

  void * start();
  void end();
  void reset(){
    proto::reset();
  }
};

void * reply::start(){
  //printf("starting communication\n");
  
  int r = getline(buf,2500);
  if (r<=0) return NULL;  
  while ( (buf[r-1] =='\r') || (buf[r-1] == '\n') ){
    buf[r--] = '\0';
  }
  printf(">%s<\n",buf);
  if (strncmp(buf,"GET ",4)!=0) return NULL;
  
  string instr = buf;
  
  if (sscanf(buf,"%*s %s",nbuf)!=1) return NULL;
  
  strcpy(buf,webdir);
  strcat(buf,nbuf);
  
  string filestr = buf;
  // cout <<">"<< filestr<<"<" <<endl;
  
  
  char pbuf[PATH_MAX];
  if (realpath(buf, pbuf)==NULL) {
    // cerr << "Could not resolve path "<<buf<<endl;
    perror("realpath");
    return NULL;
  }
  
  if (strncmp(pbuf,allowed,strlen(allowed))!=0){
    // cerr << "Not allowed "<<buf<<endl;
   
    return NULL;
  }
  
  FILE *F = fopen(filestr.c_str(),"rb");
  
  if (F==NULL) return NULL;

  char buf[512];
  
  while ((r=fread(buf,1,512,F))>0)
    getnio()->wr(buf,r);
  
    
  return NULL;
  
}

void reply::end(){
  getnio()->shut_close();
}

