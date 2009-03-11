// $Id: Particles25.cpp,v 1.1 2009-03-11 17:24:13 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// RecEvent
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Vertices0.h"
#include "LoKi/PhysHelpers.h"
#include "LoKi/PhysSources.h"
#include "LoKi/Particles25.h"
// ============================================================================
/** @file 
 *  Implementation file for functions from the file LoKi/Particles25.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  The file contains the functions, requested by Rob Lambert
 *  (see <a href="http://savannah.cern.ch/bugs/?47745">LoKi's Savannah portal #47745</a>)
 * 
 *  All these functors are to be used within CombineParticles framework only
 *  and surely completely useless for the regular Loki or Bender application
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-10 
 */
// ============================================================================
// anonymous namespace to hide some pure technical stuff
// ============================================================================
namespace
{
  // ==========================================================================
  /// "invalid" vertex 
  const LHCb::VertexBase* const s_VERTEX = 0 ;
  // ==========================================================================
  /// the selector for *all* vertices 
  const LoKi::Constant<const LHCb::VertexBase*,bool> s_VALL = true ;
  // ==========================================================================
}
// ============================================================================


// ============================================================================
// the constructor from the source 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleWithSource::MaxCosineDirectionAngleWithSource
( const LoKi::Particles::MaxCosineDirectionAngleWithSource::Source& source ) 
  : LoKi::Particles::CosineDirectionAngle( s_VERTEX ) 
  , m_source ( source ) 
{}
// ============================================================================
// copy constructor 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleWithSource::MaxCosineDirectionAngleWithSource
( const LoKi::Particles::MaxCosineDirectionAngleWithSource& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::Particles::CosineDirectionAngle ( right ) 
  , m_source   ( right.m_source ) 
  , m_vertices () // do not copy vertices!!!
{}
// ============================================================================
// MANDATORY: clone method ("virtual contructor")
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleWithSource*
LoKi::Particles::MaxCosineDirectionAngleWithSource::clone() const 
{ return new LoKi::Particles::MaxCosineDirectionAngleWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleWithSource::result_type 
LoKi::Particles::MaxCosineDirectionAngleWithSource::operator() 
  ( LoKi::Particles::MaxCosineDirectionAngleWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // ==========================================================================
  // check the event 
  if ( !sameEvent() || m_vertices.empty() ) 
  {
    m_vertices = m_source() ;
    setEvent() ;
  }
  // ==========================================================================
  double result = -1 * std::numeric_limits<double>::max() ;  
  if ( m_vertices.end()  == LoKi::Helpers::_Max_vertex 
       ( m_vertices . begin () , 
         m_vertices . end   () ,
         LoKi::Helpers::PMFA<LoKi::Particles::CosineDirectionAngle>
         ( this , &LoKi::Particles::CosineDirectionAngle::dira ) , p , result ) ) 
  {
    Error ( "Invalid _Max_vertex evaluation; return -5000 ") ;
    return -5000 ;    
  }
  // 
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::MaxCosineDirectionAngleWithSource::fillStream
( std::ostream& s ) const 
{ return s << "MAXDIRASOURCE(" << m_source << ")" ; }
// ============================================================================



// ============================================================================
// default constructor
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleDV::MaxCosineDirectionAngleDV () 
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::CosineDirectionAngle ( s_VERTEX ) 
  , m_vcut ( s_VALL )
{}
// ============================================================================
// constructor with the vertex selection 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleDV::MaxCosineDirectionAngleDV 
( const LoKi::Types::VCuts& vcut ) 
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::CosineDirectionAngle ( s_VERTEX ) 
  , m_vcut ( vcut  )
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleDV*
LoKi::Particles::MaxCosineDirectionAngleDV::clone() const 
{ return new LoKi::Particles::MaxCosineDirectionAngleDV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleDV::result_type 
LoKi::Particles::MaxCosineDirectionAngleDV::operator() 
  ( LoKi::Particles::MaxCosineDirectionAngleDV::argument p ) const 
{
  // 
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // load the desktop if needed 
  if ( !validDesktop() ) { loadDesktop() ; }
  // check it!
  Assert ( validDesktop () , "No valid IPhysDesktop is found" );
  // get all primary vertices from desktop
  const LHCb::RecVertex::Container* primaries = desktop()-> primaryVertices() ;
  if ( 0 == primaries ) 
  {
    Error ( "LHCb::RecVertex::Container* points to NULL, return -2000" ) ;
    return -2000 ;                                                     // RETURN 
  }
  // evaluate the maximum:
  double result = -1 * std::numeric_limits<double>::max() ;  
  if ( primaries -> end()  == LoKi::Helpers::_Max_vertex 
       ( primaries -> begin () , 
         primaries -> end   () ,
         m_vcut                ,
         LoKi::Helpers::PMFA<LoKi::Particles::CosineDirectionAngle>
         ( this , &LoKi::Particles::CosineDirectionAngle::dira ) , p , result ) ) 
  {
    Error ( "Invalid _Max_vertex evaluation; return -5000 ") ;
    return -5000 ;    
  }
  //
  return result ;
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::MaxCosineDirectionAngleDV::fillStream ( std::ostream& s ) const 
{ return s << "MAXDIRADV(" << m_vcut << ")" ; }
// ============================================================================


// ============================================================================
// constructor with the TES path 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleTES::MaxCosineDirectionAngleTES 
( const std::string& path )
  : LoKi::Particles::MaxCosineDirectionAngleWithSource ( LoKi::Vertices::SourceTES ( path , s_VALL ) ) 
  , m_path ( path   ) 
  , m_vcut ( s_VALL ) 
{}
// ============================================================================
// constructor form path and cuts 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleTES::MaxCosineDirectionAngleTES 
( const std::string&        path  ,
  const LoKi::Types::VCuts& vcut ) 
  : LoKi::Particles::MaxCosineDirectionAngleWithSource ( LoKi::Vertices::SourceTES ( path , vcut ) ) 
  , m_path ( path ) 
  , m_vcut ( vcut ) 
{}
// ============================================================================
// constructor form path and cuts 
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleTES::MaxCosineDirectionAngleTES 
( const LoKi::Types::VCuts& vcut ,
  const std::string&        path )
  : LoKi::Particles::MaxCosineDirectionAngleWithSource ( LoKi::Vertices::SourceTES ( path , vcut ) ) 
  , m_path ( path ) 
  , m_vcut ( vcut ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MaxCosineDirectionAngleTES*
LoKi::Particles::MaxCosineDirectionAngleTES::clone() const 
{ return new LoKi::Particles::MaxCosineDirectionAngleTES(*this) ; }
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& 
LoKi::Particles::MaxCosineDirectionAngleTES::fillStream
( std::ostream& s ) const 
{ return s << "MAXDIRATES('" << m_path << "'," << m_vcut << ")" ; }
// ============================================================================

 


// ============================================================================
// The END 
// ============================================================================
