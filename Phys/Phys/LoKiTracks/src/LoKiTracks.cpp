// $Id$ 
// ============================================================================
// Incldiue files 
// ============================================================================
// Local 
// ============================================================================
#include "LoKi/LoKiTracks.h"
// =======================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// the specific printout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Empty<const LHCb::Track*>::fillStream
( std::ostream& s ) const { return s << "TrEMPTY" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& LoKi::Functors::Size<const LHCb::Track*>::fillStream
( std::ostream& s ) const { return s << "TrSIZE" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& 
LoKi::Constant<const LHCb::Track*,bool>::fillStream( std::ostream& s ) const 
{ return s << ( this->m_value ? "TrALL" : "TrNONE" ) ; }
// ============================================================================
// The END 
// ============================================================================


