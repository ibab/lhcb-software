
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

#if __cplusplus < 201100L
namespace LHCb_MemPoolAlloc_details 
{
  // implement the equivalent of C++11 std::conditional...
  template<bool B, class T, class F> struct conditional { typedef T type; };
  template<class T, class F> struct conditional<false, T, F> { typedef F type; };
}
#else
#include <type_traits>
#endif

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

  template <typename T, 
            unsigned NextSize = 32,  // when needed, allocate size for 'NextSize' T's at once.
            bool Mutex = false,      // do we need to protect access to the singleton with a mutex?
            typename Allocator = boost::default_user_allocator_new_delete>
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
#if __cplusplus < 201100L
      typedef boost::singleton_pool<T, sizeof(T),
                                         Allocator,
                                         typename LHCb_MemPoolAlloc_details::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                         NextSize> pool;
#else 
      using pool = boost::singleton_pool<T, sizeof(T),
                                         Allocator,
                                         typename std::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                         NextSize>;
#endif
      return ( sizeof(T) == size ?  pool::malloc() 
                                 : ::operator new ( size ) );
    }

    /// Operator delete
    inline static void operator delete ( void* pObj )
    {
#if __cplusplus < 201100L
      typedef boost::singleton_pool<T, sizeof(T),
                                         Allocator,
                                         typename LHCb_MemPoolAlloc_details::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                         NextSize> pool;
#else
      using pool = boost::singleton_pool<T, sizeof(T),
                                         Allocator,
                                         typename std::conditional<Mutex,boost::details::pool::default_mutex,boost::details::pool::null_mutex>::type,
                                         NextSize>;
#endif
      pool::is_from(pObj) ? pool::free(pObj) 
                          : ::operator delete ( pObj );
    }

#ifdef __INTEL_COMPILER // Re-enable ICC remark
#pragma warning(pop)
#endif

#endif

  };

}

#endif // LHCBKERNEL_MEMPOOLALLOC_H
