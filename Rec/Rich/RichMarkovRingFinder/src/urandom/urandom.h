
#ifndef LESTER_URANDOM_H
#define LESTER_URANDOM_H

#include <fstream>

namespace Lester {
	
char urandchar() {
  static std::ifstream f("/dev/urandom");
  return f.get();
};

template <class T>
T urand() {
  T ans=0;
  for (unsigned int i=0; i<sizeof(T); i++) {
    ans<<=8;
    T k = ((static_cast<T>(urandchar()))&0xFF);
    //    std::cout << "k is " << k << " while ans is "  << ans << std::endl;
    ans|=k;
  };
  //std:: cout << "seed was " << ans << std::endl;
  return ans; 
};


};

#endif
