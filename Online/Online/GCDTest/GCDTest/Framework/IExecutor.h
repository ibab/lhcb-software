#ifndef FRAMEWORK_IEXECUTOR_H
#define FRAMEWORK_IEXECUTOR_H

// Framework include files
#include "Framework/Status.h"
#include "Framework/IOPort.h"
#include "Framework/Timing.h"

// C++ include files
#include <stdexcept>
#include <vector>
#include <string>

/*
*    Framework namespace declaration
*/
namespace Framework {

  // Forward declarations
  class ExecutorFactory;
  class EventContext;
  class Executor;
  class Monitor;

  /**@class ExecutorFactory ExecutorFactory.h Framework/ExecutorFactory.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011 
  *
  */
  class ExecutorFactory   {

  public:
    /// Startup parameter definition
    typedef const void*              ClientData;
    /// Defintiion of the executor container
    typedef std::vector<Executor*>   Executors;
    /// Statistics type
    typedef std::pair<size_t,Timing> Statistics;

  protected:
    /** Internal member variables.             */
    /// Input IO mask
    IOMask                  m_inputMask;
    /// Output IO mask
    IOMask                  m_outputMask;
    /// Executor mask with idle instances
    InstanceMask            m_idle;
    /// Executor factory name (=name of created instances)
    std::string             m_name;
    /// Creation parameter
    ClientData              m_param;
    /// Executor type identifier
    int                     m_id;

    /** Creation and bookkeeping of executors. */
    /// Maximum number of object instances allowed
    size_t                  m_maxInstances;
    /// Number of instances created
    size_t                  m_instances;
    /// Container with executor instances, which may be chosen for work
    Executors               m_executors;

  protected:
    /// Create and setup internal processor: overload for concrete factory implementations
    virtual Executor* createInstance() = 0;

  public:
    /// Initializing constructor
    ExecutorFactory(size_t max_instances, const std::string& name, ClientData param);
    /// Default destructor
    virtual ~ExecutorFactory();
    /// Access  the identifier
    int id() const                     {   return m_id;           }
    /// Set executor identifier
    void setID(int new_id)             {   m_id = new_id;         }
    /// Access to the algorithm factory name
    const std::string& name() const    {   return m_name;         }
    /// Access to the construction parameter structure
    ClientData param() const           {   return m_param;        }
    /// Access the maximum number of instances
    size_t maxInstances()  const       {   return m_maxInstances; }
    /// Input event data mask
    const IOMask& inputMask() const    {   return m_inputMask;    }
    /// Output event data mask
    const IOMask& outputMask() const   {   return m_outputMask;   }
    /// Access to statistics entry
    Statistics statistics()  const;
    /// Set the input mask bits
    void setInputMask(const BitRegisters& bits);
    /// Set the output mask bits
    void setOutputMask(const BitRegisters& bits);
    /// Setup internal processor and run
    Executor* getFreeInstance();
    /// Initialize the factory
    virtual int initialize() throw (std::runtime_error);
    /// Print statistics
    void printStatistics(int level)  const;
    /// Print statistics
    void print(int level, const Statistics& stat)  const;
    /// Shutdown the whole stuff
    virtual void shutdown(bool delete_instances);
  };

  class Executor  {
  protected:

    /// Reference to factory the executor belongs to
    ExecutorFactory& m_fac;
    /// Alias to the factory's instance mask
    InstanceMask&    m_mask;
    /// Serial identifier of this executor
    size_t           m_serial;
    /// Timing measurements
    Timing           m_time;

  public:
    /// Initializing constructor
    Executor(ExecutorFactory& fac, InstanceMask& mask, size_t serial);
    /// Default destructor
    virtual ~Executor()                      {                                    }
    /// Access process
    const Timing& timing() const             { return m_time;                     }
    /// Accessor: Executor factory
    ExecutorFactory& factory()  const        { return m_fac;                      }
    /// Access  the identifier
    int id() const                           { return m_fac.id();                 }
    /// Access to serial number identifier
    int serial() const                       { return m_serial;                   }
    /// Output event data mask
    const IOMask& outputMask() const         { return m_fac.outputMask();         }
    /// Return instance to factory
    void returnInstance()                    { m_mask.set_bit(m_serial);          }
    /// Setup internal processor and run
    virtual Status execute(EventContext*)    { return Status::ERROR;              }
    /// Save monitoring information
    void monitor(const struct timeval& start, const struct timeval& stop);
  };
}      /* End namespace      */
#endif /* FRAMEWORK_IEXECUTOR_H */
