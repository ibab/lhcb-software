#ifndef CAMLINE_H
#define CAMLINE_H
#include <string>
#include <iostream>
class camline{
 public:
  // old content
  std::string from;
  int ilevel;
  std::string slevel;
  std::string message;
  std::string URL;

  // plus new content (after level)
  std::string to;
  std::string ID;
  std::string refID;


  


  camline(){from=slevel=message=to=ID=refID=URL="";}
  void  in(std::string & s);
  void   in (const char * s){
    //old short format
    std::string ss = s; 
    in (ss);
  }
  std::string out(std::string & sout);
};

#endif
