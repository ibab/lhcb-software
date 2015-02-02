// $Id$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Particles38.h"
// ============================================================================
/** @file
 *
 * Implementation file for functions from "M_corr" family by Mike Williams 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *   with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  Last modification $Date$
 *                 by $Author$
 *  Version           $Revision$ 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-10-23
 */
// ============================================================================
/// anonymos namespace to hide local varibales 
namespace 
{
  // ==========================================================================
  /// the invalid 3-momentum
  const LoKi::ThreeVector s_VECTOR =  LoKi::ThreeVector ( 0 , 0 , -1 * Gaudi::Units::TeV   ) ;
  /// the invalid 3Dpoint 
  const LoKi::Point3D     s_POINT  =  LoKi::Point3D     ( 0 , 0 , -1 * Gaudi::Units::km    ) ;
  // ==========================================================================
} //                                                  end of anonymos namespace 
// ============================================================================
/*  constructor from the primary vertex
 *  @param x the x-position of primary vertex 
 *  @param y the x-position of primary vertex 
 *  @param z the x-position of primary vertex 
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight 
( const double x , 
  const double y , 
  const double z )
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR ) 
  , LoKi::Vertices::VertexHolder   ( LoKi::Point3D ( x , y , z )  ) 
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param pv  the primary vertex 
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight 
( const LHCb::VertexBase* pv    ) 
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR ) 
  , LoKi::Vertices::VertexHolder   ( pv ) 
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param point  the position of primary vertex 
 */
// ============================================================================
LoKi::Particles::PtFlight::PtFlight 
( const LoKi::Point3D& point ) 
  : LoKi::Particles::TransverseMomentumRel ( s_VECTOR ) 
  , LoKi::Vertices::VertexHolder ( point ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Particles::PtFlight::~PtFlight(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PtFlight*
LoKi::Particles::PtFlight::clone() const 
{ return new LoKi::Particles::PtFlight ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PtFlight::result_type 
LoKi::Particles::PtFlight::operator() 
  ( LoKi::Particles::PtFlight::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx ) 
  {
    Error("EndVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() , 
           "Vertex-Information is not valid"     ) ;
  //
  return ptFlight ( p  -> momentum() , 
                    vx -> position() , position () ) ;
  
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PtFlight::fillStream ( std::ostream& s ) const 
{
  return s << "PTFLIGHT(" 
           << position().X() << ","  
           << position().Y() << "," 
           << position().Z() << ")" ; 
}
// ============================================================================


// ============================================================================
/*  constructor from the primary vertex
 *  @param x the x-position of primary vertex 
 *  @param y the x-position of primary vertex 
 *  @param z the x-position of primary vertex 
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const double x , 
  const double y , 
  const double z )
  : LoKi::Particles::PtFlight ( x , y , z ) 
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param pv  the primary vertex 
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const LHCb::VertexBase* pv    ) 
  : LoKi::Particles::PtFlight ( pv ) 
{}
// ============================================================================
/*  constructor from the primary vertex
 *  @param point  the position of primary vertex 
 */
// ============================================================================
LoKi::Particles::MCorrected::MCorrected
( const LoKi::Point3D& point ) 
  : LoKi::Particles::PtFlight ( point ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Particles::MCorrected::~MCorrected(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MCorrected*
LoKi::Particles::MCorrected::clone() const 
{ return new LoKi::Particles::MCorrected ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MCorrected::result_type 
LoKi::Particles::MCorrected::operator() 
  ( LoKi::Particles::MCorrected::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx ) 
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() , 
           "Vertex-Information is not valid"     ) ;
  //
  return mCorrFlight ( p  -> momentum() , 
                       vx -> position() , position () ) ;
  
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MCorrected::fillStream ( std::ostream& s ) const 
{
  return s << "CORRM(" 
           << position().X() << ","  
           << position().Y() << "," 
           << position().Z() << ")" ; 
}
// ============================================================================



// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::PtFlightWithBestVertex() 
  : LoKi::AuxDesktopBase () 
  , LoKi::Particles::PtFlight ( s_POINT ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::~PtFlightWithBestVertex(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex*
LoKi::Particles::PtFlightWithBestVertex::clone() const 
{ return new LoKi::Particles::PtFlightWithBestVertex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::PtFlightWithBestVertex::result_type 
LoKi::Particles::PtFlightWithBestVertex::operator() 
  ( LoKi::Particles::PtFlightWithBestVertex::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx ) 
  {
    Error("EndVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Error("BestVertex is invalid, return 'Invalid Momentum'") ;
    return LoKi::Constants::InvalidMomentum ;
  }
  //
  setVertex ( pv ) ;
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() , 
           "Vertex-Information is not valid"     ) ;
  //
  return ptFlight ( p  -> momentum() , 
                    vx -> position() , position () ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::PtFlightWithBestVertex::fillStream ( std::ostream& s ) const 
{ return s << "BPVPTFLIGHT" ; }
// ============================================================================





// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::MCorrectedWithBestVertex() 
  : LoKi::Particles::PtFlightWithBestVertex() 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::~MCorrectedWithBestVertex(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex*
LoKi::Particles::MCorrectedWithBestVertex::clone() const 
{ return new LoKi::Particles::MCorrectedWithBestVertex ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MCorrectedWithBestVertex::result_type 
LoKi::Particles::MCorrectedWithBestVertex::operator() 
  ( LoKi::Particles::MCorrectedWithBestVertex::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error("Invalid argument, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  // get the decay vertex:
  const LHCb::VertexBase* vx = p->endVertex() ;
  if ( 0 == vx ) 
  {
    Error("EndVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  const LHCb::VertexBase* pv = bestVertex ( p ) ;
  if ( 0 == pv ) 
  {
    Error("BestVertex is invalid, return 'Invalid Mass'") ;
    return LoKi::Constants::InvalidMass ;
  }
  //
  setVertex ( pv ) ;
  //
  Assert ( LoKi::Vertices::VertexHolder::valid() , 
           "Vertex-Information is not valid"     ) ;
  //
  return mCorrFlight ( p  -> momentum() , 
                       vx -> position() , position () ) ;
}
// ============================================================================
//  OPTIONAL: the specific printout 
// ============================================================================
std::ostream& 
LoKi::Particles::MCorrectedWithBestVertex::fillStream ( std::ostream& s ) const 
{ return s << "BPVCORRM" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
