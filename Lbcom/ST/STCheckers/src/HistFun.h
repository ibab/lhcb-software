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
  typename std::vector<TYPE >::iterator iterHisto = histoVector.begin(); 
  while (iterHisto != histoVector.end()) {
    hSvc->unregisterObject(*iterHisto).ignore();
    ++iterHisto;
  } // iterHisto
}

template <typename TYPE>
inline void eraseVector(std::vector<TYPE > & histoVector){

  // helper for DELETING histos from vector
  typename std::vector<TYPE >::iterator iterHisto = histoVector.begin(); 
  while (iterHisto != histoVector.end()) {
    delete *iterHisto;
    iterHisto = histoVector.erase(iterHisto);    
  } // iterHisto
}
 
}

#endif // HISTFUN_H
