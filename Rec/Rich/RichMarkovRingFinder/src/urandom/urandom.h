
#ifndef RICHMARKOV_URANDOM_H
#define RICHMARKOV_URANDOM_H

#include <fstream>

namespace RichMarkov {

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
      ans|=k;
    };
    return ans;
  };

}

#endif
