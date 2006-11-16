#ifndef TRACKEVENT_TRACKUNITSCONVERTERS_H
#define TRACKEVENT_TRACKUNITSCONVERTERS_H 1

// Include files
// -------------
// from LHCbKernel
#include "Kernel/TrackTypes.h"

/** @namespace TrackUnitsConverters
 *
 * This namespace defines some useful units converters
 *
 * @author Eduardo Rodrigues
 * created 2006-02-10
 * 
 */

namespace TrackUnitsConverters 
{
  void convertToG3( Gaudi::TrackSymMatrix& C );
  void convertToG3( Gaudi::SymMatrix4x4& C );
  void convertToG4( Gaudi::TrackSymMatrix& invC );  
  void convertToG4( Gaudi::SymMatrix4x4& invC );
}

#endif /// TRACKEVENT_TRACKUNITSCONVERTERS_H
