// $Id: MCSources.cpp,v 1.1 2007-12-09 18:20:17 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCSources.h"
#include "LoKi/MCAlgs.h"
#include "LoKi/MCExtract.h"
#include "LoKi/Services.h"
// ============================================================================
/** @file 
 *  Implementation file for various sources
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @date 2006-12-07
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const std::string&           path , 
  IDataProviderSvc*            svc  , 
  const LoKi::MCTypes::MCCuts& cuts ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const std::string&           path , 
  const LoKi::MCTypes::MCCuts& cuts , 
  IDataProviderSvc*            svc  ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const LoKi::MCTypes::MCCuts& cuts ,
  const std::string&           path , 
  IDataProviderSvc*            svc  ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const LoKi::MCTypes::MCCuts& cuts ,
  IDataProviderSvc*            svc  ,
  const std::string&           path ) 
  : LoKi::MCParticles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::MCParticles::SourceTES::SourceTES 
( const LoKi::MCParticles::SourceTES& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::MCParticles::SourceTES::_Source ( right ) 
  , m_path    ( right.m_path    ) 
  , m_dataSvc ( right.m_dataSvc ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::SourceTES::~SourceTES(){}
// ============================================================================
// MANDATORY: the only essential method:
LoKi::MCParticles::SourceTES::result_type 
LoKi::MCParticles::SourceTES::operator() () const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               , 
             "Could not locate valid IDataProvidrSvc" ) ;
  }
  SmartDataPtr<LHCb::MCParticle::Container> events 
    ( m_dataSvc , m_path ) ;
  const LHCb::MCParticle::Container* parts = events ;
  Assert ( 0 != parts , "No valid data is found at location '"+m_path+"'") ;
  // 
  return LoKi::MCParticles::SourceTES::result_type 
    ( parts->begin() , parts->end  () ) ;
} 
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::MCParticles::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << "MCSOURCE(" << m_path << "," << m_cut << ")" ; }
// ============================================================================
// The END 
// ============================================================================
