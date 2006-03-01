
//--------------------------------------------------------------------------------
/** @file RichHashMap.h
 *
 *  Header file for utility hash map for the RICH : RichHashMap
 *
 *  CVS Log :-
 *  $Id: RichHashMap.h,v 1.6 2006-03-01 09:57:25 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHASHMAP_H
#define RICHKERNEL_RICHHASHMAP_H 1

// GaudiKernel
#include "GaudiKernel/HashMap.h"

// Include here to make sure always use defined hash functions for RichSmartIDs
#include "Kernel/RichSmartIDHashFuncs.h"

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class HashMap RichHashMap.h RichKernel/RichHashMap.h
   *
   *  A utility class providing a templated HashMap for fast
   *  loop up table like access.
   *
   *  Provides a convenient single point of definition for optimisation studies.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

  template < class KEY, class VALUE >
  class HashMap : public GaudiUtils::HashMap < KEY, VALUE > { };

}

#endif // RICHKERNEL_RICHHASHMAP_H
