
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
// Use a memory pool allocator from boost

// boost
#include "GaudiKernel/boost_allocator.h"
#include <boost/pool/pool_alloc.hpp>

// Boost pool allocator, no mutex
#define LHCb_FastAllocVector_allocator(TYPE) boost::pool_allocator< TYPE, boost::default_user_allocator_new_delete, boost::details::pool::null_mutex, 32 >

// Boost pool allocator, with mutex
//#define LHCb_FastAllocVector_allocator(TYPE) boost::pool_allocator< TYPE, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex, 32 >

#else
// GOD NOALLOC - Disable memory pools completely

#define LHCb_FastAllocVector_allocator(TYPE) std::allocator< TYPE >

#endif


namespace LHCb
{

  //--------------------------------------------------------------------------------
  /** @class FastAllocVector FastAllocVector.h Kernel/FastAllocVector.h
   *
   *  Vector with (fast) pool allocator from boost
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   29/03/2007
   */
  //--------------------------------------------------------------------------------
  
  template < typename TYPE, typename ALLOC = LHCb_FastAllocVector_allocator(TYPE) >
  class FastAllocVector : public std::vector<TYPE,ALLOC>
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
                     const TYPE & init ) : std::vector<TYPE,ALLOC> (size,init) { }

    /// Copy Constructor
    FastAllocVector( const FastAllocVector & init ) : std::vector<TYPE,ALLOC> (init) { }

    /// Operator overloading for ostream
    friend inline std::ostream& operator << ( std::ostream& str ,
                                              const FastAllocVector<TYPE,ALLOC> & v )
    {
      str << "[ ";
      for ( const auto& i : v ) { str << i << " "; }
      return str << "]";
    }

  };

}

#endif // KERNEL_FastAllocVector_H
