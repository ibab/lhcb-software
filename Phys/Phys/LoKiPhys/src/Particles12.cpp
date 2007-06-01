// $Id: Particles12.cpp,v 1.6 2007-06-01 12:07:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Constants.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles12.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-23
 */
// ============================================================================


// ============================================================================
LoKi::Particles::ProtoHasInfo::ProtoHasInfo( const int info ) 
  : LoKi::Predicate<const LHCb::Particle*>() 
  , m_info ( info ) 
{}
// ============================================================================
LoKi::Particles::ProtoHasInfo::ProtoHasInfo
( const LoKi::Particles::ProtoHasInfo& right ) 
  : LoKi::AuxFunBase                      ( right )
  , LoKi::Predicate<const LHCb::Particle*>( right ) 
  , m_info ( right.m_info )
{}
// ============================================================================
LoKi::Particles::ProtoHasInfo::result_type 
LoKi::Particles::ProtoHasInfo::operator() 
  ( LoKi::Particles::ProtoHasInfo::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'false'" ) ;
    return false ;                                    // RETURN 
  }
  // 
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " Invalid ProtoParticle, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  return pp->hasInfo ( m_info ) ;                    // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::ProtoHasInfo::fillStream
( std::ostream& s ) const 
{ return s << "PPHASINFO[" << m_info << "]" ; }
// ============================================================================

// ============================================================================
LoKi::Particles::ProtoInfo::ProtoInfo
( const int    key , 
  const double def , 
  const double bad )
  : LoKi::Function<const LHCb::Particle*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( bad ) 
{}
// ============================================================================
LoKi::Particles::ProtoInfo::ProtoInfo
( const int    key , 
  const double def )
  : LoKi::Function<const LHCb::Particle*>() 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( def ) 
{}
// ============================================================================
LoKi::Particles::ProtoInfo::ProtoInfo
( const LoKi::Particles::ProtoInfo& right ) 
  : LoKi::AuxFunBase                     ( right )
  , LoKi::Function<const LHCb::Particle*>( right ) 
  , m_key  ( right.m_key  )
  , m_def  ( right.m_def  )
  , m_bad  ( right.m_bad  )
{}
// ============================================================================
LoKi::Particles::ProtoInfo::result_type 
LoKi::Particles::ProtoInfo::operator() 
  ( LoKi::Particles::ProtoInfo::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Argument is invalid! return " + 
            Gaudi::Utils::toString ( m_bad ) ) ;
    return m_bad ;                                          // RETURN 
  }  
  //
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " ProtoParticle is invalid! return " + 
            Gaudi::Utils::toString ( m_bad ) ) ;
    return m_bad ;                                          // RETURN 
  }  
  //
  return pp->info( m_key , m_def ) ;                      // RETURN 
};
// ============================================================================
std::ostream& 
LoKi::Particles::ProtoInfo::fillStream
( std::ostream& s ) const 
{ 
  s << "INFO[" << m_key << "," << m_def ;
  if ( m_bad != m_def ) { s << "," << m_bad ; }
  return s << "]" ;
}
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProtoHasRichPID::result_type
LoKi::Particles::ProtoHasRichPID::operator() 
  ( LoKi::Particles::ProtoHasRichPID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'false'" ) ;
    return false ;                                    // RETURN 
  }
  // 
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " Invalid ProtoParticle, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  return 0 != pp->richPID() ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProtoHasRichPID::fillStream ( std::ostream& s ) const 
{ return s << "HASRICH" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProtoHasMuonPID::result_type
LoKi::Particles::ProtoHasMuonPID::operator() 
  ( LoKi::Particles::ProtoHasMuonPID::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'false'" ) ;
    return false ;                                    // RETURN 
  }
  // 
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " Invalid ProtoParticle, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  return 0 != pp->richPID() ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProtoHasMuonPID::fillStream ( std::ostream& s ) const 
{ return s << "HASMUON" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ProtoHasCaloHypos::result_type
LoKi::Particles::ProtoHasCaloHypos::operator() 
  ( LoKi::Particles::ProtoHasCaloHypos::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'false'" ) ;
    return false ;                                    // RETURN 
  }
  // 
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " Invalid ProtoParticle, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  return !(pp->calo().empty()) ;                     // RETURN 
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::ProtoHasCaloHypos::fillStream ( std::ostream& s ) const 
{ return s << "HASCALOS" ; }
// ============================================================================


// ============================================================================
LoKi::Particles::IsMuon::result_type 
LoKi::Particles::IsMuon::operator() 
  ( LoKi::Particles::IsMuon::argument p ) const
{
  //
  if ( 0 == p ) 
  {
    Error ( " Invalid Particle, return 'false'" ) ;
    return false ;                                    // RETURN 
  }
  // 
  const LHCb::ProtoParticle* pp = p->proto() ;
  //
  if ( 0 == pp ) 
  {
    Error ( " Invalid ProtoParticle, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  const LHCb::MuonPID* mPID = pp->muonPID() ;
  //
  if ( 0 == mPID ) 
  {
    Error ( " Invalid MuonPID, return 'false'" ) ;
    return false ;                                   // RETURN 
  }
  //
  return mPID -> IsMuon() ;                          // RETURN   
} ;
// ============================================================================
/// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::IsMuon::fillStream ( std::ostream& s ) const 
{ return s << "ISMUON" ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================

