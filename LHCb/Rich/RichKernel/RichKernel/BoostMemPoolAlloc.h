
//================================================================
/** @file BoostMemPoolAlloc.h
 *
 *  Header file for class Rich::BoostMemPoolAlloc
 *
 *  CVS Log :-
 *  $Id: BoostMemPoolAlloc.h,v 1.6 2007-04-18 10:04:08 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//================================================================

#ifndef RICHKERNEL_BOOSTPOOLALLOC_H
#define RICHKERNEL_BOOSTPOOLALLOC_H 1

// boost stuff from Gaudi
#include "GaudiKernel/boost_allocator.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class BoostMemPoolAlloc BoostMemPoolAlloc.h RichKernel/BoostMemPoolAlloc.h
   *
   *  Templated classes providing an interface to the boost memory pool management.
   *
   *  Usage is simply via inheritance, i.e.
   *
   *  @code
   *
   *  // class with standard new/delete allocation
   *  class MyClass { ... };
   *
   *  // class with boost memory allocation
   *  class MyClass : public Rich::BoostMemPoolAlloc<MyClass> { ... } ;
   *
   *  @endcode
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   07/11/2005
   */
  //-----------------------------------------------------------------------------

  template <class T>
  class BoostMemPoolAlloc
  {

  public:

    // on windows, do nothing
#ifndef _WIN32

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

    /// placement operator new[]
    inline static void* operator new[] ( size_t size, void*& pObj )
    {
      return ::operator new[] ( size, pObj );
    }

    /// Operator delete
    inline static void operator delete ( void* pObj )
    {
      if ( boost::singleton_pool<T, sizeof(T)>::is_from(pObj) )
      { boost::singleton_pool<T, sizeof(T)>::free(pObj); }
      else
      { ::operator delete ( pObj ); }
    }

    /// operator delete
    inline static void operator delete ( void* pObj, size_t size )
    {
      if ( sizeof(T) == size )
      { boost::singleton_pool<T, sizeof(T)>::free(pObj); }
      else
      { ::operator delete ( pObj ); }
    }

    /// placement operator delete (needed to avoid a warning on win32)
    inline static void operator delete ( void* p, void* pObj )
    {
      ::operator delete ( p, pObj );
    }

#endif

  };

}

#endif // RICHKERNEL_BOOSTPOOLALLOC_H
