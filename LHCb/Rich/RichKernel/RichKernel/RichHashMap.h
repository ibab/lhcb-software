
//--------------------------------------------------------------------------------
/** @file RichHashMap.h
 *
 *  Header file for utility hash map for the RICH : RichHashMap
 *
 *  CVS Log :-
 *  $Id: RichHashMap.h,v 1.3 2005-03-06 13:02:34 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHASHMAP_H
#define RICHKERNEL_RICHHASHMAP_H 1

//--------------------------------------------------------------------------------
/** @class RichHashMap RichHashMap.h RichKernel/RichHashMap.h
 *
 *  A utility class providing a templated HashMap for fast 
 *  loop up table like access.
 *
 *  @attention
 *  Hash maps are not currently a part of the standard stl, only the stl
 *  extensions. Also on windows, the hash map appears to be less efficient 
 *  than the standard map, so this is used instead. 
 *  To be reviewed as the stl hash maps evolve.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 *
 *  @todo Keep an eye on the underlying hash map implementations for both 
 *        linux and windows
 */
//--------------------------------------------------------------------------------

#ifdef __GNUC__
// Linux : Use std extension hash map

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

template < class KEY, class VALUE >
class RichHashMap
  : public __gnu_cxx::hash_map < KEY, VALUE, __gnu_cxx::hash< KEY > > { };

#else
// All other platforms use standard stl map

// STD
#include <map>

template < class KEY, class VALUE >
class RichHashMap
  : public std::map < KEY, VALUE > { };
//: public __gnu_cxx::hash_map < KEY, VALUE, __gnu_cxx::hash_compare< KEY > > { };

#endif

#endif // RICHKERNEL_RICHHASHMAP_H
