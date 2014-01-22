// $Id$
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file
 *  Implementation of geenric Hlt-functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 */
// ============================================================================
// the specific printout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Empty<const Hlt::Candidate*>::fillStream
( std::ostream& s ) const { return s << "TC_EMPTY" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Size<const Hlt::Candidate*>::fillStream
( std::ostream& s ) const { return s << "TC_SIZE" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& 
LoKi::Constant<const Hlt::Candidate*,bool>::fillStream( std::ostream& s ) const 
{ return s << ( this->m_value ? "TC_ALL" : "TC_NONE" ) ; }
// ============================================================================
// the specific printout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Empty<const Hlt::Stage*>::fillStream
( std::ostream& s ) const { return s << "TS_EMPTY" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Size<const Hlt::Stage*>::fillStream
( std::ostream& s ) const { return s << "TS_SIZE" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& 
LoKi::Constant<const Hlt::Stage*,bool>::fillStream( std::ostream& s ) const 
{ return s << ( this->m_value ? "TS_ALL" : "TS_NONE" ) ; }
// ============================================================================
// The END 
// ============================================================================
