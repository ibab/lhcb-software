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
    typedef int mask_type;
    mask_type disable(mask_type mask) { return fedisableexcept(mask); }
    mask_type enable(mask_type mask)  { 
        feclearexcept(mask); // remove any 'stale' exceptions before switching on trapping
                             // otherwise we immediately trigger an exception...
        return feenableexcept(mask); 
    }
    static std::map<std::string,mask_type>& map() {
         static std::map<std::string,mask_type> m = boost::assign::map_list_of
                   ( "Inexact"   , mask_type(FE_INEXACT)  )
                   ( "DivByZero" , mask_type(FE_DIVBYZERO))
                   ( "Underflow" , mask_type(FE_UNDERFLOW))
                   ( "Overflow"  , mask_type(FE_OVERFLOW) )
                   ( "Invalid"   , mask_type(FE_INVALID)  )
                   ( "AllExcept" , mask_type(FE_ALL_EXCEPT));
          return m;
    }
#elif defined(_WIN32)
    typedef unsigned int mask_type;
    // VS8
    // mask_type disable(mask_type mask) { mask_type p; _controlfp_s(&p,~mask,_MCW_EM); return p;}
    // mask_type enable(mask_type mask)  { mask_type p; _controlfp_s(&p, mask,_MCW_EM); return p;}
    // VS7
    mask_type disable(mask_type mask) { return _controlfp(~mask,_MCW_EM);}
    mask_type enable(mask_type mask)  { return _controlfp( mask,_MCW_EM);}
    static std::map<std::string,mask_type>& map() {
         static std::map<std::string,mask_type> m = boost::assign::map_list_of
                   ( "Inexact"   , mask_type(EM_INEXACT)   )
                   ( "DivByZero" , mask_type(EM_ZERODIVIDE))
                   ( "Underflow" , mask_type(EM_UNDERFLOW) )
                   ( "Overflow"  , mask_type(EM_OVERFLOW)  )
                   ( "Invalid"   , mask_type(EM_INVALID)   )
                   ( "AllExcept" , mask_type(EM_INVALID|EM_OVERFLOW|EM_UNDERFLOW|EM_INEXACT|EM_ZERODIVIDE|EM_DENORMAL));
         return m;
    }
#else
    typedef int mask_type;
    mask_type disable(mask_type) { 
      throw GaudiException("FPE trapping not implemented..... ","",StatusCode::FAILURE);
      return 0; 
    }
    mask_type enable(mask_type) { return disable(); }
    static std::map<std::string,mask_type>& map() {
         static std::map<std::string,mask_type> m;
         disable();
         return m;
    }
#endif
}


class FPEGuard {
public: 
    FPEGuard(FPE::mask_type mask, bool reverse=false)
    : m_initial( reverse ? FPE::disable(mask) : FPE::enable(mask) )
    { }

    ~FPEGuard() 
    { 
       FPE::disable( ~m_initial );
       FPE::mask_type mask = FPE::enable( m_initial );
       if (mask!=m_initial) { throw GaudiException("oops -- FPEGuard failed to restore initial state","",StatusCode::FAILURE); }
    }

private:
    FPE::mask_type m_initial;
};


class FPEMaskProperty {
 public:
    typedef SimpleProperty<std::vector<std::string> > property_type;

     FPEMaskProperty() : m_mask(0) 
     { m_property.declareUpdateHandler(&FPEMaskProperty::updateHandler,this); }
     FPE::mask_type value() const { return m_mask;}
     property_type& property() { return m_property; }
     FPEMaskProperty& set(const std::vector<std::string>& s) 
     { property().setValue(s); return *this; }
 private:
    void updateHandler(Property&) {
         m_mask=0;
         for (std::vector<std::string>::const_iterator i=m_property.value().begin();
              i!=m_property.value().end(); ++i) {
              std::map<std::string,FPE::mask_type>::const_iterator j = FPE::map().find(*i);
              if (j==FPE::map().end()) { 
                    throw GaudiException("FPEMaskProperty: unknown mask... ","",StatusCode::FAILURE);
              }
              m_mask |= j->second;
         }
     }

     property_type   m_property;
     FPE::mask_type  m_mask;
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
                                                                 ("Invalid")
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
