
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
#ifdef _WIN32
// Avoid conflict of Windows macro with std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include "GaudiKernel/boost_allocator.h"

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

  template <class T>
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
      return ( sizeof(T) == size ?
               boost::singleton_pool<T, sizeof(T)>::malloc() :
               ::operator new ( size ) );
    }

    /// placement operator new
    inline static void* operator new ( size_t size, void*& pObj )
    {
      return ::operator new ( size, pObj );
    }

    /// placement operator new
    inline static void* operator new ( size_t size, T*& pObj )
    {
      return ::operator new ( size, pObj );
    }

    /// Operator delete
    inline static void operator delete ( void* pObj )
    {
      boost::singleton_pool<T, sizeof(T)>::is_from(pObj)
        ? boost::singleton_pool<T, sizeof(T)>::free(pObj)
        : ::operator delete ( pObj );
    }

    /// placement operator delete
    inline static void operator delete ( void* p, void* pObj )
    {
      ::operator delete ( p, pObj );
    }

    /// placement operator delete
    inline static void operator delete ( void* p, T* pObj )
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
