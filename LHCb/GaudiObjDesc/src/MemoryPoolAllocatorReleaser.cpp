#include "GaudiObjDesc/MemoryPoolAllocatorReleaser.h"

#include "GaudiKernel/MsgStream.h"

#include <boost/foreach.hpp>

Gaudi::MemoryPoolAllocatorReleaser& Gaudi::MemoryPoolAllocatorReleaser::instance() {
  static Gaudi::MemoryPoolAllocatorReleaser releaser;
  return releaser;
}

void Gaudi::MemoryPoolAllocatorReleaser::releaseMemory() {
  ReleaseFuncCollType &funcMap = instance().m_releaseFunctions;
  BOOST_FOREACH(ReleaseFuncCollType::value_type entry, funcMap) {
    entry.first();
  }
}

void Gaudi::MemoryPoolAllocatorReleaser::releaseMemory(MsgStream &log) {
  ReleaseFuncCollType &funcMap = instance().m_releaseFunctions;
  // remember the level, so that we can print the heading in all lines
  const MSG::Level lvl = log.level();
  log << "Released " << funcMap.size() << " memory pools:" << endmsg;
  BOOST_FOREACH(ReleaseFuncCollType::value_type entry, funcMap) {
    log << lvl << "  - " << entry.second
    << ((entry.first()) ? " (freed)" : "")
    << endmsg;
  }
}
