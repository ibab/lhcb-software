// $Id: BoostArray.h,v 1.1 2003-08-26 14:44:11 jonrob Exp $
#ifndef RICHKERNEL_BOOSTARRAY_H 
#define RICHKERNEL_BOOSTARRAY_H 1

// Include files
#include "boost/array.hpp"

/// Implement textual MsgStream << method
template <class TYPE, std::size_t N>
inline MsgStream& operator << ( MsgStream& s, 
                                boost::array<TYPE, N>& data )
{
  for ( typename boost::array<TYPE, N>::const_iterator i = data.begin(); 
        i != data.end(); ++i ) { s << *i << " "; }
  return s;
}

#endif // RICHKERNEL_BOOSTARRAY_H
