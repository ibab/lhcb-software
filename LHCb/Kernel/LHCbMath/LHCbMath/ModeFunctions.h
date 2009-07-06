#ifndef _ModeFunctions_H
#define _ModeFunctions_H 1

#include <algorithm>
#include <numeric>
#include <vector>
#include <utility>


/** @namespace ModeFunctions ModeFunctions.h "ModeFunctions.h
 *
 *  namespace containing fuctions for getting robust estimate of the mode
 *  reasoanbly generic interface......
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

namespace ModeFunctions{

  /** calculate the shorth 
  * the mean of the half sample 
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @return double shorth
  */
  template <typename TYPE>
    double shorth(TYPE start, TYPE stop);

  /** calculate the LMS 
  * average of the values of the half sample 
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @return double LMS
  */
  template <typename TYPE>
    double LMS(TYPE start, TYPE stop);

 /** calculate the halfSample
  * value of i for which distance x_i, x_i+h is minimum
  * where h is the half sample 
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @return TYPE iterator for starting position
  */
  template <typename TYPE>
    std::pair<TYPE,TYPE> halfSample(TYPE start, TYPE stop);

 /** calculate the halfSampleWidth
  * size of the half sample 
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @return TYPE iterator for starting position
  */
  template <typename TYPE>
    double halfSampleWidth(TYPE start, TYPE stop);

  /** <a href = "http://en.wikipedia.org/wiki/Harmonic_mean" > Generalized Mean </a>
   * @param start beginning of sequence 
   * @param stop  end of sequence
   * @param p     power 
   * @return double LMS
   */
   template <typename TYPE>
    double generalizedMean(TYPE start, TYPE stop, double p = -0.5);
}

template <typename TYPE>
inline std::pair<TYPE,TYPE> ModeFunctions::halfSample(TYPE start, TYPE stop){
 const unsigned int halfSize = (unsigned int)((stop-start)/2u); 
 unsigned int pos = 0u;
 unsigned int bestpos = halfSize-1u;
 for (TYPE iter = start; iter != stop ; ++iter, ++pos){
   if (pos < halfSize){
    if (*(iter+halfSize) - *iter < *(start+bestpos+halfSize) - *(start+bestpos) ) { 
       bestpos = pos; 
     }
   }
 } //iter1 
 return std::make_pair(start+bestpos, start+bestpos+halfSize);
}

template <typename TYPE>
inline double ModeFunctions::halfSampleWidth(TYPE start, TYPE stop){
  // trivial cases
  if (start == stop){
    return 0.0;
  }
  else if (stop - start <= 2) {
    return 0.5* fabs(*start - *stop); 
  }
  std::pair<TYPE,TYPE> bestpos = halfSample(start,stop);
  return fabs(*bestpos.first - *bestpos.second);
}

template<typename TYPE>
inline double ModeFunctions::shorth(TYPE start, TYPE stop){
   // trivial cases
   if (start == stop) {
    return 0.0;
   }
   else if (stop - start == 1) {
     return *start;
   } 
   else if (stop - start == 2){
     return (*start + *(start+1))/2u;
   }
   std::pair<TYPE,TYPE> bestpos = halfSample(start,stop);
   const unsigned int halfSize = (unsigned int)((stop-start)/2u); 
    return std::accumulate(bestpos.first, bestpos.second,0.0)/double(halfSize);
}

template <typename TYPE>
inline double ModeFunctions::LMS(TYPE start, TYPE stop){
   // trivial cases
   if (start == stop) {
    return 0.0;
   }
   else if (stop - start == 1) {
     return *start;
   } 
   else if (stop - start == 2){
     return 0.5*(*start + *(start+1));
   }

   std::pair<TYPE,TYPE> bestPos = halfSample(start,stop);
   return 0.5* (*(bestPos.first) + *(bestPos.second));
}

template <typename TYPE>
inline double ModeFunctions::generalizedMean(TYPE start, TYPE stop, double power){

  double sum = 0.0; unsigned int size = 0u;
  for (TYPE iter = start; iter != stop; ++iter, ++size){
    if (power > 0.0){
      sum += pow(*iter,power);
    }
    else { 
      // we have to be careful if the data is zero
      double value = std::max(1e-5,fabs(*iter));
      sum +=pow(value,power);      
    }
  } // iter
  return size != 0u ?  pow(sum/double(size),1.0/power) : 0.0;
}


#endif
