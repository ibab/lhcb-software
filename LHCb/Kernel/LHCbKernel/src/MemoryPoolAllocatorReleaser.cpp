#include "Kernel/MemoryPoolAllocatorReleaser.h"

#include "GaudiKernel/MsgStream.h"

#include <boost/foreach.hpp>

LHCb::MemoryPoolAllocatorReleaser& LHCb::MemoryPoolAllocatorReleaser::instance() {
  static LHCb::MemoryPoolAllocatorReleaser releaser;
  return releaser;
}

void LHCb::MemoryPoolAllocatorReleaser::releaseMemory() {
  ReleaseFuncCollType &funcMap = instance().m_releaseFunctions;
  BOOST_FOREACH(ReleaseFuncCollType::value_type entry, funcMap) {
    entry.first();
  }
}

void LHCb::MemoryPoolAllocatorReleaser::releaseMemory(MsgStream &log) {
  ReleaseFuncCollType &funcMap = instance().m_releaseFunctions;
  // remember the level, so that we can print the heading in all lines
  const MSG::Level lvl = log.level();
  log << "Released " << funcMap.size() << " memory pools:" << endmsg;
  BOOST_FOREACH(ReleaseFuncCollType::value_type entry, funcMap) {
    entry.first();
    log << lvl << "  - " << entry.second << endmsg;
  }
}
