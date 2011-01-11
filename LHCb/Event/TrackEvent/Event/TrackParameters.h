#ifndef TRACKEVENT_TRACKPARAMETERS_H
#define TRACKEVENT_TRACKPARAMETERS_H 1

// Include files
#include "GaudiKernel/SystemOfUnits.h"

/** @namespace TrackParameters
 *
 * This namespace defines some useful items 
 *
 * @author unknown
 * created Wed Feb  9 15:43:46 2005
 * 
 */

namespace TrackParameters
{
  static const double hiTolerance     = 1e-40;          ///< High tolerance
  static const double lowTolerance    = 1e-20;          ///< Low tolerance
  static const double looseTolerance  = 1e-5;           ///< Loose tolerance
  static const double propagationTolerance = 1e-6*Gaudi::Units::mm ; ///< Tolerance for extrapolators
  static const double moliereFactor   = 13.6*Gaudi::Units::MeV; ///< Moliere factor
}


// -----------------------------------------------------------------------------
//   end of namespace
// -----------------------------------------------------------------------------

#endif /// TRACKEVENT_TRACKPARAMETERS_H
