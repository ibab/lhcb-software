

#ifndef LESTER_DATA_H
#define LESTER_DATA_H

#include "Hit.h"
#include "CircleParams.h"
#include <vector>

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
#ifdef LESTER_USE_GRAPHICS
    void draw(BasicCanvasInterface & canvas, const bool showTruth=false,
              const double hitScaleFactor=1.0) const;
#endif
    void setFromFile(const std::string & file);
    void jokeSetRandom(); /* deprecated */
  };

}

#endif
