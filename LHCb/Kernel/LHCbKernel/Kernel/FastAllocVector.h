
//--------------------------------------------------------------------------------
/** @file FastAllocVector.h
 *
 *  Header file for vector class LHCb::FastAllocVector
 *
 *  CVS Log :-
 *  $Id: FastAllocVector.h,v 1.3 2010-05-26 09:01:47 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   29/03/2007
 */
//--------------------------------------------------------------------------------

#ifndef KERNEL_FastAllocVector_H
#define KERNEL_FastAllocVector_H 1

// Include files
#include <vector>

// need to use macro as template typedefs don't work yet :(

// Check if memory pools are isabled completely
#ifndef GOD_NOALLOC

#ifdef __GNUC__
#if __GNUC__ > 3
// This is GCC 4 and above. Use the allocators
#include <ext/mt_allocator.h>
#define LHCb_FastAllocVector_allocator(TYPE) __gnu_cxx::__mt_alloc< TYPE >
#else
#if __GNUC_MINOR__ > 3
// This is gcc 3.4.X so has the custom allocators
#include <ext/mt_allocator.h>
#define LHCb_FastAllocVector_allocator(TYPE) __gnu_cxx::__mt_alloc< TYPE >
//#include <ext/pool_allocator.h>
//#define LHCb_FastAllocVector_allocator(TYPE) __gnu_cxx::__pool_alloc< TYPE >
//#define LHCb_FastAllocVector_allocator(TYPE) std::allocator< TYPE >
#else
// This is older than gcc 3.4.X so use standard allocator
#define LHCb_FastAllocVector_allocator(TYPE) std::allocator< TYPE >
#endif
#endif
#else
// Not GNUC, so disable allocators 
#define LHCb_FastAllocVector_allocator(TYPE) std::allocator< TYPE >
#endif

#else

// GOD NOALLOC - Disable memory pools completely
#define LHCb_FastAllocVector_allocator(TYPE) std::allocator< TYPE >

#endif

namespace LHCb
{

  //--------------------------------------------------------------------------------
  /** @class FastAllocVector FastAllocVector.h Kernel/FastAllocVector.h
   *
   *  Vector with fast allocator
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
      for ( typename FastAllocVector<TYPE,ALLOC>::const_iterator i = v.begin(); i != v.end(); ++i )
      { str << *i << " "; }
      return str << "]";
    }

  };

}

#endif // KERNEL_FastAllocVector_H
