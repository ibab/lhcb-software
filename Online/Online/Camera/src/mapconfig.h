#ifndef MAPCONFIG_H
#define MAPCONFIG_H
#include <iostream>
#include <map>
#include <string>
#include <stdio.h>

using namespace std;

class mapconfig{
  map<string,string> m;
  static const int MAX = 1024;
  char buf[MAX+1];
 public:
  
  int setfile(const char * inconf){
    
    FILE * F = fopen(inconf,"r");
    if (F==NULL){
      cerr << "ERROR: Can not open configuration "<<inconf<<endl;
      return -1;
    }
    else{
      std::string s;
      while (fgets(buf,MAX,F)!=NULL){
	s = buf;
	string::size_type st = s.find("#",0);
	std::string ss = s.substr(0,st);
	
	string::size_type st2 = ss.find("=",0);
	if (st2 != string::npos){
	  std::string ssa = ss.substr(0,st2);
	  std::string ssb = ss.substr(st2+1);
	  cout << "config: " << ssa <<  " <- " <<ssb<<endl;
	  m[ssa] = ssb;
	}
      }
    }
   return 1; 
  }  
  mapconfig(char * inconf){
    if (inconf!=NULL)
      setfile(inconf);  
  }
 
  mapconfig(){
  }
  string get(string s){
   map<string,string>::iterator iter = m.find(s);
   if( iter != m.end() ) {
    return iter->second;
    
   }
   else return ""; 

  }
  int getint(string s){
    int i=-1;
    
    map<string,string>::iterator iter = m.find(s);
    if( iter != m.end() ) {
      if (sscanf((iter->second).c_str(),"%d",&i)==1)
	return i;
      else return 0;
      
    }
    
    else return 0; 
    
  }
  

};
#endif
