#ifndef CONTAINER_ROOT_H
#define CONTAINER_ROOT_H

#include "container.h"

class  container_ROOT: public container{

  int numHistos;
  int numFuncs;
 public:
  
  container_ROOT();

  void reset();
  
  
  int convert();
};


#endif
