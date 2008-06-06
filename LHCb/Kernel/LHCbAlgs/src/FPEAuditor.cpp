#if defined(linux) && defined(__GNUC__)
#include <fenv.h>
#elif defined(_WIN32)
#include <float.h>
#endif

#include <map>
#include <vector>
#include <string>
#include <ostream>
#include <utility>
#include <memory>
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "boost/assign/list_of.hpp"

namespace FPE {
#if defined(linux) && defined(__GNUC__)
    int get()             { return fegetexcept(); }
    int disable(int mask) { return fedisableexcept(mask); }
    int enable(int mask) { 
        feclearexcept(mask); // remove any 'stale' exceptions before switching on trapping
                             // otherwise we immediately trigger an exception...
        return feenableexcept(mask); 
    }
    static std::map<std::string,int>& map() {
         static std::map<std::string,int> m = boost::assign::map_list_of
                                        ( "Inexact"   , int(FE_INEXACT)  )
                                        ( "DivByZero" , int(FE_DIVBYZERO))
                                        ( "Underflow" , int(FE_UNDERFLOW))
                                        ( "Overflow"  , int(FE_OVERFLOW) )
                                        ( "Invalid"   , int(FE_INVALID)  )
                                        ( "AllExcept" , int(FE_ALL_EXCEPT));
          return m;
    }
#elif defined(_WIN32)
    int get()             { return _control87(0,0); }
    int disable(int mask) { return _control87(~mask,MCW_EM); }
    int enable(int mask)  { return _control87(mask,MCW_EM); }
    static std::map<std::string,int>& map() {
         static std::map<std::string,int> m = boost::assign::map_list_of
                                    ( "Inexact"   , int(EM_INEXACT)   )
                                    ( "DivByZero" , int(EM_ZERODIVIDE) )
                                    ( "Underflow" , int(EM_UNDERFLOW) )
                                    ( "Overflow"  , int(EM_OVERFLOW)  )
                                    ( "AllExcept" , int(EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));
         return m;
    }
#else
    int get() { 
      throw GaudiException("FPE trapping not implemented..... ","",StatusCode::FAILURE);
    }
    int disable(int) { get(); return 0;}
    int enable(int) { get(); return 0; }
    static std::map<std::string,int>& map() {
         static std::map<std::string,int> m;
         get();
         return m;
    }
#endif
}


class FPEGuard {
public: 
    FPEGuard(int mask, bool reverse=false)
    : m_mask(mask)
    , m_initial( FPE::get() )
    , m_reverse(reverse)
    { 
      if (m_reverse) { FPE::disable(m_mask); }
      else           { FPE::enable(m_mask); }
    }
    ~FPEGuard() 
    { 
       FPE::disable( ~m_initial);
       FPE::enable(    m_initial);
       if (FPE::get()!=m_initial) { throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE); }
    }

private:
    int m_mask;
    int m_initial;
    bool m_reverse;
};


class FPEMaskProperty {
 public:
    typedef SimpleProperty<std::vector<std::string> > property_type;

     FPEMaskProperty() : m_mask(0) 
     { m_property.declareUpdateHandler(&FPEMaskProperty::updateHandler,this); }
     int value() const { return m_mask;}
     property_type& property() { return m_property; }
     FPEMaskProperty& set(const std::vector<std::string>& s) 
     { property().setValue(s); return *this; }
 private:
    void updateHandler(Property&) {
         m_mask=0;
         for (std::vector<std::string>::const_iterator i=m_property.value().begin();
              i!=m_property.value().end(); ++i) {
              std::map<std::string,int>::const_iterator j = FPE::map().find(*i);
              if (j==FPE::map().end()) { 
                    throw GaudiException("FPEMaskProperty: unknown mask... ","",StatusCode::FAILURE);
              }
              m_mask |= j->second;
         }
     }

     property_type m_property;
     int           m_mask;
};


class FPEAuditor : public Auditor {
public:
    FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator);
    FPEAuditor::~FPEAuditor() {  }
    StatusCode FPEAuditor::initialize() {
        if (m_activateSuperGuard) m_superGuard.reset( new FPEGuard( m_mask.value() ) );
        return StatusCode::SUCCESS;
    }
    StatusCode FPEAuditor::finalize() {
        m_superGuard.reset( (FPEGuard*)0 );
        return StatusCode::SUCCESS;
    }

    void before(StandardEventType type, INamedInterface* i) { before(type,i->name()); }
    void before(CustomEventTypeRef   type, INamedInterface* i) { before(type,i->name()); }
    void before(StandardEventType type, const std::string& s) 
    {  std::ostringstream t; t << type;before(t.str(),s); }
    void before(CustomEventTypeRef eventType, const std::string& s) 
    { 
      if ( activeAt(eventType) ) {
         bool veto = ( std::find(m_veto.begin(),m_veto.end(),s) != m_veto.end()) ;
		 m_guards.push_back( std::make_pair( s, new FPEGuard( m_mask.value(), veto) ) ); 
      }
    }

    void after(StandardEventType  type, INamedInterface* i, const StatusCode& sc) { after(type,i->name(),sc); }
    void after(CustomEventTypeRef type, INamedInterface* i, const StatusCode& sc) {  after(type,i->name(),sc); }
    void after(StandardEventType type, const std::string& s, const StatusCode& sc) 
    {  std::ostringstream t; t << type;after(t.str(),s,sc); }
    void after(CustomEventTypeRef eventType, const std::string& s, const StatusCode&) 
    { 
     if ( activeAt(eventType) ) {
       if (m_guards.empty()) { 
        throw GaudiException("FPEAuditor: inbalance of before/after calls...","",StatusCode::FAILURE);
       }
       std::pair<std::string,FPEGuard *>& p = m_guards.back();
       if (p.first!=s) { throw GaudiException("FPEAuditor: unexpected stack state...","",StatusCode::FAILURE); }
       delete p.second;
       m_guards.pop_back();
     }
    }


    void beforeInitialize(INamedInterface *i) { return before(IAuditor::Initialize,i); }
    void beforeReInitialize(INamedInterface *i) { return before(IAuditor::ReInitialize,i); }
    void beforeExecute(INamedInterface *i) { return before(IAuditor::Execute,i); }
    void beforeBeginRun(INamedInterface *i) { return before(IAuditor::BeginRun,i); }
    void beforeEndRun(INamedInterface *i) { return before(IAuditor::EndRun,i); }
    void beforeFinalize(INamedInterface *i) { return before(IAuditor::Finalize,i); }

    void afterInitialize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Initialize,i,s); }
    void afterReInitialize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::ReInitialize,i,s); }
    void afterExecute(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Execute,i,s); }
    void afterBeginRun(INamedInterface *i, const StatusCode& s) { return after(IAuditor::BeginRun,i,s); }
    void afterEndRun(INamedInterface *i, const StatusCode& s) { return after(IAuditor::EndRun,i,s); }
    void afterFinalize(INamedInterface *i, const StatusCode& s) { return after(IAuditor::Finalize,i,s); }

private:
    bool activeAt(const std::string s) { return ( std::find(m_when.begin(),m_when.end(),s)!=m_when.end() ); }

    mutable MsgStream                              m_log;
    std::vector<std::pair<std::string,FPEGuard*> > m_guards;
    std::auto_ptr<FPEGuard>                        m_superGuard;
    FPEMaskProperty                                m_mask;
    std::vector<std::string>                       m_when,
                                                   m_veto;
    bool                                           m_activateSuperGuard;
};

DECLARE_AUDITOR_FACTORY( FPEAuditor );

FPEAuditor::FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator)
  : Auditor ( name , pSvcLocator )
  , m_log( msgSvc() , name )
{
    declareProperty("TrapOn", m_mask.set(  boost::assign::list_of("DivByZero")
                                                                 ("Inexact")
                                                                 ("Overflow") ).property() );
    declareProperty("ActivateAt", m_when = boost::assign::list_of("Initialize")
                                                                 ("ReInitialize")
                                                                 ("Execute")
                                                                 ("BeginRun")
                                                                 ("EndRun")
                                                                 ("Finalize") );
    declareProperty("DisableTrapFor", m_veto );
    declareProperty("EnableGlobal", m_activateSuperGuard = false );
}
