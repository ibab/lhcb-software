
//---------------------------------------------------------------------------------
/** @file TrackTypes.h
 *
 *  Track based typedefs
 *
 *  CVS Log :-
 *  $Id: TrackTypes.h,v 1.1 2006-12-15 14:05:16 cattanem Exp $
 *
 *  @author M. Needham
 *  @date   2005-1-09
 */

#ifndef EVENT_TRACKTYPES_H
#define EVENT_TRACKTYPES_H 1
 
// Include files
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace Gaudi 
{

  // NB : Please remember to give a simple doxygen comment for each tyedef

  typedef Vector5 TrackVector;              ///< Vector type for Track
  typedef SymMatrix5x5 TrackSymMatrix;      ///< Symmetrix matrix type for Track
  typedef Matrix5x5 TrackMatrix;            ///< Matrix type for Track
  typedef Matrix1x5 TrackProjectionMatrix;  ///< Projection matrix type for Track

}
 
#endif //  EVENT_TRACKTYPES_H
