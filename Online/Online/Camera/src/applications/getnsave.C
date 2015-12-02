#ifndef _WIN32
#include "getnsave.h"
#include <string>
#include <sstream>
#include "camline.h"
#include "mapconfig.h"

#ifndef THREADCNT
#define THREADCNT
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

  //  std::cout << OutStack.MessageCnt<<std::endl;

  //check if buf is a command
  if (strncmp(buf,"COMMAND/",8)==0){

    //if so interpret it
    if (strncmp(cl.message.c_str(),"status",6)==0){
      os->addline(CONFIG_STR().c_str());
      // return NULL; //cpb later
    }
    if (strncmp(cl.message.c_str(),"switch",6)==0){
      std::cerr<<"Manual switch initiated using directory "<<std::endl; // cpb new manual switching block

      if ( sscanf(cl.message.c_str(),"%*s %s",bufm) ==1){
        //std::string s = ldir+"/"+(std::string)buf;
        std::cerr<<bufm<<std::endl;
        pthread_mutex_lock(&(os->switchmtx));
        if (os->Fswitch(bufm)<0){
          os->addline("SERVER/3/OutStack::Fswitch failed");
          std::cerr << "Manual switch failed." <<std::endl;
        }
        pthread_mutex_unlock(&(os->switchmtx));
      }
      else {
        os->addline("SERVER/3/sscanf failed");;
      }
    }
    // std::cerr <<"unknown command: "<<cl.message<<std::endl;
    char nbuf[2049];
    int re=0;
    //int outn =
    getrest(nbuf,2048,&re);
    if (re>0){
      while(re>0){
        re =0;
        //outn = 
        getrest(nbuf,511,&re);
      }
    }
    return NULL;
    // cpb end of new switching block
  }
  pthread_mutex_lock(&(os->switchmtx));
  if ((os->MaxTime >0 ) && ((time(NULL) - os->lastSwitch) > os->MaxTime)) {
    (os->DirNum)++;
    std::cerr <<"More than "<<os->MaxTime<< " seconds since last switch."<<std::endl;
    os->Fswitch(os->lastName.c_str(),os->DirNum);
  }
  pthread_mutex_unlock(&(os->switchmtx));

  pthread_mutex_lock(&(os->switchmtx));
  if ( (os->MaxFiles >0) && (os->MessageCnt>=os->MaxFiles)) {
    (os->DirNum)++;
    std::cerr <<"More than "<<os->MaxFiles<< " files since last switch."<<std::endl;
    os->Fswitch(os->lastName.c_str(),os->DirNum);
  }
  pthread_mutex_unlock(&(os->switchmtx));

  char nbuf[2049];

  int re=0;
  //int outn = 
  getrest(nbuf,2048,&re);
  //  std::cout << re <<" rest"<<std::endl;

  if (re>0){

    // don't start a new file while we are switching locations with Fswitch!
    pthread_mutex_lock(&(os->locmtx));
    cldir = os->currLoc();
    std::string cwdir = os->currWeb();
    //get the current location
    //string cloc = os->dataLocation();
    sprintf(tempf,"%scam.%s.%d.XXXXXX",cldir.c_str(),id.c_str(),u);
    int s = mkstemp(tempf);
    // std::cout <<tempf<<std::endl;

    std::string templ = tempf;

    string::size_type st = templ.find_last_of( "/" );
    std::string fstr;
    if (st != string::npos) {
      fstr= templ.substr(st+1);
    }
    else fstr = templ;

    //if (r>0){

    cl.URL = cwdir+fstr;
    std::string sout;
    cl.out(sout);
    os->addline(sout.c_str());
    pthread_mutex_unlock(&(os->locmtx));

    //}

    if (s<0) perror("mkstemp");

    while(re>0){
      write(s,nbuf,re);
      re =0;
      //outn = 
      getrest(nbuf,511,&re);
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
