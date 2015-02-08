
//================================================================
/** @file BoostArray.h
 *
 *  Header file for boost::array + Additional Gaudi methods
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//================================================================

#ifndef RICHKERNEL_BOOSTARRAY_H 
#define RICHKERNEL_BOOSTARRAY_H 1

// Boost
//#define BOOST_DISABLE_ASSERTS
#include "boost/array.hpp"

// Gaudi
#include "GaudiKernel/MsgStream.h"

/// Implement textual MsgStream << method for boost::array's
template < class TYPE, std::size_t N >
inline MsgStream& operator << ( MsgStream& s, 
                                const boost::array<TYPE,N>& data )
{
  for ( const auto& i : data ) { s << i << " "; }
  return s;
}

#endif // RICHKERNEL_BOOSTARRAY_H
