// Framework includes
#include "Framework/IExecutor.h"

using namespace Framework;

/// Initializing constructor
Executor::Executor(ExecutorFactory& factory, InstanceMask& mask, size_t serial)
: m_fac(factory), m_mask(mask), m_serial(serial)
{
}

/// Save monitoring information
void Executor::monitor(const struct timeval& start, const struct timeval& stop) {
  m_time.collect(start,stop);
}
