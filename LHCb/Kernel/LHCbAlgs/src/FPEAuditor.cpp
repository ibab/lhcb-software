#include "Kernel/FPEGuard.h"
#include <vector>
#include <string>
#include <utility>
#include <memory>
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"

/** @class FPEMaskProperty FPEAuditor.cpp
 *
 *  Utility class for the FPEAuditor
 *
 *  @author Gerhard Raven
 *  @date   09/06/2008
 */
class FPEMaskProperty {
public:
  typedef SimpleProperty<std::vector<std::string> > property_type;

  FPEMaskProperty() : m_mask(0)
  { m_property.declareUpdateHandler(&FPEMaskProperty::updateHandler,this); }
  FPE::Guard::mask_type value() const { return m_mask;}
  property_type& property() { return m_property; }
  FPEMaskProperty& set(const std::vector<std::string>& s)
  { property().setValue(s); return *this; }
private:
  void updateHandler(Property&) {
    m_mask=FPE::Guard::mask(m_property.value().begin(),m_property.value().end());
  }
  property_type   m_property;
  FPE::Guard::mask_type  m_mask;
};

/** @class FPEAuditor FPEAuditor.cpp
 *
 *  Auditor for enabling and trapping Floating Point Exceptions (FPEs).
 *
 *  Configure the FPU to trap a specified set of
 *  FPEs, and generate a SIGFPE (which leads to Gaudi
 *  providing a traceback and aborting the job).
 *
 *  Example configuration options :-
 *  @verbatim
 *  // Enable the auditor
 *  ApplicationMgr.ExtSvc += { "AuditorSvc" };
 *  AuditorSvc.Auditors += { "FPEAuditor" };
 *  // Exceptions to trap
 *  // Full list of possible types is "DivByZero","Overflow","Underflow","Invalid","Inexact".
 *  // "AllExcept" turns on all exceptions.
 *  FPEAuditor.TrapOn  = { "DivByZero","Overflow","Underflow","Invalid" };
 *  // Processing phases to enable exceptions during.
 *  FPEAuditor.ActivateAt = {"Execute" };
 *  // Disable exceptions for certain problematic components
 *  FPEAuditor.DisableTrapFor = { "Init","InitReprocSeq","BrunelInit" };
 *  @endverbatim
 *
 *  @see FPE::Guard
 *
 *  @author Gerhard Raven
 *  @date   09/06/2008
 */
class FPEAuditor : public Auditor {
public:
  FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator);
  ~FPEAuditor() {  }
  StatusCode initialize() {
    if (!FPE::Guard::has_working_implementation) { // note: this is a compile-time constant...
      throw GaudiException("FPEAuditor: no FPE trapping support on this architecture...","",StatusCode::FAILURE);
    }
    if (m_activateSuperGuard) m_superGuard.reset( new FPE::Guard( m_mask.value() ) );
    return StatusCode::SUCCESS;
  }
  StatusCode finalize() {
    m_superGuard.reset( (FPE::Guard*)0 );
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
      m_guards.push_back( std::make_pair( s, new FPE::Guard( m_mask.value(), veto) ) );
    }
  }

  void after(StandardEventType type, INamedInterface* i, const StatusCode& sc)
  {  std::ostringstream t; t << type;after(t.str(),i,sc); }
  void after(CustomEventTypeRef type, INamedInterface* i, const StatusCode& sc)
  {  if(!beforeCannotHaveBeenCalled(type,i)) after(type,i->name(),sc); }
  void after(StandardEventType type, const std::string& s, const StatusCode& sc)
  {  std::ostringstream t; t << type;after(t.str(),s,sc); }
  void after(CustomEventTypeRef eventType, const std::string& s, const StatusCode&)
  {
    if ( activeAt(eventType) ) {
      if (m_guards.empty()) {
        throw GaudiException("FPEAuditor: inbalance of before/after calls...","",StatusCode::FAILURE);
      }
      std::pair<std::string,FPE::Guard *>& p = m_guards.back();
      if (p.first!=s) { throw GaudiException("FPEAuditor: unexpected stack state...","",StatusCode::FAILURE); }
      delete p.second;
      m_guards.pop_back();
    }
  }

private:
  bool activeAt(const std::string s) { return ( std::find(m_when.begin(),m_when.end(),s)!=m_when.end() ); }
  bool beforeCannotHaveBeenCalled(CustomEventType type, INamedInterface *i) {
    // auditing starts 'after' AuditorSvc::initialize has been called
    // but, at that time, the matching 'before' hasn't been called (since
    // we were not yet activated!), so there is no state to restore, i.e.
    // m_guards is empty. So we make an exception to the rule that m_guards
    // cannot be empty for that particular corner case by ignoring the 'after'
    // call (which is the right thing to do ;-)...
    return (m_guards.empty() && type == "Initialize" && SmartIF<IAuditorSvc>(i).isValid());
  }

  mutable MsgStream                                m_log;
  std::vector<std::pair<std::string,FPE::Guard*> > m_guards;
  std::auto_ptr<FPE::Guard>                        m_superGuard;
  FPEMaskProperty                                  m_mask;
  std::vector<std::string>                         m_when, m_veto;
  bool                                             m_activateSuperGuard;
};

DECLARE_AUDITOR_FACTORY( FPEAuditor )

FPEAuditor::FPEAuditor( const std::string& name, ISvcLocator* pSvcLocator)
  : Auditor ( name , pSvcLocator )
  , m_log( msgSvc() , name )
{
  declareProperty("TrapOn", m_mask.set(
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
    {"DivByZero", "Invalid", "Overflow"}
#else
    boost::assign::list_of("DivByZero")("Invalid")("Overflow")
#endif
  ).property() );
  declareProperty("ActivateAt", m_when =
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
    {"Initialize", "ReInitialize", "Execute", "BeginRun", "EndRun", "Finalize"}
#else
    boost::assign::list_of("Initialize")
                  ("ReInitialize")
                  ("Execute")
                  ("BeginRun")
                  ("EndRun")
                  ("Finalize")
#endif
  );
  declareProperty("DisableTrapFor", m_veto );
  declareProperty("EnableGlobal", m_activateSuperGuard = false );
}
