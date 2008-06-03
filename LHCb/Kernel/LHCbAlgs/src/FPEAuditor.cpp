#if defined(linux) && defined(__GNUC__)
#include <fenv.h>
#endif

#include <map>
#include <vector>
#include <string>
#include <ostream>
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "boost/assign/list_of.hpp"

class FPEGuard {
public: 
    FPEGuard(std::vector<std::string>& masks,bool enableOnConstruction=false) 
        : m_mask(0)
        , m_prevMask( get() )
    {
     static std::map<std::string,int> m = boost::assign::map_list_of
                                    ( "DivByZero" , int(FE_DIVBYZERO) )
                                    ( "Inexact"   , int(FE_INEXACT)   )
                                    ( "Overflow"  , int(FE_OVERFLOW)  )
                                    ( "Underflow" , int(FE_UNDERFLOW) )
                                    ( "AllExcept" , int(FE_ALL_EXCEPT));
     for (std::vector<std::string>::const_iterator i=masks.begin();
          i!=masks.end(); ++i) {
          std::map<std::string,int>::const_iterator j = m.find(*i);
          if (j==m.end()) { 
                throw GaudiException("FPEAuditor: unknown mask... ","",StatusCode::FAILURE);
          }
          m_mask |= j->second;
     }
     if (enableOnConstruction) enable();
    }
    ~FPEGuard() { disable(); }

#if defined(linux) && defined(__GNUC__)
    int get() { return fegetexcept(); }
    void enable()  { feenableexcept( m_mask ); }
    void disable() { feenableexcept( m_prevMask ); }
#else
// on windows, the following _might_ work:
// #include <float.h>
// unsigned int cw;
// /* could use _controlfp */
// cw = _control87(0,0) & MCW_EM;
// cw &= ~(_EM_INVALID|_EM_ZERODIVIDE|_EM_OVERFLOW);
// _control87(cw,MCW_EM);
//
    int get() { throw GaudiException("FPE trapping not implemented..... ","",StatusCode::FAILURE); return 0;}
    void enable()  { get(); }
    void disable() { get(); }
#endif

private:
    int m_mask;
    int m_prevMask;
};


class FPEAuditor : public Auditor {
public:
    FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator);
    FPEAuditor::~FPEAuditor() { delete m_guard; m_guard = 0; }

    StatusCode FPEAuditor::initialize() { m_guard = new FPEGuard( m_mask ); return StatusCode::SUCCESS;}
    StatusCode FPEAuditor::finalize() { delete m_guard; m_guard = 0;return StatusCode::SUCCESS; }



    void before(StandardEventType type, INamedInterface*) { before(type);  }
    void before(StandardEventType type, const std::string&) { before(type); }
    void before(StandardEventType type) { std::ostringstream s; s << type; before(s.str()); }
    void before(CustomEventType type, INamedInterface*) { before(type);  }
    void before(CustomEventType type, const std::string&) { before(type); }
    void before(CustomEventTypeRef type) {
        if ( std::find(m_when.begin(),m_when.end(),type)!=m_when.end()) { m_guard->enable(); }
    }

    void after(StandardEventType type, INamedInterface*, const StatusCode&) { after(type); }
    void after(StandardEventType type, const std::string&, const StatusCode&) { after(type); }
    void after(StandardEventType type) { std::ostringstream s; s << type; after(s.str()); }
    void after(CustomEventTypeRef type, INamedInterface*, const StatusCode&) { after(type); }
    void after(CustomEventTypeRef type, const std::string&, const StatusCode&) { after(type); }
    void after(CustomEventTypeRef type) {
        if ( std::find(m_when.begin(),m_when.end(),type)!=m_when.end()) { m_guard->disable(); }
    }


    void beforeInitialize(INamedInterface *i) { return before(IAuditor::Initialize,i); }
    void beforeReInitialize(INamedInterface *i) { return before(IAuditor::ReInitialize,i); }
    void beforeExecute(INamedInterface *i) { return before(IAuditor::Execute,i); }
    void beforeBeginRun(INamedInterface *i) { return before(IAuditor::BeginRun,i); }
    void beforeEndRun(INamedInterface *i) { return before(IAuditor::EndRun,i); }
    void beforeFinalize(INamedInterface *i) { return before(IAuditor::EndRun,i); }

    void afterInitialize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Initialize,i,s); }
    void afterReInitialize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::ReInitialize,i,s); }
    void afterExecute(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Execute,i,s); }
    void afterBeginRun(INamedInterface *i, const StatusCode& s) { return after(IAuditor::BeginRun,i,s); }
    void afterEndRun(INamedInterface *i, const StatusCode& s) { return after(IAuditor::EndRun,i,s); }
    void afterFinalize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Finalize,i,s); }

private:
    FPEGuard *m_guard;
    mutable MsgStream m_log;

    std::vector<std::string> m_mask,m_when;
};

DECLARE_AUDITOR_FACTORY( FPEAuditor );

FPEAuditor::FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator)
  : Auditor ( name , pSvcLocator )
  , m_guard(0)
  , m_log( msgSvc() , name )
{
    declareProperty("TrapOn", m_mask = boost::assign::list_of("DivByZero")
                                                             ("Inexact")
                                                             ("Overflow") );
    declareProperty("ActivateAt", m_when = boost::assign::list_of("Initialize")
                                                                 ("ReInitialize")
                                                                 ("Execute")
                                                                 ("BeginRun")
                                                                 ("EndRun")
                                                                 ("Finalize") );
//TODO: at a DoNotActivateFor property to decide for which Algorithms not to enable..
}
