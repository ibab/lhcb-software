

#include "RichParams.h"
#include <iostream>



std::ostream & operator<<(std::ostream & os, const Lester::RichParams & rp) {
  return rp.printMeTo(os);
};


