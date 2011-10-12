#ifndef FRAMEWORKTEST_ALGORITHM_H
#define FRAMEWORKTEST_ALGORITHM_H

// Framework include files
#include "Framework/IExecutor.h"
#include "Framework/EventContext.h"

// C++ include files
#include <vector>
#include <string>
#include <typeinfo>

/*
*    Framework namespace declaration
*/ 
namespace FrameworkTest  {
  typedef std::vector<std::string> StrVector;
  typedef const void* ClientData;
  struct ContextFactory;
  struct AlgorithmConfig;
  template <class T> struct Algorithm;

  int A__execute(void* instance, const std::type_info& typ, void* evt_context, const AlgorithmConfig* config);
  int A__declareIO(Framework::ExecutorFactory* f, const AlgorithmConfig* config);

  /**@class IODef Algorithm.h Framework/Algorithm.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011
  *
  */
  struct IODef : public StrVector  {
    /// Default constructor
    IODef();
    /// Initializing constructor
    IODef(const char* in[]);
    /// Initializing constructor
    IODef(const StrVector& c);
    /// Copy constructor
    IODef(const IODef& c);
  };

  /**@class AlgorithmConfig Algorithm.h Framework/Algorithm.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011
  *
  */
  struct AlgorithmConfig {
    AlgorithmConfig(const std::string& n, ClientData d, const IODef& i, const IODef& o, float t, float s) 
     : name(n), param(d), inputs(i), outputs(o), time(t), timeSigma(s)    {    }
    /// Executor factory name (=name of created instances)
    std::string name;
    /// Creation parameter
    ClientData  param;
    /// Input IO mask
    StrVector   inputs;
    /// Output IO mask
    StrVector   outputs;
    float       time;
    float       timeSigma;
  };

  /**@class Algorithm Algorithm.h Framework/Algorithm.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011
  *
  */
  template <class T> struct Algorithm   {
    const AlgorithmConfig* config;
    /// Initializing constructor
    Algorithm(const AlgorithmConfig* c) : config(c)  {                                }
    /// Default destructor
    virtual ~Algorithm()      {                                                       }
    /// Object initialization
    int initialize()          {  return A__declareIO(this,config);                    }
    /// Object finalization
    int finalize()            {  return 1;                                            }
    /// Event callback
    int execute(void* ctxt)   {  return A__execute(this,typeid(*this),ctxt,config);   }
  };

  template <class T> 
  struct AlgorithmExecutor : public Framework::Executor {
    T     m_alg;
  public:
    /// Initializing constructor
  AlgorithmExecutor(Framework::ExecutorFactory* f, Framework::InstanceMask& mask, size_t serial, const AlgorithmConfig* c)
    : Executor(f,mask,serial), m_alg(c) {}
    /// Executor override: Setup internal processor and run
    virtual Framework::Status execute(Framework::EventContext* context)   {
      int sc = m_alg.execute(context);
      return sc == 1 ? Framework::Status::SUCCESS : Framework::Status::ERROR;
    }
  };

  /** @class AlgorithmExecutorFactory Algorithm.h Tests/Algorithm.h
  *
  * @author  M.Frank
  * @version 1.0
  * @date    20/06/2011
  */
 template <class T> struct AlgorithmExecutorFactory : public Framework::ExecutorFactory {
   typedef T                        config_t;
   typedef Algorithm<config_t>      alg_t;
   typedef AlgorithmExecutor<alg_t> exec_t;
   // Startup data
   config_t m_config;

  public:
    /// Standard constructor
  AlgorithmExecutorFactory(size_t mx, const config_t& c)
    : Framework::ExecutorFactory(mx, c.name, c.param), m_config(c) {}

    /// Standard destructor
    virtual ~AlgorithmExecutorFactory() {}

    /// Object initialization
    virtual int initialize()  throw (std::runtime_error)
   {  return A__declareIO(this,&m_config);                   }

    /// Setup internal processor and run
    virtual Framework::Executor* createInstance()  throw (std::runtime_error)
   {  return new exec_t(this,m_idle,m_instances,&m_config);  }
  };
  
  /** @class AlgorithmExecutorFactory Algorithm.h Tests/Algorithm.h
  *
  * @author  M.Frank
  * @version 1.0
  * @date    20/06/2011
  */
  struct ContextFactory : virtual public Framework::EventContextFactory  {
    typedef Framework::IOMask  IOMask;
    typedef void*              ClientData;
    /// Input IO mask
    StrVector  m_data;
    /// Input IO mask
    IOMask     m_mask;
    /// Client parameter
    ClientData m_param;
    /// Event id counter
    long       m_id;
    /// Standard constructor
    ContextFactory(ClientData par, const IODef& data) : m_data(data), m_param(par), m_id(0)  { }
    /// Default destructor
    virtual ~ContextFactory()  {}
    /// Release factory. Factory may not be used anymore afterwards
    virtual void release()   {  delete this; }
    /// Get access to the IOMask to resolve the dependencies
    virtual const IOMask& dataMask() const { return m_mask; }
    /// Declare input data content to data registry
    virtual Framework::Status declareData();
    /// Create a new event context. On error the returned context is invalid (NULL)
    virtual Framework::EventContext* create()  {  return new Framework::EventContext(m_id++,m_mask); }
  };
}
#endif // FRAMEWORKTEST_ALGORITHM_H
