#include <iostream>
#include "TAN/TanDB.h"
extern "C" int rtl_tandb_dump ( int /* argc */, char** /* argv */ )  {
  std::cout << "+------------------------------------------------------------------------------+" << std::endl;
  std::cout << "||||||||||||           T A N   D A T A B A S E   D U M P            ||||||||||||" << std::endl;
  std::cout << "+------------------------------------------------------------------------------+" << std::endl;
  TanDataBase::Instance().Dump( std::cout );
  std::cout << "--------------------------------------------------------------------------------" << std::endl;
  return 0x1;
}
