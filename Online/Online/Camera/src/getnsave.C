#ifndef _WIN32
#include "getnsave.h"
#include <string>
#include <sstream>
#include "camline.h"
#include "mapconfig.h"

#ifndef THREADCNT
#define THREADCNT
const int MAXTH=10;
//static int BUSYTHREAD;
#endif

//class mapconfig mapc;

std::string CONFIG_STR();

void * getnsave::start(){

  int r = getline(buf,2999);

  if (r<=0)
    return NULL;

  buf[r]='\0';

  camline cl;
  cl.in(buf);

  //check if buf is a command
  if (strncmp(buf,"COMMAND/",8)==0){

    //if so interpret it
    if (strncmp(cl.message.c_str(),"status",6)==0){
      os->addline(CONFIG_STR().c_str());
      return NULL;
    }
    if (strncmp(cl.message.c_str(),"switch",6)==0){
      char * buf = (char *)malloc(cl.message.size());
      if (buf !=NULL){
        if ( sscanf(cl.message.c_str(),"%*s %s",buf) ==1){
          std::string s = ldir+"/"+(std::string)buf;
          if (os->Fswitch(s.c_str())<0){
            os->addline("SERVER/3/OutStack::Fswitch failed");
          }
        }
        else {
          os->addline("SERVER/3/sscanf failed");;
          return NULL;
        }
      }
      else{
        os->addline("SERVER/3/malloc failed");
        return NULL;
      }
      return NULL;
    }
    // std::cerr <<"unknown command: "<<cl.message<<std::endl;
    return NULL;
    //broadcast result
  }


  char nbuf[2049];

  int re=0;
  int outn = getrest(nbuf,2048,&re);
  //  std::cout << re <<" rest"<<std::endl;

  if (re>0){

    sprintf(tempf,"%scam.%s.%d.XXXXXX",ldir.c_str(),id.c_str(),u);
    int s = mkstemp(tempf);

    std::string templ = tempf;

    string::size_type st = templ.find_last_of( "/" );
    std::string fstr;
    if (st != string::npos) {
      fstr= templ.substr(st+1);
    }
    else fstr = templ;

    //if (r>0){

    cl.URL = ws+fstr;
    std::string sout;
    cl.out(sout);
    os->addline(sout.c_str());

    //}

    if (s<0) perror("mkstemp");

    while(re>0){
      write(s,nbuf,re);
      re =0;
      outn = getrest(nbuf,511,&re);
    }
    close(s);
  }
  else{
    //if (r>0){
    std::string sout;
    cl.out(sout);
    os->addline(sout.c_str());
    //}
  }

  return NULL;

}

void getnsave::end(){
  getnio()->shut_close();
}

#endif
