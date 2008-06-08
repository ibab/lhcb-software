
#ifndef LESTER_DATA_H
#define LESTER_DATA_H

#include <vector>

#include "Hit.h"
#include "CircleParams.h"
#include "NimTypeRichModel.fwd"

namespace Lester {

  class Data {
  public:
    typedef std::vector<Hit> Hits;
    void doNothing() const throw();
  public: // should be private: but temporarily public for revision of NIM paper
    typedef std::vector<CircleParams> Circs;
    Circs secretCircs;
    Hits secretBg;
  public:
    Hits hits;
    void setFromFile(const std::string & file);
    void jokeSetRandom(const NimTypeRichModel & ntrm); /* deprecated */
  };

}

#endif
