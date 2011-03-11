// $Id: $
// ============================================================================
// Include files 
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
// Constructor from velo-detector name 
// ============================================================================
LoKi::Vertices::BeamSpotRho::BeamSpotRho 
( const std::string& condition ) 
  : LoKi::BasicFunctors<const LHCb::VertexBase*>::Function () 
  , m_condName  ( condition ) 
  , m_condition (           ) 
{
  //
  LoKi::ILoKiSvc* svc = lokiSvc() ;
  // 
  // 1. Get The condition
  //
  Assert ( 0 != svc      , "Unable to reach LoKi Service!"  ) ;
  SmartIF<ISvcLocator> loc ( svc ) ;
  Assert ( loc.isValid() , "Unable to reach ServiceLocator" ) ;
  //
  SmartIF<IDataProviderSvc> det = 
    loc->service<IDataProviderSvc>( "DetectorDataSvc" , true ) ;
  Assert ( det.isValid() , "Unable to reach Detector Service" ) ;
  //
  SmartDataPtr<Condition> cond (  det , m_condName ) ;
  Assert ( !(!cond) , "Unable to locate CONDITION='" + m_condName + "'" ) ;
  //
  m_condition = cond ;
  //
  // 2. Register condition for Update Manager Service 
  //
  SmartIF<IUpdateManagerSvc> upd  ( svc ) ;
  Assert ( upd.isValid() , "Unable to reach Update Manager Service" ) ;
  //
  upd -> registerCondition
    ( this                                          , 
      m_condName                                    ,
      &LoKi::Vertices::BeamSpotRho::updateCondition ) ;
  //
}  
// ============================================================================
// update the condition
// ============================================================================
StatusCode LoKi::Vertices::BeamSpotRho::updateCondition () 
{ 
  return Warning ( "updateCondition: not yet implemented" ) ; 
}
// ======================================================================

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Vertices::BeamSpotRho::~BeamSpotRho(){}
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
  Assert ( !(!m_condition) , "Invalid CONDITION!" ) ;
  //
  Warning  ( "Not yet implemented, return +100" ) ;
  //
  //
  //
  return +100 ;
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Vertices::BeamSpotRho::fillStream( std::ostream& s ) const 
{ 
  return s << "VX_BEAMSPOTRHO(" << m_condName << ")" ;
}








// ============================================================================
// The END 
// ============================================================================


