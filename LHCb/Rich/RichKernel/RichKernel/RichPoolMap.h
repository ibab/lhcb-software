
//--------------------------------------------------------------------------------
/** @file RichPoolMap.h
 *
 *  Header file for utility has map for the RICH : Rich::Map
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHPOOLMAP_H
#define RICHKERNEL_RICHPOOLMAP_H 1

// STL
#include <map>
#include <ostream>

// Traits
#include <type_traits>

#ifndef GOD_NOALLOC
// boost
#include "GaudiKernel/boost_allocator.h"
#include <boost/pool/pool_alloc.hpp>
#endif

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class PoolMap RichKernel/RichPoolMap.h
   *
   *  A utility class providing a standard std::map like object using a boost
   *  pool allocator.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

#ifndef GOD_NOALLOC
  template < typename KEY,
             typename VALUE, 
             typename USERALLOC = boost::default_user_allocator_new_delete,
             typename MUTEX     = boost::details::pool::null_mutex,
             unsigned NEXTSIZE  = 32 >
  using MapPoolAlloc = boost::fast_pool_allocator< std::pair<const KEY,VALUE>, 
                                                   USERALLOC, MUTEX, NEXTSIZE >;
#else
  template < typename KEY, typename VALUE >
  using MapPoolAlloc = std::allocator< std::pair<const KEY,VALUE> >;
#endif

  template < typename KEY, 
             typename VALUE, 
             typename COMPARE = std::less<KEY>,
             typename ALLOC = MapPoolAlloc<KEY,VALUE>
             >
  class PoolMap : public std::map < KEY , VALUE , COMPARE , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const PoolMap<KEY,VALUE,COMPARE,ALLOC> & m )
    {
      str << "[";
      for ( const auto& i : m ) { str << " (" << i.first << "," << i.second << ")"; }
      return str << " ]";
    }
  };

}

#endif // RICHKERNEL_RICHPOOLMAP_H
