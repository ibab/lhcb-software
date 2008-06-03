#include <fenv.h>
#include <map>
#include <vector>
#include <string>
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/assign/list_of.hpp"

#include "FPESvc.h"

DECLARE_SERVICE_FACTORY( FPESvc );

FPESvc::FPESvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
  , m_log( msgSvc() , name )
{
    declareProperty("Trap", m_mask = boost::assign::list_of("DivByZero")
                                                           ("Inexact")
                                                           ("Overflow") );
    // other than "Initialize" valid values are from GaudiKernel/Incident.h
    // in the IncidentType namespace.
    declareProperty("ActivateWhen", m_when = "Initialize" );
}

StatusCode FPESvc::initialize() {
    if (Service::initialize().isFailure()) return StatusCode::FAILURE;

    if (m_when=="Initialize") return enable();
    IIncidentSvc*  incidentSvc;
    if (service( "IncidentSvc", incidentSvc).isFailure()) return StatusCode::FAILURE;
    // add listener to be triggered with low priority
    // so it gets called first
    bool rethrow = false;
    bool oneShot = false;
    incidentSvc->addListener(this,m_when,
                                  std::numeric_limits<long>::min(),rethrow,oneShot);
                                  incidentSvc->release();
    return StatusCode::SUCCESS;
}

void FPESvc::handle(const Incident&) {
    if (enable().isFailure()) {
        throw GaudiException("FPESvc: failed to enable... ","",StatusCode::FAILURE);
    }
}

StatusCode FPESvc::enable() {
//TODO: provide a system independent wrapper around feenableexcept to
//      catch the differences between systems.
//      -- this wrapper should probably live in GaudiKernel's System.cpp...
#if defined(linux) && defined(__GNUC__)

     static std::map<std::string,int> m = boost::assign::map_list_of
                                    ( "DivByZero" , int(FE_DIVBYZERO) )
                                    ( "Inexact"   , int(FE_INEXACT)   )
                                    ( "Overflow"  , int(FE_OVERFLOW)  )
                                    ( "Underflow" , int(FE_UNDERFLOW) )
                                    ( "AllExcept" , int(FE_ALL_EXCEPT));

     int mask=0;
     for (std::vector<std::string>::const_iterator i=m_mask.begin();
          i!=m_mask.end(); ++i) {
          std::map<std::string,int>::const_iterator j = m.find(*i);
          if (j==m.end()) { 
            m_log << MSG::ERROR << " unknown FE " << *i << endmsg;
            return StatusCode::FAILURE;
          }
          m_log << MSG::DEBUG << " adding FE " << *i << " = " << j->second  << endmsg;
          mask |= j->second;
     }
     m_log << MSG::DEBUG << " setting excepts " << mask << endmsg;
     feenableexcept( mask );
     return StatusCode::SUCCESS;
#else
// on windows, the following _might_ work:

// #include <float.h>
// unsigned int cw;
// /* could use _controlfp */
// cw = _control87(0,0) & MCW_EM;
// cw &= ~(_EM_INVALID|_EM_ZERODIVIDE|_EM_OVERFLOW);
// _control87(cw,MCW_EM);
//
     m_log << MSG::ERROR 
           << " Do not know how to configure the FPU"
           << " for trapping exceptions under anything but Linux..." << endmsg;

     return StatusCode::FAILURE;
#endif

}
