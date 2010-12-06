// $Id: ThresholdMaker.h,v 1.1  $
#ifndef THRESHOLDMAKER_H 
#define THRESHOLDMAKER_H 1

// Include files
#include <math.h>

/** @class ThresholdMaker ThresholdMaker.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2010-12-06
 */

template <class T>
class ThresholdMaker{
public: 
  /// Standard constructor
  ThresholdMaker(const double sigmas):
    m_sigmas ( sigmas )
  { } 

  virtual ~ThresholdMaker( ) { }   ///< Destructor
  signed int operator()(T& noiseSigma)
  {
    double dthreshold=std::ceil(noiseSigma*m_sigmas);
    return ( static_cast<signed int>(dthreshold) );
  }

protected:

  ThresholdMaker() 
  { }

private:

  double m_sigmas;

};
#endif // THRESHOLDMAKER_H
