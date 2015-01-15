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
#include "LoKi/Algs.h"
// ============================================================================
/** @file 
 *  Implementation file for various sources
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
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
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie (  cuts , path ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie (  cuts , path ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie (  cuts , path ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
  : LoKi::AuxFunBase ( std::tie (  cuts , path ) ) 
  , LoKi::Tracks::SourceTES::_Source () 
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
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::SourceTES* LoKi::Tracks::SourceTES::clone() const 
{ return new SourceTES(*this) ; }
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
( const std::string& location , 
  const bool         exc      ) const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //  
  Gaudi::Utils::GetData<LHCb::Track::Range> data ;
  //
  SmartDataPtr<LHCb::Track::Selection> tracks_1 ( m_dataSvc , location ) ;
  if ( !(!tracks_1) ) { return data.make_range ( tracks_1 ) ; }
  //
  SmartDataPtr<LHCb::Track::Container> tracks_2 ( m_dataSvc , location ) ;
  if ( !(!tracks_2) ) { return data.make_range ( tracks_2 ) ; }
  //
  if ( exc ) 
  { Exception ( "No valid data is found at location '" + location + "'") ; }
  else 
  { Error     ( "No valid data is found at location '" + location + "'") ; }
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
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::TESCounter::TESCounter 
( const std::string&              path , 
  const LoKi::TrackTypes::TrCuts& cuts )
  : LoKi::AuxFunBase ( std::tie (  path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Tracks::TESCounter::TESCounter 
( const std::vector<std::string>& path                    , 
  const LoKi::TrackTypes::TrCuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Tracks::TESCounter::~TESCounter(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::TESCounter*
LoKi::Tracks::TESCounter::clone() const 
{ return new LoKi::Tracks::TESCounter( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Tracks::TESCounter::result_type 
LoKi::Tracks::TESCounter::operator() ( /* argument */ ) const 
{
  unsigned long num = 0 ;
  
  typedef std::vector<std::string> List ;
  const List& paths = m_source.paths() ;
  for ( List::const_iterator iaddr = paths.begin() ; 
        paths.end() != iaddr ; ++iaddr ) 
  {
    // get tracks 
    LHCb::Track::Range r = m_source.get ( *iaddr , false ) ;
    // count tracks 
    num += LoKi::Algs::count_if ( r.begin () , r.end () , m_source.cut() ) ;
  }
  //
  return num ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Tracks::TESCounter::fillStream ( std::ostream& o ) const 
{ return o << "TrNUM(" 
           << Gaudi::Utils::toString( m_source.paths() ) 
           << "," << m_source.cut() << ")" ; }
// ============================================================================





// ============================================================================
// The END 
// ============================================================================
