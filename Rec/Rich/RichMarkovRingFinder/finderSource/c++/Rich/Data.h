
#ifndef LESTER_DATA_H
#define LESTER_DATA_H

#include <vector>

#include "Hit.h"
#include "CircleParams.h"

namespace Lester
{

  class Data
  {
  public:
    typedef std::vector<Hit> Hits;
  public:
    void doNothing() const throw();
    void setFromFile(const std::string & file);
  public:
    Hits hits;
  };

}

#endif
