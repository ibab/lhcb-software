// Framework include files
#include "RTL/rtl.h"
#include "Framework/Kernel.h"
#include "Framework/IExecutor.h"

// C++ include files
#include <iostream>
#include <iomanip>

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
