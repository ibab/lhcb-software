/** @file BoostArray.h
 *
 *  Header file for boost::array + Additional Gaudi methods
 *
 *  CVS Log :-
 *  $Id: BoostArray.h,v 1.2 2004-07-26 17:53:16 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

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
