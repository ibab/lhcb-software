// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrSources.h"
#include "LoKi/Services.h"
#include "LoKi/apply.h"
// ============================================================================
/** @file 
 *  Implementation file for various sources
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @date 2006-12-07
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const std::string&              path , 
  IDataProviderSvc*               svc  , 
  const LoKi::TrackTypes::RVCuts& cuts )
  : LoKi::RecVertices::SourceTES::_Source ()
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  , 
  const LoKi::TrackTypes::RVCuts& cuts ) 
  : LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const std::string&              path , 
  const LoKi::TrackTypes::RVCuts& cuts , 
  IDataProviderSvc*               svc  )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  const LoKi::TrackTypes::RVCuts& cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const LoKi::TrackTypes::RVCuts& cuts ,
  const std::string&              path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const LoKi::TrackTypes::RVCuts& cuts ,
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const LoKi::TrackTypes::RVCuts& cuts ,
  IDataProviderSvc*               svc  ,
  const std::string&              path ) 
  : LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const LoKi::TrackTypes::RVCuts& cuts ,
  IDataProviderSvc*               svc  ,
  const std::vector<std::string>& path ) 
  : LoKi::RecVertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::RecVertices::SourceTES::SourceTES 
( const LoKi::RecVertices::SourceTES& right ) 
  : LoKi::AuxFunBase                 ( right ) 
  , LoKi::RecVertices::SourceTES::_Source ( right ) 
  , m_path    ( right.m_path    ) 
  , m_dataSvc ( right.m_dataSvc ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::RecVertices::SourceTES::~SourceTES(){}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::RecVertices::SourceTES::result_type 
LoKi::RecVertices::SourceTES::operator() () const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  Assert ( !m_path.empty() , "No TES locations are specified!" ) ;
  //
  LHCb::RecVertex::Vector output ;
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  { get ( *iaddr , output ) ; }
  //
  return output ;
}
// ============================================================================
// get the particles from the certain  TES location 
// ============================================================================
std::size_t LoKi::RecVertices::SourceTES::get 
( const std::string&           location , 
  LHCb::RecVertex::Vector& output   ) const 
{
  //
  SmartDataPtr<LHCb::RecVertex::Container> parts ( m_dataSvc , location ) ;
  //
  const LHCb::RecVertex::Container* tracks = parts ;
  Assert ( 0 != tracks , 
           "No valid data is found at location '" + location + "'") ;
  //
  const size_t n = output.size() ;
  output.reserve ( n + tracks -> size() ) ;
  /// select the particles 
  LoKi::apply_filter  
    ( tracks -> begin () , 
      tracks -> end   () , 
      m_cut.func ()      , 
      std::back_inserter ( output ) ) ;
  //
  return output.size() - n ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::RecVertices::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << "RVSOURCE(" 
           << Gaudi::Utils::toString( m_path ) << "," << m_cut << ")" ; }
// ============================================================================



  

// ============================================================================
// The END 
// ============================================================================
