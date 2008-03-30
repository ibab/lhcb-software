// $Id: Particles20.cpp,v 1.4 2008-03-30 13:43:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IPhysDesktop.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/DVAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Vertices0.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles16.h"
#include "LoKi/Particles20.h"
#include "LoKi/PhysSources.h"
// ============================================================================
/** @file 
 *  The implementation file for functions form the file LoKi/Particles20.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
// ============================================================================
/// anonymous namespace to hide some pure technical stuff
namespace
{
  // ==========================================================================
  /// "invalid" pointer 
  const IGeomDispCalculator* const s_IGEO  = 0 ;
  /// "invalid" pointer 
  const ILifetimeFitter*     const s_LTIME = 0 ;
  // ==========================================================================
  /** @var s_IPTOOL 
   *  the instance of "invalid" IP-tool
   */
  const LoKi::Vertices::ImpactParamTool s_IPTOOL ( s_IGEO ) ;
  // ==========================================================================
  /// "invalid" vertex 
  const LHCb::VertexBase* const s_VERTEX = 0 ;
  // ==========================================================================
  /** get the IGeomDispCalculator tool from DValgorith
   *  @param tool to be updated 
   *  @param loki service 
   *  @param name nickname of full type/name of the tool
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2008-01-16
   */  
  inline 
  const IGeomDispCalculator* 
  loadTool 
  ( const LoKi::Vertices::ImpactParamTool& tool      ,
    const LoKi::ILoKiSvc*                  loki      ,
    const std::string&                     name = "" ) 
  {
    if ( 0 == loki ) { return 0 ; }                            // RETURN
    // get DVAlgorithm form the context 
    DVAlgorithm* alg = Gaudi::Utils::getDVAlgorithm ( loki -> contextSvc() ) ;
    if ( 0 == alg  ) { return 0 ; }                            // RETURN 
    // get the tool form the algorithm
    const IGeomDispCalculator* geo = alg -> geomDispCalculator ( name ) ;
    // set the tool 
    tool.setTool ( geo ) ;
    return geo ;                                               // RETURN
  }
  // ==========================================================================
  /** get ILifetimeFitter tool from DVAlgorithm
   *  @param func functor to be updated 
   *  @param loki service 
   *  @param name full type/name of the tool
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2008-01-16
   */
  template <class TYPE>
  inline 
  const ILifetimeFitter*
  loadFitter  
  ( const TYPE&            func , 
    const LoKi::ILoKiSvc*  loki ,
    const std::string&     name )
  {
    if ( 0 == loki ) { return  0 ; }                           // RETURN 
    // get DVAlgorithm form the context 
    DVAlgorithm* alg = Gaudi::Utils::getDVAlgorithm ( loki -> contextSvc() ) ;
    if ( 0 == alg  ) { return 0 ; }                            // RETURN 
    // get the tool from DValgorithm
    const ILifetimeFitter* fitter = alg -> lifetimeFitter ( name ) ;
    // set the functor 
    func.setTool ( fitter ) ;
    return fitter ;                                            // RETURN 
  }
  // ==========================================================================
  /// the finder for the primary vertices:
  const LoKi::Vertices::IsPrimary s_PRIMARY = LoKi::Vertices::IsPrimary () ;
  // ==========================================================================
  /// the defautl name of Lifetime fiteter:
  const std::string s_LIFETIME = "PropertimeFitter/properTime:PUBLIC" ;
  // ==========================================================================
} // end of anonymout namespace 
// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV::
CosineDirectionAngleWithTheBestPV()
  : LoKi::AuxDesktopBase()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_dira    ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV* 
LoKi::Particles::CosineDirectionAngleWithTheBestPV::clone() const 
{ return new CosineDirectionAngleWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV::result_type 
LoKi::Particles::CosineDirectionAngleWithTheBestPV::operator() 
  ( LoKi::Particles::CosineDirectionAngleWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, retuen -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  m_dira.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_dira ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::CosineDirectionAngleWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVDIRA" ; }
// ============================================================================
// the default constructor, creates the object in invalid state 
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV::ImpParWithTheBestPV 
( const std::string& geo ) 
  : LoKi::AuxDesktopBase()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_ip  ( s_VERTEX , s_IPTOOL ) 
  , m_geo ( geo )   
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV*
LoKi::Particles::ImpParWithTheBestPV::clone() const 
{ return  new ImpParWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV::result_type 
LoKi::Particles::ImpParWithTheBestPV::operator() 
  ( LoKi::Particles::ImpParWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the IGeomDispCalculator from DVAlgorithm 
  if ( 0 == m_ip.tool() ) { loadTool ( m_ip , lokiSvc() , m_geo )  ; }
  // check it!
  Assert ( 0 != m_ip.tool() , "No valid IGeomDispCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  m_ip.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_ip.ip ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::ImpParWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVIP ('" <<  m_geo << "')"; }
// ============================================================================
// the default constructor, creates the object in invalid state 
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV::ImpParChi2WithTheBestPV 
( const std::string& geo ) 
  : LoKi::AuxDesktopBase()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_ip  ( s_VERTEX , s_IPTOOL ) 
  , m_geo ( geo )   
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV*
LoKi::Particles::ImpParChi2WithTheBestPV::clone() const 
{ return  new ImpParChi2WithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV::result_type 
LoKi::Particles::ImpParChi2WithTheBestPV::operator() 
  ( LoKi::Particles::ImpParChi2WithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the IGeomDispCalculator from DVAlgorithm 
  if ( 0 == m_ip.tool() ) { loadTool ( m_ip , lokiSvc() , m_geo )  ; }
  // check it!
  Assert ( 0 != m_ip.tool() , "No valid IGeomDispCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  m_ip.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_ip.chi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::ImpParChi2WithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVIPCHI2 ('" <<  m_geo << "')"; }
// ============================================================================
/*  constructor from the source and nickname or full type/name of 
 *  IGeomDispCalculator tool
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param source the source 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParWithSource::MinImpParWithSource 
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source ,
  const std::string&                                          geo    ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_mip    ( LHCb::VertexBase::ConstVector() , s_IPTOOL ) 
  , m_source ( source )   
  , m_geo    ( geo    )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParWithSource*
LoKi::Particles::MinImpParWithSource::clone() const 
{ return  new MinImpParWithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinImpParWithSource::result_type 
LoKi::Particles::MinImpParWithSource::operator() 
  ( LoKi::Particles::MinImpParWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IGeomDispCalculator from DVAlgorithm 
  if ( 0 == tool().tool() ) { loadTool ( m_mip , lokiSvc() , m_geo )  ; }
  // check the event 
  if ( !sameEvent() ) 
  {
    // clear the list of vertices 
    m_mip.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = m_source() ;
    // fill the functor with primary vertices:
    m_mip.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( m_mip.empty() ) 
    { Error ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return m_mip.mip ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MIPSOURCE ('" <<  m_source << ", " << m_geo << "')"; }
// ============================================================================
/* the "default" constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const std::string& geo    ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) , geo )  
{}
// ============================================================================
/*  the constructor form the vertex selection functor and 
 *  the name/nickname of IGeomDispCalculator tool from DVAlgorithm
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo )
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( cuts ) , geo )
{}
// ============================================================================
/*  the constructor form the vertex selection functor and 
 *  the name/nickname of IGeomDispCalculator tool from DVAlgorithm
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const std::string&            geo , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( cuts ) , geo )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParDV*
LoKi::Particles::MinImpParDV::clone() const 
{ return  new MinImpParDV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParDV::fillStream 
( std::ostream& s ) const 
{ return s << "MIPDV('" << geo() << "')"; }
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string& path   , 
  const std::string& geo    ) 
  : LoKi::Particles::MinImpParWithSource 
( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path   , 
  const std::string&              geo    ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const LoKi::PhysTypes::VCuts&   cuts ,
  const std::vector<std::string>& path , 
  const std::string&              geo  ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
    , m_path ( path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const LoKi::PhysTypes::VCuts& cuts ,
  const std::string&            path , 
  const std::string&            geo  ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts ,
  const std::string&              geo  ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts ,
  const std::string&            geo  ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path , 
  const std::string&              geo  , 
  const LoKi::PhysTypes::VCuts&   cuts ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
    , m_path ( path ) 
{}
// ============================================================================
/** the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string&            path , 
  const std::string&            geo  ,
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParTES*
LoKi::Particles::MinImpParTES::clone() const 
{ return  new MinImpParTES ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParTES::fillStream 
( std::ostream& s ) const 
{ return s << "MIPTES(" << Gaudi::Utils::toString ( m_path ) 
           << "," << geo() << "')"; }
// ============================================================================
/*  constructor from the source and nickname or full type/name of 
 *  IGeomDispCalculator tool
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param source the source 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource::MinImpParChi2WithSource 
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source ,
  const std::string&                                          geo    ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_mip    ( LHCb::VertexBase::ConstVector() , s_IPTOOL ) 
  , m_source ( source )   
  , m_geo    ( geo    )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource*
LoKi::Particles::MinImpParChi2WithSource::clone() const 
{ return  new MinImpParChi2WithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource::result_type 
LoKi::Particles::MinImpParChi2WithSource::operator() 
  ( LoKi::Particles::MinImpParChi2WithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IGeomDispCalculator from DVAlgorithm 
  if ( 0 == tool().tool() ) { loadTool ( m_mip , lokiSvc() , m_geo )  ; }
  // check the event 
  if ( !sameEvent() ) 
  {
    // clear the list of vertices 
    m_mip.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = m_source() ;
    // fill the functor with primary vertices:
    m_mip.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( m_mip.empty() ) { Error ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return m_mip.mipchi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2WithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2SOURCE ('" <<  m_source << ", " << m_geo << "')"; }
// ============================================================================
/* the "default" constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const std::string& geo    ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) , geo )  
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo  ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( cuts  ) , geo )  
{}
// ============================================================================
/*  the  constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const std::string&            geo  , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( cuts  ) , geo )  
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2DV*
LoKi::Particles::MinImpParChi2DV::clone() const 
{ return  new MinImpParChi2DV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2DV::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2DV('" << geo() << "')"; }
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string& path   , 
  const std::string& geo    ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path   , 
  const std::string&              geo    ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            path , 
  const std::string&            geo  ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
    , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const LoKi::PhysTypes::VCuts&   cuts , 
  const std::vector<std::string>& path , 
  const std::string&              geo  ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo  ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts , 
  const std::string&              geo  )  
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string&            path , 
  const std::string&            geo  ,
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IGeomDispCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::geomDispCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IGeomDispCalculator tool
 */
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path , 
  const std::string&              geo  , 
  const LoKi::PhysTypes::VCuts&   cuts )
  : LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2TES*
LoKi::Particles::MinImpParChi2TES::clone() const 
{ return  new MinImpParChi2TES ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2TES::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2TES(" << Gaudi::Utils::toString ( m_path ) 
           << "," << geo() << "')" ; 
}
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexDistanceDV::VertexDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDistanceDV*
LoKi::Particles::VertexDistanceDV::clone() const 
{ return new LoKi::Particles::VertexDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDistanceDV::result_type
LoKi::Particles::VertexDistanceDV::operator() 
  ( LoKi::Particles::VertexDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_fun ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVD" ; }
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV::VertexSignedDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV*
LoKi::Particles::VertexSignedDistanceDV::clone() const 
{ return new LoKi::Particles::VertexSignedDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV::result_type
LoKi::Particles::VertexSignedDistanceDV::operator() 
  ( LoKi::Particles::VertexSignedDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_fun ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexSignedDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDSIGN" ; }
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexDotDistanceDV::VertexDotDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDotDistanceDV*
LoKi::Particles::VertexDotDistanceDV::clone() const 
{ return new LoKi::Particles::VertexDotDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDotDistanceDV::result_type
LoKi::Particles::VertexDotDistanceDV::operator() 
  ( LoKi::Particles::VertexDotDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_fun ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexDotDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDDOT" ; }

// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV::VertexChi2DistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV*
LoKi::Particles::VertexChi2DistanceDV::clone() const 
{ return new LoKi::Particles::VertexChi2DistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV::result_type
LoKi::Particles::VertexChi2DistanceDV::operator() 
  ( LoKi::Particles::VertexChi2DistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  return m_fun ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexChi2DistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDCHI2" ; }
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeDV::LifeTimeDV() 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( s_LIFETIME         ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeDV::LifeTimeDV
( const std::string& fit ) 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( fit  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeDV*
LoKi::Particles::LifeTimeDV::clone() const 
{ return  new LoKi::Particles::LifeTimeDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeDV::result_type
LoKi::Particles::LifeTimeDV::operator() 
  ( LoKi::Particles::LifeTimeDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -InvalidTime") ;
    return -LoKi::Constants::InvalidTime ;
  }    
  // check the tool 
  if ( 0 == m_fun.tool() ) { loadFitter ( m_fun , lokiSvc() , m_fit ) ; }
  // check the fitter 
  Assert ( 0 != m_fun.tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the vertex from desktop 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  // use the functor 
  return m_fun ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeDV::fillStream 
( std::ostream& s ) const { return s << "BPVLTIME('" << m_fit << "')" ; }
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::LifeTimeChi2DV() 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( s_LIFETIME         ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::LifeTimeChi2DV
( const std::string& fit ) 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( fit  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeChi2DV*
LoKi::Particles::LifeTimeChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::result_type
LoKi::Particles::LifeTimeChi2DV::operator() 
  ( LoKi::Particles::LifeTimeChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }    
  // check the tool 
  if ( 0 == m_fun.tool() ) { loadFitter ( m_fun , lokiSvc() , m_fit ) ; }
  // check the fitter 
  Assert ( 0 != m_fun.tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the vertex from desktop 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  // use the functor 
  return m_fun ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeChi2DV::fillStream 
( std::ostream& s ) const { return s << "BPVLTCHI2('" << m_fit << "')" ; }
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::LifeTimeSignedChi2DV() 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX  ) 
  , m_fit ( s_LIFETIME          ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::LifeTimeSignedChi2DV
( const std::string& fit ) 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( fit  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV*
LoKi::Particles::LifeTimeSignedChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeSignedChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::result_type
LoKi::Particles::LifeTimeSignedChi2DV::operator() 
  ( LoKi::Particles::LifeTimeSignedChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }    
  // check the tool 
  if ( 0 == m_fun.tool() ) { loadFitter ( m_fun , lokiSvc() , m_fit ) ; }
  // check the fitter 
  Assert ( 0 != m_fun.tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the vertex from desktop 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  // use the functor 
  return m_fun ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeSignedChi2DV::fillStream 
( std::ostream& s ) const { return s << "BPVLTSIGNCHI2('" << m_fit << "')" ; }
// ============================================================================

// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::LifeTimeFitChi2DV() 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( s_LIFETIME         ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::LifeTimeFitChi2DV
( const std::string& fit ) 
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_LTIME , s_VERTEX ) 
  , m_fit ( fit  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV*
LoKi::Particles::LifeTimeFitChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeFitChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::result_type
LoKi::Particles::LifeTimeFitChi2DV::operator() 
  ( LoKi::Particles::LifeTimeFitChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }    
  // check the tool 
  if ( 0 == m_fun.tool() ) { loadFitter ( m_fun , lokiSvc() , m_fit ) ; }
  // check the fitter 
  Assert ( 0 != m_fun.tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the vertex from desktop 
  m_fun.setVertex ( desktop() -> relatedVertex ( p ) ) ;
  // use the functor 
  return m_fun ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeFitChi2DV::fillStream 
( std::ostream& s ) const { return s << "BPVLTFITCHI2('" << m_fit << "')" ; }
// ============================================================================


// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV::VertexZDistanceWithTheBestPV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV*
LoKi::Particles::VertexZDistanceWithTheBestPV::clone() const 
{ return new LoKi::Particles::VertexZDistanceWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV::result_type
LoKi::Particles::VertexZDistanceWithTheBestPV::operator() 
  ( LoKi::Particles::VertexZDistanceWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  const LHCb::VertexBase* ev = p->endVertex() ;
  if ( 0 == ev ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* bpv = desktop() -> relatedVertex ( p ) ; 
  if ( 0 == bpv ) 
  {
    Error ( "Related points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // 
  return ev->position().Z() - bpv->position().Z() ;    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexZDistanceWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVVDZ" ; }


// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV::VertexRhoDistanceWithTheBestPV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV*
LoKi::Particles::VertexRhoDistanceWithTheBestPV::clone() const 
{ return new LoKi::Particles::VertexRhoDistanceWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV::result_type
LoKi::Particles::VertexRhoDistanceWithTheBestPV::operator() 
  ( LoKi::Particles::VertexRhoDistanceWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  const LHCb::VertexBase* ev = p->endVertex() ;
  if ( 0 == ev ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* bpv = desktop() -> relatedVertex ( p ) ; 
  if ( 0 == bpv ) 
  {
    Error ( "Related points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // 
  return ( ev->position() - bpv->position() ) . Rho() ;    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexRhoDistanceWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVVDRHO" ; }


// ============================================================================
// constructor from the source
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource::MinVertexDistanceWithSource
( const LoKi::Particles::MinVertexDistanceWithSource::_Source& s ) 
  :  LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( s ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource*
LoKi::Particles::MinVertexDistanceWithSource::clone() const 
{ return new LoKi::Particles::MinVertexDistanceWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource::result_type
LoKi::Particles::MinVertexDistanceWithSource::operator() 
  ( LoKi::Particles::MinVertexDistanceWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDSOURCE(" << m_fun.source() << ")" ; }
// ============================================================================


// ============================================================================
// constructor from the source
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource::MinVertexChi2DistanceWithSource
( const LoKi::Particles::MinVertexChi2DistanceWithSource::_Source& s ) 
  :  LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( s ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource*
LoKi::Particles::MinVertexChi2DistanceWithSource::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource::result_type
LoKi::Particles::MinVertexChi2DistanceWithSource::operator() 
  ( LoKi::Particles::MinVertexChi2DistanceWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDCHI2SOURCE(" << m_fun.source() << ")" ; }
// ============================================================================

 

// ============================================================================
// the default constructor
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::MinVertexDistanceDV ()
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_PRIMARY ) 
{}
// ============================================================================
// the constructor from the vertex filter
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::MinVertexDistanceDV 
( const LoKi::PhysTypes::VCuts& cut )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( cut ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceDV*
LoKi::Particles::MinVertexDistanceDV::clone() const 
{ return new LoKi::Particles::MinVertexDistanceDV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::result_type 
LoKi::Particles::MinVertexDistanceDV::operator()
  ( LoKi::Particles::MinVertexDistanceDV::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceDV::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDDV(" << m_fun.cut() << ")" ; }
// ============================================================================


// ============================================================================
// the default constructor
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV ()
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_PRIMARY ) 
{}
// ============================================================================
// the constructor from the vertex filter
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV 
( const LoKi::PhysTypes::VCuts& cut )
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( cut ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV*
LoKi::Particles::MinVertexChi2DistanceDV::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceDV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::result_type 
LoKi::Particles::MinVertexChi2DistanceDV::operator()
  ( LoKi::Particles::MinVertexChi2DistanceDV::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceDV::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDCHI2DV(" << m_fun.cut() << ")" ; }
// ============================================================================


// ============================================================================
// the constructor from the TES location
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES locations
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cut  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts&   cut  ,
  const std::vector<std::string>& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceTES*
LoKi::Particles::MinVertexDistanceTES::clone() const 
{ return new LoKi::Particles::MinVertexDistanceTES(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::result_type 
LoKi::Particles::MinVertexDistanceTES::operator() 
  ( LoKi::Particles::MinVertexDistanceTES::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceTES::fillStream 
( std::ostream& s ) const 
{ 
  s << "MINVDTES("  ;
  //
  if ( 1 == m_fun.path().size() ) 
  { s << Gaudi::Utils::toString ( m_fun.path().front() ) ; }
  else 
  { s << Gaudi::Utils::toString ( m_fun.path()         ) ; }
  //
  return s << "," << m_fun.cut() << ")" ;
}
// ============================================================================


// ============================================================================
// the constructor from the TES location
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES locations
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cut  ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts&   cut  ,
  const std::vector<std::string>& path ) 
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES*
LoKi::Particles::MinVertexChi2DistanceTES::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceTES(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::result_type 
LoKi::Particles::MinVertexChi2DistanceTES::operator() 
  ( LoKi::Particles::MinVertexDistanceTES::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceTES::fillStream 
( std::ostream& s ) const 
{ 
  s << "MINVDCHI2TES("  ;
  //
  if ( 1 == m_fun.path().size() ) 
  { s << Gaudi::Utils::toString ( m_fun.path().front() ) ; }
  else 
  { s << Gaudi::Utils::toString ( m_fun.path()         ) ; }
  //
  return s << "," << m_fun.cut() << ")" ;
}
// ============================================================================



 




// ============================================================================
// The END 
// ============================================================================
