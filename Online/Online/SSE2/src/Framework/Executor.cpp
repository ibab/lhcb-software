#include "Framework/IExecutor.h"

using namespace Framework;

/// Initializing constructor
Executor::Executor(ExecutorFactory* factory, InstanceMask& mask, size_t serial)
  : m_fac(factory), m_mask(mask), m_serial(serial)
{
}

/// Default destructor
Executor::~Executor()   {
}

/// Release instance. If the refeence count reaches NULL, delete the object
unsigned long Executor::release()   {
  delete this;
  return 0;
}
