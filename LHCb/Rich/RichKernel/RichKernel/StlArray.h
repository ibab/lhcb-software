
//================================================================
/** @file StlArray.h
 *
 *  Header file for std::array + Additional Gaudi methods
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2015-02-07
 */
//================================================================

#ifndef RICHKERNEL_STLARRAY_H 
#define RICHKERNEL_STLARRAY_H 1

// STL
#include <array>

// Gaudi
#include "GaudiKernel/MsgStream.h"

/// Implement textual MsgStream << method for boost::array's
template < class TYPE, std::size_t N >
inline MsgStream& operator << ( MsgStream& s, 
                                const std::array<TYPE,N>& data )
{
  for ( const auto& i : data ) { s << i << " "; }
  return s;
}

#endif // RICHKERNEL_STLARRAY_H
