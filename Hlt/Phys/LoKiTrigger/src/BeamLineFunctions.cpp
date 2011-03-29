// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// STD & STD 
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartDataPtr.h"
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
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-03-10
 *
 *                    $Revision:$
 *  Last Modification $Date:$ 
 *                 by $Author:$ 
 */

// ============================================================================
// Constructor from bound
// ============================================================================
LoKi::Vertices::BeamSpot::BeamSpot 
( const double bound ) 
   : LoKi::AuxFunBase()
   , m_resolverBound( bound ) 
   , m_condName( "/dd/Conditions/Online/Velo/MotionSystem" ) 
   , m_condition() 
{
   //
   LoKi::ILoKiSvc* svc = lokiSvc() ;  
   //
   Assert ( 0 != svc      , "Unable to reach LoKi Service!"  ) ;
   SmartIF<ISvcLocator> loc ( svc ) ;
   Assert ( loc.isValid() , "Unable to reach ServiceLocator" ) ;
   //
   SmartIF<IDataProviderSvc> det =  loc->service<IDataProviderSvc>( "DetectorDataSvc" , true ) ;
   Assert ( det.isValid() , "Unable to reach Detector Service" ) ;
   //
   SmartDataPtr<Condition> cond (  det , m_condName ) ;
   Assert ( !(!cond) , "Unable to locate CONDITION='" + m_condName + "'" ) ;
   //
   const double xRC = cond -> paramAsDouble ( "ResolPosRC" ) ;
   const double xLA = cond -> paramAsDouble ( "ResolPosLA" ) ;
   const double   Y = cond -> paramAsDouble ( "ResolPosY"  ) ;
   //
   m_beamSpotX = ( xRC + xLA ) / 2;
   m_beamSpotY = Y ;
   //
   m_veloClosed = true;
   if (    std::abs(xRC - m_beamSpotX) > m_resolverBound 
        || std::abs(xLA - m_beamSpotX) > m_resolverBound )
      m_veloClosed = false ;
   //
   m_condition = cond;
   //
   registerCondition();
   //
}  
// ============================================================================
// Copy Constructor 
// ============================================================================
LoKi::Vertices::BeamSpot::BeamSpot 
( const LoKi::Vertices::BeamSpot& right ) 
   : LoKi::AuxFunBase ( right ) 
   , m_resolverBound  ( right.m_resolverBound  ) 
   , m_condName       ( right.m_condName  ) 
   , m_condition      ( right.m_condition ) 
{
   registerCondition() ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::BeamSpot::~BeamSpot()
{
   unregisterCondition ();
}
// ============================================================================
// register the condition
// ============================================================================
StatusCode LoKi::Vertices::BeamSpot::registerCondition () 
{
   //
   LoKi::ILoKiSvc* svc = lokiSvc() ;  
   Assert ( 0 != svc , "Unable to reach LoKi Service!"  ) ;
   //
   SmartIF<IUpdateManagerSvc> upd  ( svc ) ;
   Assert ( upd.isValid() , "Unable to reach Update Manager Service" ) ;
   //
   LoKi::Vertices::BeamSpot* this_ = this ;
   upd -> registerCondition
      ( this_                                         , 
        m_condName                                    ,
        &LoKi::Vertices::BeamSpot::updateCondition ) ;
   //
   return StatusCode::SUCCESS ;
}
// ============================================================================
// unregister the condition
// ============================================================================
StatusCode LoKi::Vertices::BeamSpot::unregisterCondition () 
{
   //
   LoKi::ILoKiSvc* svc = lokiSvc() ;  
   if ( 0 == svc  ) return StatusCode::SUCCESS;
   //
   SmartIF<IUpdateManagerSvc> upd  ( svc ) ;
   Assert ( upd.isValid() , "Unable to reach Update Manager Service" ) ;
   //
   LoKi::Vertices::BeamSpot* this_ = this ;
   upd -> unregister ( this_ ) ;
   //
   return StatusCode::SUCCESS ;
}
// ============================================================================
// update the condition
// ============================================================================
StatusCode LoKi::Vertices::BeamSpot::updateCondition () 
{
   //
   Assert ( !(!m_condition) , "Condition is invalid!" ) ;
   //
   // Get the parameters
   //
   const double xRC = m_condition -> paramAsDouble ( "ResolPosRC" ) ;
   const double xLA = m_condition -> paramAsDouble ( "ResolPosLA" ) ;
   const double   Y = m_condition -> paramAsDouble ( "ResolPosY"  ) ;
   //
   m_beamSpotX = (xRC + xLA ) / 2;
   m_beamSpotY = Y ;
   //
   // If resolver x position > bound, velo is considered as open
   m_veloClosed = true;
   if (    std::abs(xRC - m_beamSpotX) > m_resolverBound 
        || std::abs(xLA - m_beamSpotX) > m_resolverBound )
      m_veloClosed = false ;
   //
   return StatusCode::SUCCESS  ;
}
// ============================================================================
// Constructor from bound
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho 
( const double bound ) 
   : LoKi::Vertices::BeamSpot( bound )
   , LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
{
}  
// ============================================================================
// Copy Constructor 
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho 
( const LoKi::Vertices::BeamSpotRho& right ) 
   : LoKi::AuxFunBase                                       ( right ) 
   , LoKi::Vertices::BeamSpot                               ( right ) 
   , LoKi::BasicFunctors<const LHCb::VertexBase*>::Function ( right ) 
{
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::BeamSpotRho::~BeamSpotRho()
{

}
// ============================================================================
// MANDATOTY: clone method ("virtual constructor")
// ============================================================================
LoKi::Vertices::BeamSpotRho*
LoKi::Vertices::BeamSpotRho::clone() const
{
   return new LoKi::Vertices::BeamSpotRho ( *this );
}
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
   // if the velo is opened ( x resolver position > m_resolverBound ) return -1.
   if ( !closed() ) return -1.;
  
   // return radial distance from vertex to beam spot
   const double x_diff = v -> position().x() - x();
   const double y_diff = v -> position().y() - y();

   return std::sqrt( x_diff * x_diff + y_diff * y_diff );
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Vertices::BeamSpotRho::fillStream( std::ostream& s ) const 
{ return s << "VX_BEAMSPOTRHO('" << resolverBound() << "')" ; }

// ============================================================================
// The END 
// ============================================================================


