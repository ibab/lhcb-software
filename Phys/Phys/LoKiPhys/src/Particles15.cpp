// $Id: Particles15.cpp,v 1.4 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/10/27 13:39:32  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.2  2006/04/09 16:40:28  ibelyaev
//  v1r0
//
// ============================================================================
// Include files 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IFilterCriterion.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles15.h"
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
 *  @date 2006-03-20
 */
// ============================================================================

// ============================================================================
/// constructor from the filter 
// ============================================================================
LoKi::Particles::Filter::Filter
( const IFilterCriterion* filter ) 
  : LoKi::Predicate<const LHCb::Particle*> () 
  , m_filter ( filter )
{} ;
// ============================================================================
/// copy constructor 
// ============================================================================
LoKi::Particles::Filter::Filter
( const LoKi::Particles::Filter& right ) 
  : LoKi::AuxFunBase                       ( right ) 
  , LoKi::Predicate<const LHCb::Particle*> ( right ) 
  , m_filter ( right.m_filter )
{} ;
// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::Particles::Filter::~Filter(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::Filter*
LoKi::Particles::Filter::clone() const { return new Filter(*this); }
// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::Filter::result_type
LoKi::Particles::Filter::operator() 
  ( LoKi::Particles::Filter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "Invalid argument, return 'false'");
    return false ;                                                 // RETURN 
  }
  if ( !m_filter.validPointer() ) 
  {
    Error ( "Invalid tool: IFilterCriterion* points to NULL, return 'false'");
    return false ;                                                 // RETURN 
  }
  // use tool
  return m_filter->isSatisfied( p ) ;
} ;
// ============================================================================
/// OPTIONAL: the specific printout
// ============================================================================
std::ostream& 
LoKi::Particles::Filter::fillStream( std::ostream& s ) const 
{
  s << "FILTER[";
  if ( m_filter.validPointer() ) 
  { s << m_filter->type() << "/" << m_filter->name() ; }
  else 
  { s << "<invalid>" ; }
  return s << "]" ;
};
// ============================================================================






// ============================================================================
// The END
// ============================================================================

