
//--------------------------------------------------------------------------------
/** @file FastAllocVector.h
 *
 *  Header file for vector class LHCb::FastAllocVector
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   29/03/2007
 */
//--------------------------------------------------------------------------------

#ifndef KERNEL_FastAllocVector_H
#define KERNEL_FastAllocVector_H 1

// STL
#include <vector>

#ifndef GOD_NOALLOC
// Memory pool allocator from boost
#include "GaudiKernel/boost_allocator.h"
#include <boost/pool/pool_alloc.hpp>
// MT allocator
#include <ext/mt_allocator.h>
#endif

namespace LHCb
{

  //--------------------------------------------------------------------------------
  /** @class FastAllocVector Kernel/FastAllocVector.h
   *
   *  Vector with custom allocator
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   23/09/2015
   */
  //--------------------------------------------------------------------------------
  
  template < typename TYPE, typename ALLOC >
  class FastAllocVector : public std::vector< TYPE, ALLOC >
  {

  public:

    /// Default constructor
    FastAllocVector( ) { }

    /** Constructor with initial size
     *  @param size Initialisation size for vector */
    FastAllocVector( const typename std::vector<TYPE,ALLOC>::size_type size )
      : std::vector<TYPE,ALLOC> (size) { }

    /** Constructor with initial size and initialisation value
     *  @param size Initialisation size for vector
     *  @param init Initialisation value
     */
    FastAllocVector( const typename std::vector<TYPE,ALLOC>::size_type size,
                     const TYPE & init ) : std::vector<TYPE,ALLOC>(size,init) { }

    /// Copy Constructor
    FastAllocVector( const FastAllocVector & init ) : std::vector<TYPE,ALLOC>(init) { }

    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const FastAllocVector<TYPE,ALLOC> & v )
    {
      str << "[ ";
      for ( const auto& i : v ) { str << i << " "; }
      return str << "]";
    }

  };

  //--------------------------------------------------------------------------------
  /** @typedef PoolAllocVector Kernel/FastAllocVector.h
   *
   *  Vector with pool allocator from boost
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   23/09/2015
   */
  //--------------------------------------------------------------------------------

  template < typename TYPE,
             typename USERALLOC = boost::default_user_allocator_new_delete,
             typename MUTEX     = boost::details::pool::null_mutex,
             unsigned NEXTSIZE  = 32 >
  using PoolAlloc = boost::pool_allocator< TYPE, USERALLOC, MUTEX, NEXTSIZE >;
                                              
  template < typename TYPE, 
#ifndef GOD_NOALLOC
             typename ALLOC = PoolAlloc< TYPE >
#else
             typename ALLOC = std::allocator< TYPE >
#endif
             >
  using PoolAllocVector = FastAllocVector<TYPE,ALLOC>;

  //--------------------------------------------------------------------------------
  /** @typedef MTAllocVector Kernel/FastAllocVector.h
   *
   *  Vector with MT allocator
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   23/09/2015
   */
  //--------------------------------------------------------------------------------
  
  template < typename TYPE, 
#ifndef GOD_NOALLOC
             typename ALLOC = __gnu_cxx::__mt_alloc< TYPE >
#else
             typename ALLOC = std::allocator< TYPE >
#endif
             >
  using MTAllocVector = FastAllocVector<TYPE,ALLOC>;

}

#endif // KERNEL_FastAllocVector_H
