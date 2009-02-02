#ifndef CONTAINER_ROOT_H
#define CONTAINER_ROOT_H

#include "container.h"

#include <TH1.h>
#include <TH2.h>
#include <TEllipse.h>
#include <TLine.h>
#include <TBox.h>
#include <TArrow.h>
#include <TF1.h>
#include <TF2.h>

class  container_ROOT: public container{

  int numHistos;
  int numFuncs;
 public:
  
  container_ROOT();

  void reset();
  
  
  int convert();
};


#endif
