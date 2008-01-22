// $Id: EAssertions.h,v 1.1 2008-01-22 09:34:58 hernando Exp $
#ifndef HLTBASE_EASSERTIONS_H 
#define HLTBASE_EASSERTIONS_H 1

#include <iostream>
#include <stdexcept>

namespace zen
{

  
  //! exception class: not valid key
  class invalid_key : public std::logic_error {
  public:
    invalid_key(const std::string& key):logic_error(key) {
      std::cout << "--ERROR: invalid key " << key << std::endl;
    }
  };

  
}

  
#endif
