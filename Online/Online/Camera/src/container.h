#ifndef CONTAINER_H
#define CONTAINER_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class  container{
  
 public:

  std::vector<std::string> namelist;
  std::vector<std::string> contentlist;
  std::vector<int> sizelist;

  std::vector<void *> objlist;
  
  container();
  void reset();
  int tofile(char *);
  int tostream(std::stringstream &);
  int fromfile(const char *);
  int fromstream(std::stringstream &){std::cout << "Not yet."<<std::endl;return 0;}
  
  void add(const std::string& name ,const std::string& content);
  void add(const std::string& name ,void *,int sz);
  
  int entries(){return sizelist.size();}
  
  int size(int i){return sizelist[i];}
  std::string name(int i){return namelist[i];}
  void * object(int i) {return objlist[i];}
  std::string content(int i){return contentlist[i];}
  int convert();
};


#endif
