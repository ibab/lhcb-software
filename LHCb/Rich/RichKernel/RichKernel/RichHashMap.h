
//--------------------------------------------------------------------------------
/** @file RichHashMap.h
 *
 *  Header file for utility has map for the RICH : Rich::HashMap
 *
 *  CVS Log :-
 *  $Id: RichHashMap.h,v 1.2 2005-03-05 16:52:17 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHASHMAP_H
#define RICHKERNEL_RICHHASHMAP_H 1

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

// STD
#include <map>

//--------------------------------------------------------------------------------
/** @class RichHashMap RichHashMap.h RichKernel/RichHashMap.h
 *
 *  A utility class providing a templated HashMap for fast 
 *  loop up table like access.
 *
 *  @attention
 *  On windows, implementation uses a standard std::map as this appears
 *  to be faster. This might be a feature of the debug libraries. 
 *  To be reviewed if optimised libraries are ever used on windows
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 *
 *  @todo Keep an eye on the windows implementation
 */
//--------------------------------------------------------------------------------

template < class KEY, class VALUE >
class RichHashMap
#ifdef __GNUC__
  // Linux : Use std extension hash map
  : public __gnu_cxx::hash_map < KEY, VALUE, __gnu_cxx::hash< KEY > >
#else
  // For windows, using normal map as it appears to be faster
  //: public stdext::hash_map < KEY, VALUE, stdext::hash_compare< KEY > >
  : public std::map < KEY, VALUE >
#endif
{
};

#endif // RICHKERNEL_RICHHASHMAP_H
