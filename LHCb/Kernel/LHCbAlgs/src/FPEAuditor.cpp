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
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "boost/assign/list_of.hpp"


class FPEMaskProperty {
 public:
    typedef SimpleProperty<std::vector<std::string> > property_type;
     FPEMaskProperty() : m_mask(0) {
        m_property.declareUpdateHandler(&FPEMaskProperty::updateHandler,this); 
     }
     int value() const { return m_mask;}
     property_type& property() { return m_property; }
     FPEMaskProperty& set(const std::vector<std::string>& s) { property().setValue(s); return *this; }
 private:
    void updateHandler(Property&) {
#if defined(linux) && defined(__GNUC__)
         static std::map<std::string,int> m = boost::assign::map_list_of
                                        ( "DivByZero" , int(FE_DIVBYZERO) )
                                        ( "Inexact"   , int(FE_INEXACT)   )
                                        ( "Overflow"  , int(FE_OVERFLOW)  )
                                        ( "Underflow" , int(FE_UNDERFLOW) )
                                        ( "AllExcept" , int(FE_ALL_EXCEPT));
#elif defined(_WIN32)
         static std::map<std::string,int> m = boost::assign::map_list_of
                                    ( "DivByZero" , int(EM_ZERODIVIDE) )
                                    ( "Inexact"   , int(EM_INEXACT)   )
                                    ( "Overflow"  , int(EM_OVERFLOW)  )
                                    ( "Underflow" , int(EM_UNDERFLOW) )
                                    ( "AllExcept" , int(EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));
#else
         static std::map<std::string,int> m;
#endif
         m_mask=0;
         for (std::vector<std::string>::const_iterator i=m_property.value().begin();
              i!=m_property.value().end(); ++i) {
              std::map<std::string,int>::const_iterator j = m.find(*i);
              if (j==m.end()) { 
                    throw GaudiException("FPEMaskProperty: unknown mask... ","",StatusCode::FAILURE);
              }
              m_mask |= j->second;
         }
     }

     property_type m_property;
     int           m_mask;
};

class FPEGuard {
public: 
#if defined(linux) && defined(__GNUC__)
    FPEGuard(int mask, bool reverse=false)
    : m_mask(mask)
    , m_initial( fegetexcept() )
    , m_reverse(reverse)
    { 
      if (m_reverse) { fedisableexcept(m_mask); }
      else           { feenableexcept(m_mask); }
    }
    ~FPEGuard() 
    { 
       fedisableexcept( ~m_initial);
       feenableexcept(   m_initial);
       if (fegetexcept()!=m_initial) { throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE); }
    }
#elif defined(_WIN32)
    FPEGuard(int mask, bool reverse=false)
    : m_mask(mask)
    , m_initial( _control87(0,0) )
    , m_reverse(reverse)
    { 
      if (m_reverse) { _control87(~m_mask,MCW_EM); }
      else           { _control87( m_mask,MCW_EM); }
    }
    ~FPEGuard() 
    { 
       _control87( m_initial, MCW_EM);
       if (_control87(0,0)!=m_initial) { throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE); }
    }
#else

    FPEGuard(int mask, bool reverse=false)
    { 
      throw GaudiException("FPE trapping not implemented..... ","",StatusCode::FAILURE);
    }
#endif

private:
    int m_mask;
    int m_initial;
    bool m_reverse;
};


class FPEAuditor : public Auditor {
public:
    FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator);
    FPEAuditor::~FPEAuditor() {  }

    void before(StandardEventType type, INamedInterface* i) { before(type,i->name()); }
    void before(CustomEventType   type, INamedInterface* i) { before(type,i->name()); }
    void before(StandardEventType type, const std::string& s) 
    {  std::ostringstream t; t << type;before(t.str(),s); }
    void before(CustomEventType type, const std::string& s) 
    { 
      if ( std::find(m_when.begin(),m_when.end(),type)!=m_when.end() ) {
         bool veto = ( std::find(m_veto.begin(),m_veto.end(),s) != m_veto.end()) ;
		 m_guards.push_back( std::make_pair( s, new FPEGuard( m_mask.value(), veto) ) ); 
      }
    }

    void after(StandardEventType type, INamedInterface* i, const StatusCode& sc) { after(type,i->name(),sc); }
    void after(CustomEventTypeRef type, INamedInterface* i, const StatusCode& sc) { after(type,i->name(),sc); }
    void after(StandardEventType type, const std::string& s, const StatusCode& sc) 
    {  std::ostringstream t; t << type;after(t.str(),s,sc); }
    void after(CustomEventTypeRef type, const std::string& s, const StatusCode&) 
    { 
     if ( std::find(m_when.begin(),m_when.end(),type)!=m_when.end() ) { 
       std::pair<std::string,FPEGuard *>& p = m_guards.back();
       if (p.first!=s) { throw GaudiException("FPEAuditor: inconsistent state...","",StatusCode::FAILURE); }
       delete p.second;
       m_guards.pop_back();
     }
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
    std::vector<std::pair<std::string,FPEGuard*> > m_guards;
    mutable MsgStream m_log;

    FPEMaskProperty          m_mask;
    std::vector<std::string> m_when,m_veto;
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
}
