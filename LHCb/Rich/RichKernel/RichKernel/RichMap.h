
//--------------------------------------------------------------------------------
/** @file RichMap.h
 *
 *  Header file for utility has map for the RICH : Rich::HashMap
 *
 *  CVS Log :-
 *  $Id: RichMap.h,v 1.4 2006-01-23 13:48:35 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMAP_H
#define RICHKERNEL_RICHMAP_H 1

// STL
#include <map>

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class Map RichMap.h RichKernel/RichMap.h
   *
   *  A utility class providing a standard std::map like object.
   *
   *  Provides a convenient single point of definition for optimisation studies.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

  template < class KEY, class VALUE >
  class Map : public std::map < KEY , VALUE > { };

}

#endif // RICHKERNEL_RICHMAP_H
