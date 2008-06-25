// $Id: PhysSources.cpp,v 1.3 2008-06-25 10:09:41 ibelyaev Exp $
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
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPhysDesktop.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysSources.h"
#include "LoKi/Services.h"
#include "LoKi/select.h"
// ============================================================================
/** @file 
 *  Implementation file for various sources
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
  const LoKi::PhysTypes::Cuts&    cuts , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Particles::SourceTES::_Source () 
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
  const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  ) 
  : LoKi::Particles::SourceTES::_Source () 
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
LoKi::Particles::SourceTES::~SourceTES(){}
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
    Assert ( m_dataSvc.validPointer ( )               ,
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
std::size_t LoKi::Particles::SourceTES::get 
( const std::string&           location , 
  LHCb::Particle::ConstVector& output   ) const 
{
  static const std::string s_particles = "/Particles" ;
  //
  SmartDataPtr<LHCb::Particle::Container> parts ( m_dataSvc , location ) ;
  //
  // check the suffix of type "/Particles" and add if needed
  if ( !parts &&
       location.size() != location.rfind ( s_particles ) + s_particles.size() ) 
  { return get ( location + s_particles , output ) ; }
  //
  const LHCb::Particle::Container* particles = parts ;
  Assert ( 0 != particles , 
           "No valid data is found at location '" + location + "'") ;
  //
  const size_t n = output.size() ;
  output.reserve ( n + particles -> size() ) ;
  /// select the particles 
  LoKi::select 
    ( particles -> begin () , 
      particles -> end   () , 
      std::back_inserter ( output ) , m_cut.fun() ) ;
  //
  return output.size() - n ;
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
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const IPhysDesktop*          desktop , 
  const LoKi::PhysTypes::Cuts& cuts    ) 
  : LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::Cuts& cuts    ,
  const IPhysDesktop*          desktop )
  : LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from DVAlgorithm & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const DVAlgorithm*           alg  , 
  const LoKi::PhysTypes::Cuts& cuts ) 
  : LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( (IPhysDesktop*) 0 ) 
  , m_cut     ( cuts    )
{
  if ( 0 != alg ) { m_desktop = alg->desktop() ; }
}
// ============================================================================
// constructor from DVAlgorithm & cuts 
// ============================================================================
LoKi::Particles::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::Cuts& cuts ,
  const DVAlgorithm*           alg  )
  : LoKi::Particles::SourceDesktop::_Source () 
  , m_desktop ( (IPhysDesktop*) 0 ) 
  , m_cut     ( cuts    )
{
  if ( 0 != alg ) { m_desktop = alg->desktop() ; }
}
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
LoKi::Particles::SourceDesktop::~SourceDesktop(){}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Particles::SourceDesktop::result_type 
LoKi::Particles::SourceDesktop::operator() () const 
{
  if ( !m_desktop ) 
  { 
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    DVAlgorithm* alg = 
      Gaudi::Utils::getDVAlgorithm ( svcs.contextSvc() ) ;
    if ( 0 != alg ) 
    {
      if ( alg->msgLevel( MSG::DEBUG) ) 
      { 
        alg->debug() 
          << objType() << "('" << (*this) << "')"
          << " : Use DVAlgorithm('" 
          << alg->name() 
          << "') to access IPhysDesktop"
          << endreq ;
      }
      m_desktop = alg->desktop() ; 
    }
    Assert ( m_desktop.validPointer ( )               ,
             "Could not locate valid IPhysDesktop" ) ;
  }
  //
  const LHCb::Particle::ConstVector& input = m_desktop -> particles() ;
  LHCb::Particle::ConstVector output ;
  output.reserve ( input.size() ) ;
  // use cuts:
  LoKi::select 
    ( input.begin () , 
      input.end   () , 
      std::back_inserter ( output ) , 
      m_cut.fun() ) ;
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
// set the  desktop
// ============================================================================
void 
LoKi::Particles::SourceDesktop::setDesktop 
( const                 DVAlgorithm*   value ) 
{ if ( 0 != value ) { m_desktop = value -> desktop() ; } }
// ============================================================================


// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::Vertices::SourceTES::SourceTES 
( const std::string&            path , 
  IDataProviderSvc*             svc  , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
  : LoKi::Vertices::SourceTES::_Source () 
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
LoKi::Vertices::SourceTES::~SourceTES(){}
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
    Assert ( m_dataSvc.validPointer ( )               ,
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
  static const std::string s_vertices = "/Vertices" ;
  //
  const size_t n = output.size() ;
  //
  { /// try to get reconstructed(primary) vertices 
    SmartDataPtr<LHCb::RecVertex::Container>  v ( m_dataSvc , location ) ;
    if ( !v ) {}
    else
    {
      output.reserve ( n + v -> size() ) ;    
      //
      LoKi::select 
        ( v -> begin () , 
          v -> end   () , 
          std::back_inserter ( output ) , m_cut.fun() ) ;
      //
      return output.size() - n ;                                     // RETURN 
      //    
    } 
  }
  //
  { /// try to get physics(secondary) vertices 
    SmartDataPtr<LHCb::Vertex::Container>  v ( m_dataSvc , location ) ;
    if ( !v ) {}
    else
    {
      output.reserve ( n + v -> size() ) ;    
      //
      LoKi::select 
        ( v -> begin () , 
          v -> end   () , 
          std::back_inserter ( output ) , m_cut.fun() ) ;
      //
      return output.size() - n ;                                     // RETURN
      //    
    } 
  }
  //
  { /// try to get physics(secondary) vertices 
    SmartDataPtr<LHCb::VertexBase::Container>  v ( m_dataSvc , location ) ;
    if ( !v ) {}
    else 
    {
      output.reserve ( n + v -> size() ) ;    
      //
      LoKi::select 
        ( v -> begin () , 
          v -> end   () , 
          std::back_inserter ( output ) , m_cut.fun() ) ;
      //
      return output.size() - n ;                                    // RETURN 
      //    
    } 
  }
  // check the suffix of type "/Vertices" and add if needed
  if ( location.size() != location.rfind ( s_vertices ) + s_vertices.size() ) 
  { return get ( location + s_vertices , output ) ; }
  //
  Assert ( false , 
           "No valid data is found at location '" + location + "'") ;
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
( const IPhysDesktop*           desktop , 
  const LoKi::PhysTypes::VCuts& cuts    ) 
  : LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from the desktop & cuts 
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::VCuts& cuts    ,
  const IPhysDesktop*           desktop )
  : LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( desktop ) 
  , m_cut     ( cuts    )
{}
// ============================================================================
// constructor from DVAlgorithm & cuts 
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const DVAlgorithm*            alg  , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( (IPhysDesktop*) 0 ) 
  , m_cut     ( cuts    )
{
  if ( 0 != alg ) { m_desktop = alg->desktop() ; }
}
// ============================================================================
// constructor from DVAlgorithm & cuts 
// ============================================================================
LoKi::Vertices::SourceDesktop::SourceDesktop 
( const LoKi::PhysTypes::VCuts& cuts ,
  const DVAlgorithm*            alg  )
  : LoKi::Vertices::SourceDesktop::_Source () 
  , m_desktop ( (IPhysDesktop*) 0 ) 
  , m_cut     ( cuts    )
{
  if ( 0 != alg ) { m_desktop = alg->desktop() ; }
}
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
LoKi::Vertices::SourceDesktop::~SourceDesktop(){}
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::Vertices::SourceDesktop::result_type 
LoKi::Vertices::SourceDesktop::operator() () const 
{
  if ( !m_desktop ) 
  { 
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    DVAlgorithm* alg = 
      Gaudi::Utils::getDVAlgorithm ( svcs.contextSvc() ) ;
    if ( 0 != alg ) 
    {
      if ( alg->msgLevel( MSG::DEBUG) ) 
      { 
        alg->debug() 
          << objType() << "('" << (*this) << "')"
          << " : Use DVAlgorithm('" 
          << alg->name() 
          << "') to access IPhysDesktop"
          << endreq ;
      }
      m_desktop = alg->desktop() ; 
    }
    Assert ( m_desktop.validPointer ( )               ,
             "Could not locate valid IPhysDesktop" ) ;
  }
  //  
  const LHCb::Vertex::ConstVector&    input1 = 
    m_desktop -> secondaryVertices () ;
  const LHCb::RecVertex::ConstVector& input2 = 
    m_desktop -> primaryVertices   () ;
  //
  if ( input1.empty() && input2.empty() ) 
  { Warning ( "No input vertices (secondary&primary) from Desktop" ) ; }
  
  LHCb::VertexBase::ConstVector output ;
  output.reserve ( input1.size() + input2.size() ) ;
  // use cuts:
  LoKi::select ( input1.begin () , 
                 input1.end   () , 
                 std::back_inserter ( output ) , 
                 m_cut.fun() ) ;
  LoKi::select ( input2.begin () , 
                 input2.end   () , 
                 std::back_inserter ( output ) , 
                 m_cut.fun() ) ;
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
void 
LoKi::Vertices::SourceDesktop::setDesktop 
( const                 DVAlgorithm*   value ) 
{ if ( 0 != value ) { m_desktop = value -> desktop() ; } }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
