#ifndef _SiChargeFun_H
#define _SiChargeFun_H 1

#include <algorithm>
#include <numeric>
#include <vector>
#include <cmath>

#include "LHCbMath/ModeFunctions.h"
#include "Kernel/SiDataFunctor.h"

/** @class SiChargeFun SiChargeFun.h "SiChargeFun.h"
 *
 *  namespace containing classes for extracting charge, mean charge
 *  robust estimates of the mode etc
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */


namespace SiChargeFun{


  /** calculate the shorth
  * mean of the half sample 
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

  /** truncated mean
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @param trimFraction amount of tail to reject
  * @return double LMS
  */
  template <typename TYPE>
    double truncatedMean(TYPE start, TYPE stop, double trimFraction = 0.7);

  /** <a href = "http://en.wikipedia.org/wiki/Harmonic_mean" > Generalized Mean </a>
   * @param start beginning of sequence 
   * @param stop  end of sequence
   * @param power 
   * @return double LMS
   */
  template <typename TYPE>
    double generalizedMean(TYPE start, TYPE stop, double p = -2.0);

  /** Mean [for completeness]
   * @param start beginning of sequence 
   * @param stop  end of sequence
   * @return double mean
   */
  template <typename TYPE>
    double mean(TYPE start, TYPE stop);

  /** extract the charge from the list 
  * @param start beginning of sequence 
  * @param stop  end of sequence
  * @param values OUTPUTLIST
  */
  template<typename INTYPE>
    void extract(INTYPE start, INTYPE stop, std::vector<double>& values);

  /** extract charge from single object 
  * @param TYPE object 
  */
  template<class TYPE>
    double charge(TYPE input); 

}

template <typename TYPE>
inline void SiChargeFun::extract(TYPE start, TYPE stop, std::vector<double>& values){
  values.reserve(stop-start);
  for (TYPE iter = start; iter != stop; ++iter) {
    values.push_back(SiChargeFun::charge(*iter)); 
   } // values
  //typedef TYPE::value_type;
  //std::transform(start,stop,std::back_inserter(values), std::mem_fun(&TYPE::value_type::totalCharge));
}

template <class TYPE> 
inline double SiChargeFun::charge(TYPE input) {
  return input->totalCharge();
}

template<typename TYPE>
inline double SiChargeFun::shorth(TYPE start, TYPE stop){
  std::vector<double> values; SiChargeFun::extract(start,stop,values);
  std::sort(values.begin(), values.end());
  return ModeFunctions::shorth(values.begin(), values.end());
}

template <typename TYPE>
inline double SiChargeFun::LMS(TYPE start, TYPE stop){
  std::vector<double> values; SiChargeFun::extract(start,stop,values);
  std::sort(values.begin(), values.end());
  return ModeFunctions::LMS(values.begin(), values.end());
}

template <typename TYPE>
inline double SiChargeFun::truncatedMean(TYPE start, TYPE stop, double trimFraction){
  std::vector<double> values; SiChargeFun::extract(start,stop,values);
  const unsigned int reducedSize =
      (unsigned int)ceil(trimFraction*(double)values.size());
  std::nth_element( values.begin(), values.begin()+reducedSize, values.end() );
  return( reducedSize != 0.0 ?  std::accumulate( values.begin(), values.begin()+reducedSize, (double)0.0 )/((double)reducedSize) : 0.0); 
}

template <typename TYPE>
inline double SiChargeFun::generalizedMean(TYPE start, TYPE stop, double power){
  std::vector<double> values; SiChargeFun::extract(start,stop,values);
  return ModeFunctions::generalizedMean(values.begin(), values.end(), power);
}

template <typename TYPE>
inline double SiChargeFun::mean(TYPE start, TYPE stop){
  const unsigned int size = stop - start;
  return size != 0 ? std::accumulate(start,stop,0.0,SiDataFunctor::Accumulate_Charge<typename TYPE::value_type>())/size : 0.0;
}


#endif
