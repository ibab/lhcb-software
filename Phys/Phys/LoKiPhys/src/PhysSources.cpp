// $Id$
// ============================================================================
// Include files 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// DaVinciInterfaces 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysSources.h"
#include "LoKi/Services.h"
#include "LoKi/Algs.h"
#include "LoKi/PhysExtract.h"
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
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV ibelyav@physics.syr.edu
 *  @date 2006-12-07
 */
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const std::string&           path , 
  IDataProviderSvc*            svc  , 
  const LoKi::PhysTypes::Cuts& cuts ) 
  : LoKi::Particles::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  , 
  const LoKi::PhysTypes::Cuts&    cuts ) 
  : LoKi::Particles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const std::string&           path , 
  const LoKi::PhysTypes::Cuts& cuts , 
  IDataProviderSvc*            svc  ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Particles::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::Cuts&    cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Particles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const LoKi::PhysTypes::Cuts& cuts ,
  const std::string&           path , 
  IDataProviderSvc*            svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::Particles::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const LoKi::PhysTypes::Cuts&    cuts ,
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::Particles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const LoKi::PhysTypes::Cuts& cuts ,
  IDataProviderSvc*            svc  ,
  const std::string&           path ) 
  : LoKi::Particles::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const LoKi::PhysTypes::Cuts&    cuts ,
  IDataProviderSvc*               svc  ,
  const std::vector<std::string>& path ) 
  : LoKi::Particles::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::SourceTES::SourceTES 
( const LoKi::Particles::SourceTES& right ) 
  : LoKi::AuxFunBase                    ( right ) 
  , LoKi::Particles::SourceTES::_Source ( right ) 
  , m_path    ( right.m_path    ) 
  , m_dataSvc ( right.m_dataSvc ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::SourceTES::~SourceTES()
{
  if ( m_dataSvc && !gaudi() ) 
  {
    // Warning("IDataProviderSvc: manual reset") ;
    m_dataSvc.reset() ;
  }
}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Particles::SourceTES::result_type 
LoKi::Particles::SourceTES::operator() () const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  Assert ( !m_path.empty() , "No TES locations are specified!" ) ;
  //
  LHCb::Particle::ConstVector output ;
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  { get ( *iaddr , output ) ; }
  //
  return output ;
}
// ============================================================================
// get the particles from the certain  TES location 
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class CONTAINER, class OUTPUT, class PREDICATE>
  inline std::size_t _fill_ 
  ( CONTAINER& cnt , 
    OUTPUT&    out , 
    const PREDICATE& cut ) 
  {
    const std::size_t n = out.size() ;
    out.reserve ( n + cnt->size() ) ;
    std::copy_if ( cnt->begin () , 
                   cnt->end   () , 
                   std::back_inserter ( out ) , std::cref(cut) ) ;
    return out.size() - n ;
  } 
  // ==========================================================================  
  template <class CONTAINER, class PREDICATE>
  inline std::size_t _count_ 
  ( CONTAINER& cnt , 
    const PREDICATE& cut ) 
  {
    return LoKi::Algs::count_if ( cnt->begin () , 
                                  cnt->end   () , 
                                  cut          ) ;
  } 
  // ==========================================================================
}
// ============================================================================
// get the particles from the certain  TES location 
// ============================================================================
std::size_t LoKi::Particles::SourceTES::get 
( const std::string&           location , 
  LHCb::Particle::ConstVector& output   ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  //
  SmartDataPtr<LHCb::Particle::Selection> parts1 ( m_dataSvc , location ) ;
  if ( !(!parts1) ) { return _fill_ ( parts1 , output , m_cut.func() ) ; }
  SmartDataPtr<LHCb::Particle::Container> parts2 ( m_dataSvc , location ) ;
  if ( !(!parts2) ) { return _fill_ ( parts2 , output , m_cut.func() ) ; }
  
  // check the suffix of type "/Particles" and add if needed
  static const std::string s_particles = "/Particles" ;
  if ( location.size() != location.rfind ( s_particles ) + s_particles.size() ) 
  { return get ( location + s_particles , output ) ; }
  //
  Assert ( false , "No valid data is found at location '" + location + "'") ;
  //
  return 0 ;
}
// ============================================================================
// count the particles from the certain  TES location 
// ============================================================================
std::size_t LoKi::Particles::SourceTES::count
( const std::string&           location ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  //
  SmartDataPtr<LHCb::Particle::Selection> parts1 ( m_dataSvc , location ) ;
  if ( !(!parts1) ) { return _count_ ( parts1 , m_cut.func() ) ; }
  SmartDataPtr<LHCb::Particle::Container> parts2 ( m_dataSvc , location ) ;
  if ( !(!parts2) ) { return _count_ ( parts2 , m_cut.func() ) ; }
  
  // check the suffix of type "/Particles" and add if needed
  static const std::string s_particles = "/Particles" ;
  if ( location.size() != location.rfind ( s_particles ) + s_particles.size() ) 
  { return count ( location + s_particles ) ; }
  //
  Error ( "No valid data is found at location '" + location + "'" ) ;
  //
  return 0 ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << "SOURCE(" 
           << Gaudi::Utils::toString( m_path ) << "," << m_cut << ")" ; }
// ============================================================================


// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::TESCounter::TESCounter 
( const std::string&           path , 
  const LoKi::PhysTypes::Cuts& cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Particles::TESCounter::TESCounter 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::Cuts&    cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::TESCounter::~TESCounter () {}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Particles::TESCounter::result_type 
LoKi::Particles::TESCounter::operator() ( /* argument */ ) const 
{
  //
  unsigned int num = 0 ;
  //
  typedef std::vector<std::string> List ;
  //
  const List& paths = m_source.paths() ;
  for ( List::const_iterator item = paths.begin() ; paths.end() != item ; ++item ) 
  {
    num += m_source.count ( *item ) ;
  }
  //
  return num ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::TESCounter::fillStream ( std::ostream& o ) const 
{ return o << "NUMBER(" 
           << Gaudi::Utils::toString( m_source.paths() )
           << "," << m_source.cut() << ")" ; }
// ============================================================================





// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const IDVAlgorithm*          desktop , 
  const LoKi::PhysTypes::Cuts& cuts    ) 
  : LoKi::AuxFunBase ( std::tie ( cuts ) ) 
  , LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::Cuts& cuts    ,
  const IDVAlgorithm*          desktop )
  : LoKi::AuxFunBase ( std::tie ( cuts ) ) 
  , LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const LoKi::Particles::SourceDesktop& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::Particles::SourceDesktop::_Source ( right ) 
  , m_desktop ( right.m_desktop ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::SourceDesktop::~SourceDesktop()
{
  if ( m_desktop && !gaudi() ) 
  {
    // Warning("IPhysDesktop: manual reset") ;
    m_desktop.reset () ;
  }
}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Particles::SourceDesktop::result_type 
LoKi::Particles::SourceDesktop::operator() () const 
{
  if ( !m_desktop ) 
  { 
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    const IDVAlgorithm* alg = Gaudi::Utils::getIDVAlgorithm ( svcs.contextSvc() ) ;
    if ( 0 != alg ) { m_desktop = alg ; }
    //
    Assert ( m_desktop               ,
             "Could not locate valid IDVAlgorithm" ) ;
  }
  //
  LHCb::Particle::Range input = m_desktop -> particles() ;
  //
  LHCb::Particle::ConstVector output ;
  output.reserve ( input.size() ) ;
  // use cuts:
  std::copy_if 
    ( input.begin () , 
      input.end   () , 
      std::back_inserter ( output ) , 
      std::cref(m_cut.func()) ) ;
  //
  return output ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::SourceDesktop::fillStream ( std::ostream& o ) const 
{ return o << "SOURCEDESKTOP( " << m_cut << ")" ; }
// ============================================================================


// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const std::string&            path , 
  IDataProviderSvc*             svc  , 
  const LoKi::PhysTypes::VCuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const std::vector<std::string>&  path , 
  IDataProviderSvc*                svc  , 
  const LoKi::PhysTypes::VCuts&    cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts , 
  IDataProviderSvc*             svc  )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const LoKi::PhysTypes::VCuts& cuts ,
  const std::string&            path , 
  IDataProviderSvc*             svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const LoKi::PhysTypes::VCuts&   cuts ,
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts , path ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const LoKi::PhysTypes::VCuts& cuts ,
  IDataProviderSvc*             svc  ,
  const std::string&            path ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const LoKi::PhysTypes::VCuts&   cuts ,
  IDataProviderSvc*               svc  ,
  const std::vector<std::string>& path ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Vertices::SourceTES::_Source () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const LoKi::Vertices::SourceTES& right ) 
  : LoKi::AuxFunBase                    ( right ) 
  , LoKi::Vertices::SourceTES::_Source ( right ) 
  , m_path    ( right.m_path    ) 
  , m_dataSvc ( right.m_dataSvc ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::SourceTES::~SourceTES()
{
  if ( m_dataSvc && !gaudi() ) 
  {
    // Warning("IDataProviderSvc: manual reset") ;
    m_dataSvc.reset() ;
  }
}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Vertices::SourceTES::result_type 
LoKi::Vertices::SourceTES::operator() () const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  Assert ( !m_path.empty() , "No TES locations are specified!" ) ;
  //
  LHCb::VertexBase::ConstVector output ;
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  { get ( *iaddr , output ) ; }
  //
  return output ;
}
// ============================================================================
// get the vertices from the certain  TES location 
// ============================================================================
std::size_t LoKi::Vertices::SourceTES::get 
( const std::string&           location , 
  LHCb::VertexBase::ConstVector& output   ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }  
  //
  SmartDataPtr<LHCb::RecVertex::Container>  v1 ( m_dataSvc , location ) ;
  if (  !(!v1) ) { return _fill_ ( v1 , output , m_cut.func() ) ; }
  SmartDataPtr<LHCb::RecVertex::Selection>  v2 ( m_dataSvc , location ) ;
  if (  !(!v2) ) { return _fill_ ( v2 , output , m_cut.func() ) ; }
  //
  SmartDataPtr<LHCb::Vertex::Selection>     v3 ( m_dataSvc , location ) ;
  if (  !(!v3) ) { return _fill_ ( v3 , output , m_cut.func() ) ; }
  SmartDataPtr<LHCb::Vertex::Container>     v4 ( m_dataSvc , location ) ;
  if (  !(!v4) ) { return _fill_ ( v4 , output , m_cut.func() ) ; }
  //
  SmartDataPtr<LHCb::VertexBase::Container> v5 ( m_dataSvc , location ) ;
  if (  !(!v5) ) { return _fill_ ( v5 , output , m_cut.func() ) ; }
  
  // check the suffix of type "/Vertices" and add if needed
  static const std::string s_vertices = "/Vertices" ;
  if ( location.size() != location.rfind ( s_vertices ) + s_vertices.size() ) 
  { return get ( location + s_vertices , output ) ; }
  //
  Assert ( false , "No valid data is found at location '" + location + "'") ;
  //
  return 0 ; 
}
// ============================================================================
// get the vertices from the certain  TES location 
// ============================================================================
std::size_t LoKi::Vertices::SourceTES::count 
( const std::string&           location ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }  
  //
  SmartDataPtr<LHCb::RecVertex::Container>  v1 ( m_dataSvc , location ) ;
  if (  !(!v1) ) { return _count_ ( v1 , m_cut.func() ) ; }
  SmartDataPtr<LHCb::RecVertex::Selection>  v2 ( m_dataSvc , location ) ;
  if (  !(!v2) ) { return _count_ ( v2 , m_cut.func() ) ; }
  //
  SmartDataPtr<LHCb::Vertex::Selection>     v3 ( m_dataSvc , location ) ;
  if (  !(!v3) ) { return _count_ ( v3 , m_cut.func() ) ; }
  SmartDataPtr<LHCb::Vertex::Container>     v4 ( m_dataSvc , location ) ;
  if (  !(!v4) ) { return _count_ ( v4 , m_cut.func() ) ; }
  //
  SmartDataPtr<LHCb::VertexBase::Container> v5 ( m_dataSvc , location ) ;
  if (  !(!v5) ) { return _count_ ( v5 , m_cut.func() ) ; }
  //
  // check the suffix of type "/Vertices" and add if needed
  static const std::string s_vertices = "/Vertices" ;
  if ( location.size() != location.rfind ( s_vertices ) + s_vertices.size() ) 
  { return count ( location + s_vertices ) ; }
  //
  Error ( "No valid data is found at location '" + location + "'") ;
  //
  return 0 ; 
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Vertices::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << "VSOURCE(" 
           << Gaudi::Utils::toString ( m_path ) << "," << m_cut << ")" ; }
// ============================================================================

// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const IDVAlgorithm*           desktop , 
  const LoKi::PhysTypes::VCuts& cuts    ) 
  : LoKi::AuxFunBase ( std::tie ( cuts ) ) 
  , LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::VCuts& cuts    ,
  const IDVAlgorithm*           desktop )
  : LoKi::AuxFunBase ( std::tie ( cuts ) ) 
  , LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// copy constructor
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const LoKi::Vertices::SourceDesktop& right ) 
  : LoKi::AuxFunBase                        ( right ) 
  , LoKi::Vertices::SourceDesktop::_Source ( right ) 
  , m_desktop ( right.m_desktop ) 
  , m_cut     ( right.m_cut     ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::SourceDesktop::~SourceDesktop()
{
  if ( m_desktop && !gaudi() ) 
  {
    // Warning("IPhysDesktop: manual reset") ;
    m_desktop.reset() ;
  } 
}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Vertices::SourceDesktop::result_type 
LoKi::Vertices::SourceDesktop::operator() () const 
{
  if ( !m_desktop ) 
  { 
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    const IDVAlgorithm* alg = Gaudi::Utils::getIDVAlgorithm ( svcs.contextSvc() ) ;
    if ( alg ) { m_desktop = alg ; }
    //
    Assert ( m_desktop               ,
             "Could not locate valid IDVAlgorithm" ) ;
  }
  //  
  const LHCb::RecVertex::Range input2 = 
    m_desktop -> primaryVertices   () ;
  //
  if ( input2.empty() ) 
  { Warning ( "No input primary vertices from IDVAlgorithm" ) ; }
  
  LHCb::VertexBase::ConstVector output ;
  output.reserve ( input2.size() ) ;
  // use cuts:
  std::copy_if ( input2.begin () , 
                 input2.end   () , 
                 std::back_inserter ( output ) , 
                 std::cref(m_cut.func ()) ) ;
  //
  if ( output.empty() ) 
  { Warning ("No vertices are selected by '" + m_cut.printOut() + "'" ) ; }
  //
  return output ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Vertices::SourceDesktop::fillStream ( std::ostream& o ) const 
{ return o << "VSOURCEDESKTOP( " << m_cut << ")" ; }
// ============================================================================
// set the  desktop
// ============================================================================




// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::TESCounter::TESCounter 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::TESCounter::TESCounter 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::TESCounter::~TESCounter () {}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Vertices::TESCounter::result_type 
LoKi::Vertices::TESCounter::operator() ( /* argument */ ) const 
{
  //
  unsigned int num = 0 ;
  //
  typedef std::vector<std::string> List ;
  //
  const List& paths = m_source.paths() ;
  for ( List::const_iterator item = paths.begin() ; 
        paths.end() != item ; ++item ) 
  { num += m_source.count ( *item ) ; }
  //
  return num ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Vertices::TESCounter::fillStream ( std::ostream& o ) const 
{ return o << "VNUMBER(" 
           << Gaudi::Utils::toString( m_source.paths() )
           << "," << m_source.cut() << ")" ; }
// ============================================================================

// ============================================================================
// constructor from the optional cuts 
// ============================================================================
LoKi::Particles::Flatten::Flatten
( const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut ) 
  : LoKi::AuxFunBase ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Pipe () 
  ,  m_cut ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::Flatten::~Flatten(){}
// ============================================================================
// MANDATORY: clone method("virtual constructor")
// ============================================================================
LoKi::Particles::Flatten*
LoKi::Particles::Flatten::clone() const 
{ return new LoKi::Particles::Flatten ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::Flatten::result_type
LoKi::Particles::Flatten::operator() 
  ( LoKi::Particles::Flatten::argument a ) const 
{
  //
  result_type flatten ;
  if ( a.empty() ) { return flatten ; }  // RETURN 
  //
  flatten.reserve ( 6 * a.size() ) ;
  //
  LoKi::Extract::getParticles ( a.begin ()                     , 
                                a.end   ()                     , 
                                std::back_inserter ( flatten ) ,
                                m_cut                          ) ;
  //
  return flatten ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::Flatten::fillStream ( std::ostream& o ) const 
{ return o << " FLATTEN(" << m_cut << ") " ; }
// ============================================================================



 



// ============================================================================
// The END 
// ============================================================================
