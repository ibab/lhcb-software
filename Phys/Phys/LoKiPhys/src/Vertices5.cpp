// $Id: Vertices5.cpp,v 1.1 2008-03-30 13:43:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/PhysSources.h"
#include "LoKi/Vertices0.h"
#include "LoKi/Vertices5.h"
// ============================================================================
/** @file
 *  Collection of "context-dependent" functors, needed for the 
 *  new framework "CombineParticles", developed by Juan PALACIOS, 
 *   Patrick KOPPENBURG and Gerhard RAVEN.
 * 
 *  Essentially all these functord depends on "event-data" and 
 *  get the nesessary "context-dependent" data from Algorithm Context Service
 *
 *  The basic ingredients here:
 *   - LoKi Service 
 *   - Algorithm Context Service
 *   - PhysDesktop
 *   - LoKi::getPhysDesktop
 *   - Gaudi::Utils::getDValgorithm
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-28
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  // the local empty vector of vertices 
  static const LHCb::VertexBase::ConstVector s_EMPTY ;
  // ==========================================================================
  /// the finder for the primary vertices:
  const LoKi::Vertices::IsPrimary s_PRIMARY = LoKi::Vertices::IsPrimary () ; 
  // ==========================================================================
}
// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Vertices::MinVertexDistanceWithSource::MinVertexDistanceWithSource 
( const LoKi::Vertices::MinVertexDistanceWithSource::_Source& source )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function() 
  , m_mvd    ( s_EMPTY ) 
  , m_source ( source  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexDistanceWithSource*
LoKi::Vertices::MinVertexDistanceWithSource::clone() const 
{ return new LoKi::Vertices::MinVertexDistanceWithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Vertices::MinVertexDistanceWithSource::result_type 
LoKi::Vertices::MinVertexDistanceWithSource::minvd 
( LoKi::Vertices::MinVertexDistanceWithSource::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'Huge Distance'" ) ;
    return LoKi::Constants::HugeDistance ;                      // RETURN 
  }
  // check the event 
  if ( !sameEvent() ) 
  {
    // clear the list of vertices 
    m_mvd.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = m_source() ;
    // fill the functor with primary vertices:
    m_mvd.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( m_mvd.empty() ) { Error ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return m_mvd ( v  ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Vertices::MinVertexDistanceWithSource::fillStream 
( std::ostream& s ) const { return s << "VMINVDSOURCE(" << m_source << ")" ; }
// ============================================================================


// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Vertices::MinVertexDistanceDV::MinVertexDistanceDV()
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) ) 
  , m_cut ( s_PRIMARY )
{}
// ============================================================================
// the constructor form the vertex filter
// ============================================================================
LoKi::Vertices::MinVertexDistanceDV::MinVertexDistanceDV 
( const LoKi::PhysTypes::VCuts& cut ) 
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceDesktop ( cut ) ) 
  , m_cut ( cut  )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexDistanceDV*
LoKi::Vertices::MinVertexDistanceDV::clone() const 
{ return new LoKi::Vertices::MinVertexDistanceDV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Vertices::MinVertexDistanceDV::fillStream ( std::ostream& s ) const 
{ return s << "VMINVDDV(" << m_cut << ")" ; }
// ============================================================================


// ============================================================================
// the constructor from the TES location  
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string& path )
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY )  ) 
  , m_cut  ( s_PRIMARY ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path )
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY )  ) 
  , m_cut  ( s_PRIMARY ) 
  , m_path (     path  ) 
{}
// ============================================================================
// the constructor from the TES location  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  )
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut       ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path ,
  const LoKi::PhysTypes::VCuts&   cut  )
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut   ) 
  , m_path ( path  ) 
{}
// ============================================================================
// the constructor from the TES location  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut       ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts&   cut  , 
  const std::vector<std::string>& path )
  : LoKi::Vertices::MinVertexDistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut   ) 
  , m_path ( path  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexDistanceTES*
LoKi::Vertices::MinVertexDistanceTES::clone() const 
{ return new LoKi::Vertices::MinVertexDistanceTES(*this) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& 
LoKi::Vertices::MinVertexDistanceTES::fillStream 
( std::ostream& s ) const 
{
  s << "VMINVDTES(" ;
  //
  if ( 1 == m_path.size() ) { s << Gaudi::Utils::toString ( m_path.front() ) ; }
  else                      { s << Gaudi::Utils::toString ( m_path         ) ; }
  //
  return s << "," << m_cut << ")" ;
}
// ============================================================================

                                                  
// ============================================================================
// constructor from the source 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceWithSource::MinVertexChi2DistanceWithSource 
( const LoKi::Vertices::MinVertexChi2DistanceWithSource::_Source& source )
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function() 
  , m_mvd    ( s_EMPTY ) 
  , m_source ( source  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceWithSource*
LoKi::Vertices::MinVertexChi2DistanceWithSource::clone() const 
{ return new LoKi::Vertices::MinVertexChi2DistanceWithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceWithSource::result_type 
LoKi::Vertices::MinVertexChi2DistanceWithSource::minvdchi2  
( LoKi::Vertices::MinVertexChi2DistanceWithSource::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'Huge Chi2'" ) ;
    return LoKi::Constants::HugeChi2 ;                       // RETURN 
  }
  // check the event 
  if ( !sameEvent() ) 
  {
    // clear the list of vertices 
    m_mvd.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = m_source() ;
    // fill the functor with primary vertices:
    m_mvd.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( m_mvd.empty() ) { Error ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return m_mvd ( v  ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Vertices::MinVertexChi2DistanceWithSource::fillStream 
( std::ostream& s ) const { return s << "VMINVDCHI2SOURCE(" << m_source << ")" ; }
// ============================================================================


// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV()
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) ) 
  , m_cut ( s_PRIMARY )
{}
// ============================================================================
// the constructor form the vertex filter
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV 
( const LoKi::PhysTypes::VCuts& cut ) 
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceDesktop ( cut ) ) 
  , m_cut ( cut  )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceDV*
LoKi::Vertices::MinVertexChi2DistanceDV::clone() const 
{ return new LoKi::Vertices::MinVertexChi2DistanceDV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream&
LoKi::Vertices::MinVertexChi2DistanceDV::fillStream ( std::ostream& s ) const 
{ return s << "VMINVDCHI2DV(" << m_cut << ")" ; }
// ============================================================================




// ============================================================================
// the constructor from the TES location  
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string& path )
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY )  ) 
  , m_cut  ( s_PRIMARY ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path )
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY )  ) 
  , m_cut  ( s_PRIMARY ) 
  , m_path (     path  ) 
{}
// ============================================================================
// the constructor from the TES location  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  )
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut       ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path ,
  const LoKi::PhysTypes::VCuts&   cut  )
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut   ) 
  , m_path ( path  ) 
{}
// ============================================================================
// the constructor from the TES location  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut       ) 
  , m_path ( 1 , path  ) 
{}
// ============================================================================
// the constructor from the TES locations  and vertex selector 
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts&   cut  , 
  const std::vector<std::string>& path )
  : LoKi::Vertices::MinVertexChi2DistanceWithSource ( LoKi::Vertices::SourceTES ( path , cut )  ) 
  , m_cut  ( cut   ) 
  , m_path ( path  ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::MinVertexChi2DistanceTES*
LoKi::Vertices::MinVertexChi2DistanceTES::clone() const 
{ return new LoKi::Vertices::MinVertexChi2DistanceTES(*this) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& 
LoKi::Vertices::MinVertexChi2DistanceTES::fillStream 
( std::ostream& s ) const 
{
  s << "VMINVDCHI2TES(" ;
  //
  if ( 1 == m_path.size() ) { s << Gaudi::Utils::toString ( m_path.front() ) ; }
  else                      { s << Gaudi::Utils::toString ( m_path         ) ; }
  //
  return s << "," << m_cut << ")" ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
