
//================================================================
/** @file BoostArray.h
 *
 *  Header file for class Rich::BoostMemPoolAlloc
 *
 *  CVS Log :-
 *  $Id: BoostMemPoolAlloc.h,v 1.2 2005-11-12 19:11:52 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//================================================================

#ifndef RICHKERNEL_BOOSTPOOLALLOC_H
#define RICHKERNEL_BOOSTPOOLALLOC_H 1

#ifdef __GNUC__
// Pool from Boost
#include <boost/pool/pool.hpp>
#endif

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
   *
   *  @todo Figure out why this Boost utility fails to compile on windows
   */
  //-----------------------------------------------------------------------------

#ifdef __GNUC__
  // Linux, use Boost pool

  template <class T>
  class BoostMemPoolAlloc
  {

  public:

    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(T) == size ? 
               s_memPool.malloc() : ::operator new(size) );
    }

    /// operator delete
    static void operator delete ( void* p, size_t size )
    {
      if ( size == sizeof(T) ) { s_memPool.free(p);    }
      else                     { ::operator delete(p); }
    }

  private:

    /// The memory pool
    static boost::pool<> s_memPool;

  };

#else
  // All other platforms use null class
  
  template <class T>
  class BoostMemPoolAlloc { };
  
#endif

}

#ifdef __GNUC__
/// Allocate the static memory pool for the template class
template <class T>
boost::pool<> Rich::BoostMemPoolAlloc<T>::s_memPool( sizeof(T) );
#endif

#endif // RICHKERNEL_BOOSTPOOLALLOC_H
