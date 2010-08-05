// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetData.h"
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
  LHCb::Track::ConstVector output ;
  //
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  {
    LHCb::Track::Range r = get ( *iaddr ) ;
    //
    /// select the tracks 
    LoKi::apply_filter  
      ( r.begin () , 
        r.end   () , 
        m_cut.func ()      , 
        std::back_inserter ( output ) ) ;
  }
  //
  return output ;
}
// ============================================================================
// get the particles from the certain  TES location 
// ============================================================================
LHCb::Track::Range LoKi::Tracks::SourceTES::get
( const std::string& location ) const 
{
  Gaudi::Utils::GetData<LHCb::Track::Range> data ;
  //
  SmartDataPtr<LHCb::Track::Selection> tracks_1 ( m_dataSvc , location ) ;
  if ( !(!tracks_1) ) { return data.make_range ( tracks_1 ) ; }
  //
  SmartDataPtr<LHCb::Track::Container> tracks_2 ( m_dataSvc , location ) ;
  if ( !(!tracks_2) ) { return data.make_range ( tracks_2 ) ; }
  //
  Exception ( "No valid data is found at location '" + location + "'") ;
  //
  return LHCb::Track::Range() ;
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
// The END 
// ============================================================================
