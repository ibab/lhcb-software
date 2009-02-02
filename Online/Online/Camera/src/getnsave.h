#ifndef GETNSAVE_H
#define GETNSAVE_H
#include "mapconfig.h"
#include "nio.h"

#include <string>
#include <list>
#include <vector>

#include <pthread.h>
#include <string>

class outstack{
  
  std::list<std::string> lines;
  std::list<int> socks;
  //std::list<std::string> files;
  std::list<FILE*> Flist;

  
public:
  std::string ldir;
  
  pthread_mutex_t mtx;
  pthread_mutex_t Fmtx;

  int addfile(const char * name){
    std::string s = name;
    //files.push_back(s);
  
    FILE *F = fopen(s.c_str(),"a");
    if (F!=NULL){
      Flist.push_back(F);
      std::string ls = ldir;//mapc.get("datadir");
      if (ls=="") ls = ".";
      ls += "/filelist.txt";
      FILE * Flog = fopen(ls.c_str(),"a");
      if (Flog!=NULL){
	time_t t = time(NULL);
	fprintf(Flog,"%s %s\n",s.c_str(),ctime(&t));
	fclose(Flog);
      }
      else {
	perror("Opening filelist.txt");
      }
       
      return 1;
    }
    else{
      perror("outstack::addfile()");
      return -1;
    }
  }

  void addsock(int i){socks.push_back(i);}
  
  
  
  void addline(const char *l){
    
    std::string s = l;
    pthread_mutex_lock(&mtx);
    lines.push_back(s);
    pthread_mutex_unlock(&mtx);
  }

#include <errno.h>
 
  
  int  Fswitch(const char *n){
 
    pthread_mutex_lock(&Fmtx);
    int re = addfile(n);
    if (Flist.size()>1){
      std::list<FILE*>::iterator it = Flist.begin();
      fclose((*it));
      Flist.erase(it);
    }
    
    pthread_mutex_unlock(&Fmtx);
    return re;
  }
  void all_out(){

    if (lines.size()<1) return;
    
    pthread_mutex_lock(&mtx);
    std::list<std::string>  linescp;  
    for (std::list<std::string>::iterator itl = lines.begin();itl != lines.end(); itl++){
      linescp.push_back((*itl));
    }
    lines.clear();
    pthread_mutex_unlock(&mtx);
    
    
    std::list<int> errsocks;




    for (std::list<std::string>::iterator itl = linescp.begin();itl != linescp.end(); itl++){
      pthread_mutex_lock(&Fmtx);
      for (std::list<FILE*>::iterator it = Flist.begin();it != Flist.end(); ++it){	
	fprintf((*it),"%s\n",(*itl).c_str());
      }
      pthread_mutex_unlock(&Fmtx);
      for (std::list<int>::iterator it = socks.begin();it != socks.end(); ++it){

	
	int ret = write((*it), ((*itl)+"\n").c_str(), (*itl).size()+1 );
	
	if (ret<0) {
	  	  
	  if (errno != EAGAIN){
	  
	    errsocks.push_back((*it));
	    
	  }
	  perror("OutStack::out_all(),write()");  
	}
	
      }    
    }


    for (std::list<FILE*>::iterator it = Flist.begin();it != Flist.end(); ++it){	
      fflush((*it));
    }

     
    for (std::list<int>::iterator it = errsocks.begin();it != errsocks.end();++it){
      std::cout << "closing " << (*it)<<std::endl; 
      close((*it));
      socks.remove((*it));
    }
    errsocks.clear();
    
}
  
  void all_file_out(){
    
    if (lines.size()<1) return;
    
    pthread_mutex_lock(&mtx);
    
    for (std::list<FILE*>::iterator it = Flist.begin();it != Flist.end(); ++it){
      for (std::list<std::string>::iterator itl = lines.begin();itl != lines.end(); ++itl){
	fprintf((*it),"%s\n",(*itl).c_str());
      }
    }
    lines.clear();
    pthread_mutex_unlock(&mtx);
  }
  
  void all_sock_out(){
    
    if (lines.size()<1) return ;
    
    pthread_mutex_lock(&mtx);
    
    
    for (std::list<int>::iterator it = socks.begin();it != socks.end(); ++it){
      
      for (std::list<std::string>::iterator itl = lines.begin();itl != lines.end(); ++itl){
	
	int ret = write((*it), ((*itl)+"\n").c_str(), (*itl).size()+1 );
	//fprintf(F,"%s\n",(*itl).c_str());
	if (ret<0){
	  it--;
	  std::list<int>::iterator itdel = it;
	  itdel++;
	  socks.erase(itdel);
	}

      }
      
    }
    pthread_mutex_unlock(&mtx);
  }
  
  
  
  outstack(){
    ldir = "";
    pthread_mutex_init(&mtx,NULL);
    pthread_mutex_init(&Fmtx,NULL);
  }
  
  

  
};

class getnsave : public proto{
  
private:
  char buf[3000];
  char tempf[1024];
  int u;
  outstack * os;
  std::string ldir,ws,wp,wd,id;
  std::string templ;
  
 public:
  getnsave(nio * n,int unique,mapconfig & mapc):proto(n){
    u = unique;
    sprintf(tempf,"outfile.%d.XXXXXX",unique);
    
    ldir = mapc.get("datadir");
    if (ldir =="") ldir = "/tmp"; 
    ldir += (string) "/";
    
    id = mapc.get("ID");
    
    ws = mapc.get("webserver");
    if (ws =="") ws = "localhost"; 
    ws += (string) ":";
    
    
    wp = mapc.get("webport");
    if (wp =="") wp = "8888"; 
    wp += (string) "/";
   
    ws += wp;
   
    wd = mapc.get("webdir");
    wd += (string) "/";
    ws+=wd;
    //  cout <<"configured to use: " << ws <<" and "<<ldir<<id<<endl;
    
    
    
  };

  void setoutstack(outstack* o){os = o;}

  void * start();
  void end();
  void reset(){}
};
#endif

