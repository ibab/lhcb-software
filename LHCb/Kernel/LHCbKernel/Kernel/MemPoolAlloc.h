
//================================================================
/** @file MemPoolAlloc.h
 *
 *  Header file for class LHCb::MemPoolAlloc
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//================================================================

#ifndef LHCBKERNEL_MEMPOOLALLOC_H
#define LHCBKERNEL_MEMPOOLALLOC_H 1

// boost stuff from Gaudi
#include "GaudiKernel/boost_allocator.h"

// Traits
#include <type_traits>

namespace LHCb
{

  //-----------------------------------------------------------------------------
  /** @class MemPoolAlloc MemPoolAlloc.h Kernel/MemPoolAlloc.h
   *
   *  Templated class providing an interface to the boost memory pool management.
   *
   *  Usage is simply via inheritance, i.e.
   *
   *  @code
   *
   *  // class with standard new/delete allocation
   *  class MyClass { ... };
   *
   *  // class with boost memory allocation
   *  class MyClass : public LHCb::MemPoolAlloc<MyClass> { ... } ;
   *
   *  @endcode
   *
   *  @todo Figure out how to avoid the need to disable an ICC remark with #pragma
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   07/11/2005
   */
  //-----------------------------------------------------------------------------

  template <typename T,              /// The type
            unsigned NextSize = 32,  /// When needed, allocate size for 'NextSize' T's at once.
            bool Mutex = false,      /// Do we need to protect access to the singleton with a mutex?
            typename Allocator = boost::default_user_allocator_new_delete /// The allocator
            >
  class MemPoolAlloc
  {

  public:

    /// If GOD_NOALLOC flag set, do nothing
#ifndef GOD_NOALLOC

#ifdef __INTEL_COMPILER         // Disable ICC remark
#pragma warning(disable:1720) // Operator new has no corresponding member operator delete (to be called if an exception is thrown during initialization of an allocated object)
#pragma warning(push)
#endif

    /// operator new
    inline static void* operator new ( size_t size )
    {
      using pool = boost::singleton_pool< T, sizeof(T),
                                          Allocator,
                                          typename std::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                          NextSize >;
      return ( sizeof(T) == size ?  pool::malloc() : ::operator new ( size ) );
    }

    /// Operator delete
    inline static void operator delete ( void* pObj )
    {
      using pool = boost::singleton_pool<T, sizeof(T),
                                         Allocator,
                                         typename std::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                         NextSize >;
      pool::is_from(pObj) ? pool::free(pObj) : ::operator delete ( pObj );
    }

    /// placement operator new
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new ( size, pObj );
    }

    /// placement operator delete
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete ( p, pObj );
    }

#ifdef __INTEL_COMPILER // Re-enable ICC remark
#pragma warning(pop)
#endif

#endif

  };

}

#endif // LHCBKERNEL_MEMPOOLALLOC_H
