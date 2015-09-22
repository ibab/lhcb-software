
//--------------------------------------------------------------------------------
/** @file RichMap.h
 *
 *  Header file for utility has map for the RICH : Rich::Map
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-11
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHMAP_H
#define RICHKERNEL_RICHMAP_H 1

// STL
#include <map>
#include <unordered_map>
#include <ostream>

#ifndef GOD_NOALLOC
// boost
#include "GaudiKernel/boost_allocator.h"
#include <boost/pool/pool_alloc.hpp>
#endif

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class Map RichMap.h RichKernel/RichMap.h
   *
   *  A utility class providing a standard std::map like object.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

  template < typename KEY, 
             typename VALUE, 
             typename COMPARE = std::less<KEY>,
             typename ALLOC   = std::allocator<std::pair<const KEY,VALUE> > >
  class Map : public std::map < const KEY , VALUE , COMPARE , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator <<
    ( std::ostream& str ,
      const Map<const KEY,VALUE,COMPARE,ALLOC> & map )
    {
      str << "[";
      for ( const auto& i : map ) { str << " (" << i.first << "," << i.second << ")"; }
      return str << " ]";
    }
  };

  //--------------------------------------------------------------------------------
  /** @class UnorderedMap RichMap.h RichKernel/RichMap.h
   *
   *  A utility class providing a standard std::unordered_map like object.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2015-09-22
   */
  //--------------------------------------------------------------------------------

  template < typename KEY, 
             typename VALUE, 
             typename HASH  = std::hash<KEY>,
             typename EQUAL = std::equal_to<KEY>,
             typename ALLOC = std::allocator<std::pair<const KEY,VALUE> > >
  class UnorderedMap : public std::unordered_map < const KEY , VALUE , HASH , EQUAL , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator << 
    ( std::ostream& str ,
      const UnorderedMap<const KEY,VALUE,HASH,EQUAL,ALLOC> & map )
    {
      str << "[";
      for ( const auto& i : map ) { str << " (" << i.first << "," << i.second << ")"; }
      return str << " ]";
    }
  };

  //--------------------------------------------------------------------------------
  /** @class PoolMap RichMap.h RichKernel/RichMap.h
   *
   *  A utility class providing a standard std::map like object using a boost
   *  pool allocator.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2005-01-11
   */
  //--------------------------------------------------------------------------------

  template < typename KEY, 
             typename VALUE, 
             typename COMPARE = std::less<KEY>,
#ifndef GOD_NOALLOC
             typename ALLOC   = boost::fast_pool_allocator< std::pair<const KEY,VALUE>, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex, 32 >
#else
             typename ALLOC   = std::allocator<std::pair<const KEY,VALUE> >
#endif
             >
  class PoolMap : public std::map < const KEY , VALUE , COMPARE , ALLOC >
  {
  public:
    /// Operator overloading for ostream
    friend inline std::ostream& operator <<
    ( std::ostream& str ,
      const PoolMap<const KEY,VALUE,COMPARE,ALLOC> & map )
    {
      str << "[";
      for ( const auto& i : map ) { str << " (" << i.first << "," << i.second << ")"; }
      return str << " ]";
    }
  };

}

#endif // RICHKERNEL_RICHMAP_H
