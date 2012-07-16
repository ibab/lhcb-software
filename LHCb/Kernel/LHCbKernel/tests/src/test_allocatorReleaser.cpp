/*
 * test_allocatorReleaser.cpp
 *
 *  Created on: 16/jul/2012
 *      Author: Marco Clemencic
 */
#define BOOST_TEST_MODULE BoostAllocatorReleaser test
// header-only Boost test framework
#include <boost/test/included/unit_test.hpp>
// requires library "unit_test_framework"
// #include <boost/test/unit_test.hpp>

#include "GaudiKernel/MsgStream.h"

// enable debug output in BoostAllocatorReleaser
#define DEBUG_MEMORY_POOL_ALLOCATOR_RELEASER
#include "Kernel/MemoryPoolAllocatorReleaser.h"

/// Dummy event model class to test the release method
class EventModelClass {
public:
  /// Method required by the BoostAllocatorReleaser protocol.
  static void release_pool() {
    // increase the counter of calls to this method
    ++call_count;
  }
  /// Counter for the calls to the release_pool method.
  static int call_count;
};
int EventModelClass::call_count = 0;

/// Dummy event model class to test the release method
class AnotherClass {
public:
  /// Method required by the BoostAllocatorReleaser protocol.
  static void release_pool() {
    // increase the counter of calls to this method
    ++call_count;
  }
  /// Counter for the calls to the release_pool method.
  static int call_count;
};
int AnotherClass::call_count = 0;

BOOST_AUTO_TEST_CASE( test )
{

  // Register the two event model classes
  LHCb::RegisterReleaseFunction<EventModelClass> r1;
  LHCb::RegisterReleaseFunction<AnotherClass>    r2;
  // Register one of them again to check the correct handling of duplicates
  LHCb::RegisterReleaseFunction<EventModelClass> r1b;

  // ensure that the counters are cleared
  EventModelClass::call_count = 0;
  AnotherClass::call_count = 0;

  // call the release_pool methods
  LHCb::MemoryPoolAllocatorReleaser::releaseMemory();

  // check that the counts are correct
  BOOST_CHECK_EQUAL(EventModelClass::call_count, 1);
  BOOST_CHECK_EQUAL(AnotherClass::call_count, 1);

  MsgStream log(0, "test");
  // try once more, with logging
  log << MSG::INFO;
  LHCb::MemoryPoolAllocatorReleaser::releaseMemory(log);

  // check that the counts are correct
  BOOST_CHECK_EQUAL(EventModelClass::call_count, 2);
  BOOST_CHECK_EQUAL(AnotherClass::call_count, 2);
}
