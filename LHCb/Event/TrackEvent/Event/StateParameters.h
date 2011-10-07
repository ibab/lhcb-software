#ifndef TRACKEVENT_STATEPARAMETERS_H
#define TRACKEVENT_STATEPARAMETERS_H 1

// Include files
#include "GaudiKernel/SystemOfUnits.h"

/** @namespace StateParameters
 *
 * This namespace defines some useful variables for the
 * manipulation of States.
 *
 *  @author Eduardo Rodrigues
 *  @date   2006-08-02
 * 
 */

namespace StateParameters
{
  /// Pre-defined z-positions of states for certain State locations
  /// (see Location enum in State.h)
  static const double ZBegRich1 =   990. * Gaudi::Units::mm;
  static const double ZEndRich1 =  2165. * Gaudi::Units::mm;
  static const double ZEndT     =  9410. * Gaudi::Units::mm;
  static const double ZBegRich2 =  9450. * Gaudi::Units::mm;
  static const double ZEndRich2 = 11900. * Gaudi::Units::mm;
  static const double ZBegT     =  7500. * Gaudi::Units::mm;
  static const double ZMidT     =  8520. * Gaudi::Units::mm;
  static const double ZEndTT    =  2700. * Gaudi::Units::mm;
  static const double ZEndVelo  =   770. * Gaudi::Units::mm;
}

// ----------------------------------------------------------------------------
//   end of namespace
// ----------------------------------------------------------------------------

#endif /// TRACKEVENT_STATEPARAMETERS_H
