// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STD 
// ============================================================================
#include <cmath>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Constants.h"
#include "LoKi/BeamLineFunctions.h"
// ============================================================================
/** @file 
 *  Collection of "beam-line"-related functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// Constructor from bound
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho ( const double bound ) 
  : LoKi::BeamSpot( bound )
  , LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
{}  
// ============================================================================
// Constructor from bound & condname 
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho 
( const double       bound    , 
  const std::string& condname ) 
  : LoKi::BeamSpot ( bound , condname )
  , LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
{}  
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::BeamSpotRho::~BeamSpotRho() {}
// ============================================================================
// MANDATOTY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::BeamSpotRho*
LoKi::Vertices::BeamSpotRho::clone() const
{ return new LoKi::Vertices::BeamSpotRho ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Vertices::BeamSpotRho::result_type 
LoKi::Vertices::BeamSpotRho::operator() 
  ( LoKi::Vertices::BeamSpotRho::argument v ) const 
{
  if ( 0 == v ) 
  {
    Error ("invalid argument, return InvalidDistance");
    return LoKi::Constants::InvalidDistance ;
  }
  //
  // if the velo is opened ( x resolver position > m_resolverBound ) return -1.
  //
  if ( !closed() ) { return -1 ; }  // RETURN
  //
  // return radial distance from vertex to beam spot
  //
  const double x_diff = v -> position().x() - x() ;
  const double y_diff = v -> position().y() - y() ;
  //
  return std::sqrt( x_diff * x_diff + y_diff * y_diff );
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Vertices::BeamSpotRho::fillStream( std::ostream& s ) const 
{
  s << "VX_BEAMSPOTRHO(" << resolverBound()   << "" ;
  s << ",'" << condName() << "'" ; 
  return s << ")" ;
}
// ============================================================================
// The END 
// ============================================================================


