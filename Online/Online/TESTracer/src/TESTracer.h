#ifndef TESTRACER_TESTRACER_H
#define TESTRACER_TESTRACER_H

#include "ITESTracer.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/Auditor.h"

#include <sys/time.h>
#include <set>

namespace TES {
  struct AlgInfo {
    int    order;
    int    level;
    int    flags;
    int    calls;
    long   time, time2;
    struct timeval         start;
    std::string            typ;
    std::string            abiTyp;
    std::string            name;
    INamedInterface*       ptr;
    AlgInfo*               parent;
    std::vector<AlgInfo*>  daughters;
    std::set<std::string>  get;
    std::set<std::string>  put;
    std::set<std::string>  aux;
  };

  struct Tracer : virtual public TESTracer  {
    INamedInterface* m_current;
    typedef std::pair<std::string,int> AlgEntry;
    typedef std::pair<int,std::pair<int,int> >  AccessEntry;
    typedef std::map<std::string, AccessEntry>  AccessEntries;
    typedef std::map<std::string, AlgInfo*>     AlgEntries;
    
    int  m_level;
    int  m_nested;
    AccessEntries         m_data;
    std::vector<AlgEntry> m_algorithms;
    std::vector<AlgInfo*> m_initSeq;

    enum { IS_GAUDISEQUENCER= 1<<0,
	   GAUDISEQ_OR      = 1<<1,
	   GAUDISEQ_SHORT   = 1<<2,
	   GAUDISEQ_IGNORE  = 1<<3,
	   GAUDISEQ_RETOK   = 1<<4,
	   IS_SEQUENCER     = 1<<10,
	   SEQ_STOPOVERRIDE = 1<<11,
	   IS_PRESCALER     = 1<<20,
	   IS_FRAMEWORK     = 1<<25,
	   LAST
    };

    AlgEntries  m_algos;

    void i_trace(IService* svc, TracerCommand cmd, const std::string& caller, IRegistry* pReg, const char* path);
    void i_trace(IService* svc, TracerCommand cmd, IRegistry* pReg, const char* path);
    virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg) {
      i_trace(svc, cmd, pReg, 0);
    }
    virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg, const std::string& path) {
      i_trace(svc, cmd, pReg, path.c_str());
    }
    virtual void start();
    virtual void stop();

    virtual void beforeInitialize(INamedInterface*  a);
    virtual void afterInitialize (INamedInterface*  a);
    virtual void beforeFinalize  (INamedInterface*  a);
    virtual void afterFinalize   (INamedInterface*  a);
    virtual void beforeExecute   (INamedInterface*  a);
    virtual void afterExecute    (INamedInterface*  a, const StatusCode& s );

    static const char* indent(int n);
    void printExecAlgs(int max_level);
    void printInitAlgs(int max_level);
    void printDataRegistry();
    void fillAlgProperties(INamedInterface* a);
    void addAlgorithm(INamedInterface* a, AlgInfo* pi);
    void makeCode();
    Tracer();
    virtual ~Tracer() {}
  };


  Tracer& _tracer();

  struct TraceAuditor : public Auditor, virtual public TESTracer  {
    virtual void start()                                  {  _tracer().start();             }
    virtual void stop()                                   {  _tracer().stop();              }
    virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg) 
    {      _tracer().i_trace(svc, cmd, pReg, 0);                  }
    virtual void trace(IService* svc, TracerCommand cmd, IRegistry* pReg, const std::string& path) 
    {      _tracer().i_trace(svc, cmd, pReg, path.c_str());       }
    virtual void beforeInitialize(INamedInterface*  a)    {  _tracer().beforeInitialize(a); }
    virtual void afterInitialize (INamedInterface*  a)    {  _tracer().afterInitialize(a);  }
    virtual void beforeFinalize  (INamedInterface*  a)    {  _tracer().beforeFinalize(a);   }
    virtual void afterFinalize   (INamedInterface*  a)    {  _tracer().afterFinalize(a);    }
    virtual void beforeExecute   (INamedInterface*  a)    {  _tracer().beforeExecute(a);    }
    virtual void afterExecute    (INamedInterface*  a, const StatusCode& s )    {  _tracer().afterExecute(a,s); }

  public:
    /// standard constructor @see Auditor
    TraceAuditor(const std::string& name, ISvcLocator* pSvc) : Auditor(name,pSvc)  {}
    /// virtual destructor
    virtual ~TraceAuditor() {}
    /// standard initialization, @see IAuditor
    virtual StatusCode initialize() {  return StatusCode::SUCCESS;  }
    /// standard finalization, @see IAuditor
    virtual StatusCode finalize()   {  return StatusCode::SUCCESS;  }
  };
}       // End namespace
#endif  // End header
