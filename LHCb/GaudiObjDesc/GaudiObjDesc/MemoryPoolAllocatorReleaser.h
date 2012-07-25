#ifndef GOD_MEMORY_POOL_ALLOCATOR_RELEASER_H
#define GOD_MEMORY_POOL_ALLOCATOR_RELEASER_H

#include <map>
#include <string>

#ifdef DEBUG_MEMORY_POOL_ALLOCATOR_RELEASER
#include <iostream>
#endif

#include "GaudiKernel/System.h"

class MsgStream;

namespace Gaudi {
  /** Class used to collect the functions to call to free the memory used by Boost
   *  pool allocators.
   *
   *  GaudiObjDesc must be instrumented to register (via LHCb::RegisterReleaseFunction)
   *  the classes that use a Boost pool allocator. Those classes must also
   *  implement a static method <code>void release_pool()</code> which call the
   *  pool release function.
   *
   *  Users will have to call <code>MemoryPoolAllocatorReleaser::releaseMemory()</code>,
   *  or <code>MemoryPoolAllocatorReleaser::releaseMemory(msgStream)</code>,
   *  when needed.
   *
   *  @author Marco Clemencic
   */
  class MemoryPoolAllocatorReleaser {
  public:
    /// Call all the registered pool release functions.
    static void releaseMemory();

    /// Call all the registered pool release functions printing log messages
    /// through a Gaudi MsgStream.
    ///
    /// \note
    /// it is up to the user to set the required level for the messages before
    /// passing the MsgStream to the function.
    static void releaseMemory(MsgStream &log);

  private:
    /// Private constructor to have a singleton.
    MemoryPoolAllocatorReleaser() {}
    /// Return the instance of the singleton.
    static MemoryPoolAllocatorReleaser& instance();

    /// Prototype of the release functions: <code>void f()</code>.
    typedef bool (*ReleaseFunctionType) ();
    /// Typedef for the collection of release functions.
    typedef std::map<ReleaseFunctionType, std::string> ReleaseFuncCollType;

    /// Collection of release functions to call.
    ReleaseFuncCollType m_releaseFunctions;

    /// Friend class to register the release functions.
    template <class T>
    friend class RegisterReleaseFunction;
  };

  /** Class used to register classes implementing the method <code>void release_pool()</code>.
   *
   *  In the header file of the class, or in a source file, there must be a static
   *  instance of this class. For example:
   *  @code
   *  class MyClass {
   *  public:
   *    // ...
   *    static void release_pool() {
   *      // free the memory
   *    }
   *  };
   *
   *  namespace {
   *    static LHCb::RegisterReleaseFunction<MyClass> _MyClass_releaser;
   *  }
   *  @endcode
   *  The static instance will be constructed by the library loader, triggering the
   *  insertion of the release function to BoostAllocatorReleaser.
   */
  template <class T>
  class RegisterReleaseFunction {
  public:
    /// Register the release function of the template argument class.
    RegisterReleaseFunction() {
      typedef MemoryPoolAllocatorReleaser bar;
      bar::ReleaseFunctionType f = &T::release_pool;
      bar &releaser = MemoryPoolAllocatorReleaser::instance();
      bar::ReleaseFuncCollType &funcMap = releaser.m_releaseFunctions;
      if (funcMap.find(f) == funcMap.end()) {
        funcMap[f] = System::typeinfoName(typeid(T));
#ifdef DEBUG_MEMORY_POOL_ALLOCATOR_RELEASER
        std::cout << "RegisterReleaseFunction for "
                  << System::typeinfoName(typeid(T))
                  << " count: " << funcMap.size() << std::endl;
#endif
      }
    }
  };
}


#endif /* GOD_MEMORY_POOL_ALLOCATOR_RELEASER_H */
