
/** @file BoostArray.h
 *
 *  Header file for boost::array + Additional Gaudi methods
 *
 *  CVS Log :-
 *  $Id: BoostArray.h,v 1.4 2004-09-23 07:01:55 cattanem Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/08/19 13:55:58  jonrob
 *  Add new method to MC truth tool + various other things after CVS server switch
 *
 *  Revision 1.2  2004/07/26 17:53:16  jonrob
 *  Various improvements to the doxygen comments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */

#ifndef RICHKERNEL_BOOSTARRAY_H 
#define RICHKERNEL_BOOSTARRAY_H 1

// Include files
#include "boost/array.hpp"
#include "GaudiKernel/MsgStream.h"

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
