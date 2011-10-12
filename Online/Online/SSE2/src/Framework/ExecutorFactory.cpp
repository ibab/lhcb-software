// Framework include files
#include "RTL/rtl.h"
#include "Framework/Kernel.h"
#include "Framework/IExecutor.h"

// C++ include files
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace Framework;
using namespace std;

/// Initializing constructor
ExecutorFactory::ExecutorFactory(size_t max_instances, const string& nam, const void* par) 
  : m_name(nam), m_param(par), m_maxInstances(max_instances), m_instances(0)
{
}

/// Default destructor
ExecutorFactory::~ExecutorFactory()  {
  for(Executors::iterator i=m_executors.begin(); i != m_executors.end(); ++i)
    delete *i;
  m_executors.clear();
}

/// Debug printout
void ExecutorFactory::Print::operator()(const ExecutorFactory* f)  {
  cout << "    -->  " << left << setw(24) << f->name() << hex;
  f->inputMask().dump("Input ");
  cout << "         " << left << setw(24) << "" << hex;
  f->outputMask().dump("Output");
}

/// Debug printout
void ExecutorFactory::PrintMissing::operator()(const ExecutorFactory* f)  {
  vector<size_t> bits=_mask.missing(f->inputMask());
  cout << "    -->  " << left << setw(24) << f->name() << hex;
  f->inputMask().dump("Input ");
  cout << "         " << left << setw(24) << "Missing bits:" << dec;
  for(size_t i=0; i<bits.size();++i) 
    cout << " " << bits[i];
  cout << endl
       << "         " << left << setw(24) << "" << hex;
  f->outputMask().dump("Output");
}

/// Initialize the factory
int ExecutorFactory::initialize()   throw (std::runtime_error)  {
  throw std::runtime_error("ExecutorFactory::initialize> FAILED to create instance.");
}

/// Shutdown the whole stuff
void ExecutorFactory::shutdown(bool delete_instances) {
  if ( delete_instances ) {
    for(Executors::iterator i=m_executors.begin(); i != m_executors.end(); ++i)
      delete *i;
    m_executors.clear();
  }
}

/// Print statistics
void ExecutorFactory::printStatistics()  const {
  double time=0e0, time2=0e0, calls=0e0, err=0e0;
  for(Executors::const_iterator i=m_executors.begin(); i != m_executors.end(); ++i) {
    const Timing& t = (*i)->timing();
    calls += double(t.numCalls());
    time2 += t.totalTimeSqr();
    time  += t.totalTime();
  }
  if ( calls>0.9 ) {
    time2 /= calls;
    time  /= calls;
    err = std::sqrt(std::fabs(time2-time*time));
  }
  ::lib_rtl_output(LIB_RTL_ALWAYS," %-36s    %10.2f +- %10.2f seconds  [%ld instances/%ld calls]\n",
		   m_name.c_str(), time, err, long(m_executors.size()),long(calls));
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
