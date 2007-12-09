// $Id: TrSources.cpp,v 1.1 2007-12-09 19:04:44 ibelyaev Exp $
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
#include "LoKi/select.h"
#include "LoKi/apply.h"
#include "LoKi/ITrHybridTool.h"
// ============================================================================
/** @file 
 *  Implementation file for various sources
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @date 2006-12-07
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const std::string&              path , 
  IDataProviderSvc*               svc  , 
  const LoKi::TrackTypes::TrCuts& cuts ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  , 
  const LoKi::TrackTypes::TrCuts& cuts ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const std::string&              path , 
  const LoKi::TrackTypes::TrCuts& cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  const LoKi::TrackTypes::TrCuts& cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const LoKi::TrackTypes::TrCuts& cuts ,
  const std::string&              path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const LoKi::TrackTypes::TrCuts& cuts ,
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const LoKi::TrackTypes::TrCuts& cuts ,
  IDataProviderSvc*               svc  ,
  const std::string&              path ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const LoKi::TrackTypes::TrCuts& cuts ,
  IDataProviderSvc*               svc  ,
  const std::vector<std::string>& path ) 
  : LoKi::Tracks::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Tracks::SourceTES::SourceTES 
( const LoKi::Tracks::SourceTES& right ) 
  : LoKi::AuxFunBase                 ( right ) 
  , LoKi::Tracks::SourceTES::_Source ( right ) 
  , m_path    ( right.m_path    ) 
  , m_dataSvc ( right.m_dataSvc ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Tracks::SourceTES::~SourceTES(){}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Tracks::SourceTES::result_type 
LoKi::Tracks::SourceTES::operator() () const 
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
  LHCb::Track::Vector output ;
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  { get ( *iaddr , output ) ; }
  //
  return output ;
}

// ============================================================================
// get the particles from the certain  TES location 
// ============================================================================
std::size_t LoKi::Tracks::SourceTES::get 
( const std::string&           location , 
  LHCb::Track::Vector& output   ) const 
{
  //
  SmartDataPtr<LHCb::Track::Container> parts ( m_dataSvc , location ) ;
  //
  const LHCb::Track::Container* tracks = parts ;
  Assert ( 0 != tracks , 
           "No valid data is found at location '" + location + "'") ;
  //
  const size_t n = output.size() ;
  output.reserve ( n + tracks -> size() ) ;
  /// select the particles 
  LoKi::apply_filter  
    ( tracks -> begin () , 
      tracks -> end   () , 
      m_cut.fun ()       , 
      std::back_inserter ( output ) ) ;
  //
  return output.size() - n ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Tracks::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << "TrSOURCE(" 
           << Gaudi::Utils::toString( m_path ) << "," << m_cut << ")" ; }
// ============================================================================

// ============================================================================
// constructor from the data:
// ============================================================================
LoKi::Tracks::SourceDirect::SourceDirect 
( const LHCb::Track::Vector* data  )
  : LoKi::Tracks::SourceDirect::_Source ()
  , m_data ( data ) 
{}
// ============================================================================
// constructor from tool 
// ============================================================================
LoKi::Tracks::SourceDirect::SourceDirect 
( const LoKi::ITrHybridTool* tool  )
  : LoKi::Tracks::SourceDirect::_Source ()
  , m_data ( 0  ) 
{
  if ( 0 != tool ) { m_data = tool -> tracks () ; }  
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Tracks::SourceDirect::SourceDirect 
( const LoKi::Tracks::SourceDirect& right ) 
  : LoKi::AuxFunBase                    ( right ) 
  , LoKi::Tracks::SourceDirect::_Source ( right )
  , m_data ( right.m_data ) 
{}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Tracks::SourceDirect::result_type 
LoKi::Tracks::SourceDirect::operator() () const 
{
  Assert ( 0 != m_data , "Invalid pointer to std::vector<LHCb::Track*>" ) ;
  return result_type ( m_data->begin() , m_data->end() ) ;
}
// ============================================================================
std::ostream& 
LoKi::Tracks::SourceDirect::fillStream ( std::ostream& o ) const 
{ return o << "TrSOURCEDIRECT" ; }
// ============================================================================
// set the data 
// ============================================================================
void LoKi::Tracks::SourceDirect::setData ( const LoKi::ITrHybridTool* value ) 
{ if ( 0 != value ) { m_data = value -> tracks () ; } }
// ============================================================================






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
  const LoKi::TrackTypes::RVCuts& cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::RecVertices::SourceTES::_Source () 
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
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::RecVertices::SourceTES::_Source () 
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
      m_cut.fun ()       , 
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
// constructor from the data:
// ============================================================================
LoKi::RecVertices::SourceDirect::SourceDirect 
( const LHCb::RecVertex::Vector* data  )
  : LoKi::RecVertices::SourceDirect::_Source ()
  , m_data ( data ) 
{}
// ============================================================================
// constructor from tool 
// ============================================================================
LoKi::RecVertices::SourceDirect::SourceDirect 
( const LoKi::ITrHybridTool* tool  )
  : LoKi::RecVertices::SourceDirect::_Source ()
  , m_data ( 0  ) 
{
  if ( 0 != tool ) { m_data = tool -> vertices () ; }  
}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::RecVertices::SourceDirect::SourceDirect 
( const LoKi::RecVertices::SourceDirect& right ) 
  : LoKi::AuxFunBase                    ( right ) 
  , LoKi::RecVertices::SourceDirect::_Source ( right )
  , m_data ( right.m_data ) 
{}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::RecVertices::SourceDirect::result_type 
LoKi::RecVertices::SourceDirect::operator() () const 
{
  Assert ( 0 != m_data , "Invalid pointer to std::vector<LHCb::RecVertex*>" ) ;
  return result_type ( m_data->begin() , m_data->end() ) ;
}
// ============================================================================
std::ostream& 
LoKi::RecVertices::SourceDirect::fillStream ( std::ostream& o ) const 
{ return o << "RVSOURCEDIRECT" ; }
// ============================================================================
// set the data 
// ============================================================================
void LoKi::RecVertices::SourceDirect::setData ( const LoKi::ITrHybridTool* value ) 
{ if ( 0 != value ) { m_data = value -> vertices () ; } }
// ============================================================================



  

// ============================================================================
// The END 
// ============================================================================
