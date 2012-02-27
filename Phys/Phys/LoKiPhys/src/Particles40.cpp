// $Id$
// ============================================================================
// Include files 
// ============================================================================
// LoKiCode
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Particles40.h"
// ============================================================================
/** @file
 *  Implementation file for functions form file LoKi/Particles40.h
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::SmartInfo::SmartInfo
( const int                                                   index  , 
  const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun    , 
  const bool                                                  update ) 
  : LoKi::ExtraInfo::GetSmartInfo<const LHCb::Particle*> ( index , fun , update )
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::SmartInfo::SmartInfo
( const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun    , 
  const int                                                   index  , 
  const bool                                                  update ) 
  : LoKi::ExtraInfo::GetSmartInfo<const LHCb::Particle*> ( index , fun , update )
{}
// ============================================================================
// destructor
// ============================================================================
LoKi::Particles::SmartInfo::~SmartInfo(){}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::SmartInfo*
LoKi::Particles::SmartInfo::clone() const 
{ return new LoKi::Particles::SmartInfo(*this); }
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::SmartInfo::fillStream( std::ostream& s ) const 
{ 
  s << "SINFO(" << func () << "," << index() << "," ;
  if ( update() ) { s << "True"  ; }
  else            { s << "False" ; }
  return s << ")" ;  
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Vertices::SmartInfo::SmartInfo
( const int                                                     index  , 
  const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun    , 
  const bool                                                    update ) 
  : LoKi::ExtraInfo::GetSmartInfo<const LHCb::VertexBase*> ( index , fun , update )
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Vertices::SmartInfo::SmartInfo
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Function& fun    , 
  const int                                                   index  , 
  const bool                                                  update ) 
  : LoKi::ExtraInfo::GetSmartInfo<const LHCb::VertexBase*> ( index , fun , update )
{}
// ============================================================================
// destructor
// ============================================================================
LoKi::Vertices::SmartInfo::~SmartInfo(){}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Vertices::SmartInfo*
LoKi::Vertices::SmartInfo::clone() const 
{ return new LoKi::Vertices::SmartInfo(*this); }
// ============================================================================
// the specific printout 
// ============================================================================
std::ostream& 
LoKi::Vertices::SmartInfo::fillStream( std::ostream& s ) const 
{ 
  s << "VSINFO(" << func () << "," << index() << "," ;
  if ( update() ) { s << "True"  ; }
  else            { s << "False" ; }
  return s << ")" ;  
}
// ============================================================================
// The END 
// ============================================================================
