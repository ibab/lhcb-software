// Framework include files
#include "RTL/rtl.h"
#include "Framework/Kernel.h"
#include "Framework/Helpers.h"
#include "Framework/IExecutor.h"

// C++ include files
#include <cmath>
#include <cstring>

using namespace Framework;
using namespace std;

/// Initializing constructor
ExecutorFactory::ExecutorFactory(size_t max_instances, const string& nam, const void* par) 
  : m_name(nam), m_param(par), m_maxInstances(max_instances), m_instances(0)
{
}

/// Default destructor
ExecutorFactory::~ExecutorFactory()  {
  deleteContainer(m_executors);
}

/// Initialize the factory
int ExecutorFactory::initialize()   throw (std::runtime_error)  {
  throw std::runtime_error("ExecutorFactory::initialize> FAILED to create instance.");
}

/// Shutdown the whole stuff
void ExecutorFactory::shutdown(bool delete_instances) {
  if ( delete_instances ) deleteContainer(m_executors);
}

/// Access to time statistics 
ExecutorFactory::Statistics ExecutorFactory::statistics()  const {
  Timing t;
  for(Executors::const_iterator i=m_executors.begin(); i != m_executors.end(); ++i)
    t += (*i)->timing();
  return make_pair(m_executors.size(),t);
}

/// Print statistics
void ExecutorFactory::print(int level, const Statistics& stat)  const {
  ::lib_rtl_output(level," %-36s    %10.2f +- %10.2f seconds  [%ld instances/%ld calls]",
		   m_name.c_str(), stat.second.mean(), stat.second.sigma(),
		   long(stat.first), long(stat.second.numCalls()));
}

/// Print statistics
void ExecutorFactory::printStatistics(int level)  const {
  print(level,statistics());
}

/// Set the input mask bits
void ExecutorFactory::setInputMask(const BitRegisters& bits)  {
  m_inputMask.assign(bits);
}

/// Set the output mask bits
void ExecutorFactory::setOutputMask(const BitRegisters& bits)  {
  m_outputMask.assign(bits);
}

/// Setup internal processor and run
Executor* ExecutorFactory::getFreeInstance()  {
  size_t bit = m_idle.first_set();
  if ( bit != size_t(InstanceMask::NO_BIT_SET) )  {
    m_idle.clear_bit(bit);
    return m_executors[bit];
  }
  else if ( m_instances < m_maxInstances )  {
    Executor* e = createInstance();
    if ( e ) ++m_instances;
    m_executors.push_back(e);
    return e;
  }
  return 0;
}
