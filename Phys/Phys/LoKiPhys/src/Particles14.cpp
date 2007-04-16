// $Id: Particles14.cpp,v 1.4 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/10/27 13:39:32  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.2  2006/03/20 20:36:17  ibelyaev
//  LoKi::Particles::Filter (LoKi::Cuts::FILTER) is added
//
// Revision 1.1  2006/03/08 14:14:52  ibelyaev
//  add Particles14.h/.cpp
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles14.h"
#include "LoKi/Child.h"
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
 *  @date 2006-02-22
 */
// ============================================================================



// ============================================================================
/** constructor with daughter index (starts from 1).
 *  E.g. for 2-body decays it could be 1 or 2 
 *  @param index of daughter particles
 */
// ============================================================================
LoKi::Particles::DecayAngle::DecayAngle
( const size_t child ) 
  : LoKi::Function<const LHCb::Particle*>() 
  , m_child ( child ) 
{}
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::Particles::DecayAngle::DecayAngle
( const LoKi::Particles::DecayAngle& right ) 
  : LoKi::AuxFunBase                     ( right )
  , LoKi::Function<const LHCb::Particle*>( right )
  , m_child                              ( right.m_child ) 
{}
// ============================================================================
LoKi::Particles::DecayAngle::result_type 
LoKi::Particles::DecayAngle::operator() 
  ( LoKi::Particles::DecayAngle::argument p ) const
{
  if ( 0 == p ) 
  { 
    Error ( " Argumentis invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle ;                        // RETURN 
  }               
  // get the gaughter particle
  const LHCb::Particle* child = LoKi::Child::child ( p , m_child ) ;
  if ( 0 == child ) 
  { 
    Error ( " 'Child' is invalid, return 'InvalidAngle' " ) ;
    return LoKi::Constants::InvalidAngle;                         // RETURN 
  }               
  const LoKi::LorentzVector& mv = p     -> momentum() ;
  const LoKi::LorentzVector& dv = child -> momentum() ;
  // 
  return LoKi::Kinematics::decayAngle( dv , mv ) ;
} ;
// ============================================================================

// ============================================================================
/// OPTIONAL:  the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::DecayAngle::fillStream ( std::ostream& s ) const 
{ return s << "LV0[" << m_child << "]" ;} ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

