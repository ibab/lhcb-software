
//--------------------------------------------------------------------------------
/** @file RichHashMap.h
 *
 *  Header file for utility has map for the RICH : Rich::HashMap
 *
 *  CVS Log :-
 *  $Id: RichHashMap.h,v 1.1 2005-01-13 12:21:18 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHASHMAP_H
#define RICHKERNEL_RICHHASHMAP_H 1

// GaudiKernel
#include "GaudiKernel/stl_hash.h"

/** @class RichHashMap RichHashMap.h RichKernel/RichHashMap.h
 *
 *  A utility class providing a templated HashMap for fast 
 *  loop up table like access.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */

template < class KEY, class VALUE >
class RichHashMap 
#ifdef __GNUC__
  : public __gnu_cxx::hash_map < KEY, VALUE,  __gnu_cxx::hash< KEY > >
#else
  : public __gnu_cxx::hash_map < KEY, VALUE >
#endif
{
};

#endif // RICHKERNEL_RICHHASHMAP_H
