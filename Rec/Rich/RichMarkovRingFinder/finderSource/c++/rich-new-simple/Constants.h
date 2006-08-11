
#undef USEROOT

#ifndef LESTER_CONSTANTS_H
#define LESTER_CONSTANTS_H

#include "Utils/MathsConstants.h"

namespace Lester {
  namespace Constants {
    
    const bool warpedView=true;
    typedef enum { simpleModel, hardModel, rich1A, rich2A } Scenario;
    const Scenario scenario = rich2A;

    const bool useRealData=true;
    const double whiffWhaff=0.030; // unit used for jitters etc ...
    const double viewRangeParameter=0.030*5;

    const bool showSomeIdeas = true;
    const bool numberOfFramesBetweenIdeas =25; //25;
    const int numberOfFramesBetweenUpdates = 500; //500;

  }
}
#endif
