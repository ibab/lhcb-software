#ifndef HISTFUN_H
#define HISTFUN_H 1

// Include files
#include <vector>
#include "GaudiKernel/IHistogramSvc.h"
// all the fun you can want with histograms !

namespace HistFun {

template <typename TYPE>
inline void unBookVector(std::vector<TYPE >& histoVector, IHistogramSvc* hSvc ){
  // helper for unbooking histos from store
  std::for_each( histoVector.begin(), histoVector.end(),
                 [&](const TYPE& t) { hSvc->unregisterObject(t).ignore(); } );
}

template <typename TYPE>
inline void eraseVector(std::vector<TYPE > & histoVector){
  // helper for DELETING histos from vector
  std::for_each( histoVector.begin(), histoVector.end(),
                 [](TYPE& h) { delete h; });
  histoVector.clear();
}

}

#endif // HISTFUN_H
