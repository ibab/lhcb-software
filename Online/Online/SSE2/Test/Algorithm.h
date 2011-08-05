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
  struct ContextFactory;

  struct IODef : public StrVector  {
    /// Default constructor
    IODef();
    /// Initializing constructor
    IODef(const char* in[]);
    /// Copy constructor
    IODef(const IODef& c);
  };


  int A__execute(void* instance, const std::type_info& typ, const std::string& n, void* ctxt,const void* arg);
  int A__declareIO(Framework::ExecutorFactory* f, const StrVector& in, const StrVector& out);

  /**@class Algorithm Algorithm.h Framework/Algorithm.h
  *
  *  @author  M.Frank
  *  @version 1.0
  *  @date    10/06/2011
  *
  */
  template <class T> struct Algorithm   {
    typedef std::pair<int, int> Arg;
    StrVector   inputs;
    StrVector   outputs;
    std::string name;
    const void* param;
    Arg         arg;

    /// Initializing constructor
    Algorithm(const std::string& n, const void* p, const StrVector& in, const StrVector& out, const Arg& a) 
      : inputs(in), outputs(out), name(n), param(p), arg(a)
    {                            }
    /// Default destructor
    virtual ~Algorithm()      {                                                }
    /// Object initialization
    int initialize()          {  return A__declareIO(this,inputs,outputs);     }
    /// Object finalization
    int finalize()            {  return 1;                                     }
    /// Event callback
    int execute(void* ctxt)   {  return A__execute(this,typeid(*this),name,ctxt,&arg);   }
  };

  template <class T> 
  struct AlgorithmExecutor : public Framework::Executor {
    typedef const void* ClientData;
    typedef std::pair<int, int> Arg;
    T   m_alg;
  public:
    /// Initializing constructor
    AlgorithmExecutor(Framework::ExecutorFactory* f, Framework::InstanceMask& mask, size_t serial, const std::string& n,
      ClientData p, const StrVector& in, const StrVector& out, const Arg& arg)
      : Executor(f, mask, serial), m_alg(n,p,in,out,arg)
    {}
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
    typedef std::pair<int, int> Arg;
    /// Input IO mask
    StrVector m_in;
    /// Output IO mask
    StrVector m_out;
    /// Argument 
    Arg       m_arg;
  public:
    /// Standard constructor
    AlgorithmExecutorFactory(size_t mx, const std::string& nam, ClientData par, const IODef& in, const IODef& out, const Arg& arg) 
    : Framework::ExecutorFactory(mx, nam, par), m_in(in), m_out(out), m_arg(arg)
    {    }
    /// Standard destructor
    virtual ~AlgorithmExecutorFactory() {}
    /// Object initialization
    virtual int initialize()  throw (std::runtime_error)
    {  return A__declareIO(this,m_in,m_out);     }
    /// Setup internal processor and run
    virtual Framework::Executor* createInstance()
    {  return new AlgorithmExecutor<Algorithm<T> >(this,m_idle,m_instances,name(),param(),m_in,m_out,m_arg);}
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
    virtual Framework::EventContext* create()  {  return new Framework::EventContext(m_id++, m_mask); }
  };
}
#endif // FRAMEWORKTEST_ALGORITHM_H
