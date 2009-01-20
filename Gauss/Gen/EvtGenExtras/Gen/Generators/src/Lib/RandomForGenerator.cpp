// $Id: RandomForGenerator.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Include files

// local
#include "Generators/RandomForGenerator.h"

Rndm::Numbers RandomForGenerator::s_randgaudi ;

Rndm::Numbers & RandomForGenerator::getNumbers() {
  return s_randgaudi ;
}
