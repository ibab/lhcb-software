// $Id: VeloChargeThreshold.h,v 1.1 2006-05-29 11:55:23 szumlat Exp $
#ifndef VELOCHARGETHRESHOLD_H 
#define VELOCHARGETHRESHOLD_H 1

// Include files
#include "Event/MCVeloFE.h"

/** @class VeloChargeThreshold VeloChargeThreshold.h
 *
 *  the class implements functor predicate for find_if
 *  algorithm to cut-off noisy channels, it is possible to
 *  pass to the functor constructor value of the threshold
 *
 *  @author Tomasz Szumlak
 *  @date   2006-05-21
 */

class VeloChargeThreshold:
public std::unary_function<LHCb::MCVeloFE*,  bool> {

public: 
  /// Standard constructor
  VeloChargeThreshold( ): m_threshold ( 0.0 )
  { }
  VeloChargeThreshold(const double inThres): m_threshold ( inThres ) 
  { }
  virtual ~VeloChargeThreshold( ) { }; ///< Destructor
  bool operator()(LHCb::MCVeloFE* FE)
  {
    // decide to cut out large -ve noise
    return ( FE->charge() < m_threshold );
  }

protected:

private:

  double m_threshold;   /// value of the cut-off threshold

};
#endif // VELOCHARGETHRESHOLD_H
