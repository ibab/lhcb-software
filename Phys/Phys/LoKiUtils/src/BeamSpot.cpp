// $Id: BeamLineFunctions.cpp 136843 2012-03-14 16:52:03Z piedavid $
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
#include "LoKi/BeamSpot.h"
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
 *                    $Revision: 136843 $
 *  Last Modification $Date: 2012-03-14 17:52:03 +0100 (Wed, 14 Mar 2012) $ 
 *                 by $Author: piedavid $ 
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /** @var s_CONDNAME
   *  the default condition name for velo resolver 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2011-06-03
   */
  const std::string s_CONDNAME = "/dd/Conditions/Online/Velo/MotionSystem" ;
  // ==========================================================================
}
// ============================================================================
// Constructor from bound & condiname 
// ============================================================================
LoKi::BeamSpot::BeamSpot 
( const double       bound    ,
  const std::string& condname )
  : LoKi::AuxFunBase (                       )
  , m_resolverBound  ( bound                 ) 
  , m_veloClosed     ( false                 ) 
  , m_beamSpotX      ( -1 * Gaudi::Units::km ) 
  , m_beamSpotY      ( -1 * Gaudi::Units::km ) 
  , m_condName       ( condname              )
  , m_condition      (                       ) 
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
  if ( m_condName.empty () ) { m_condName = s_CONDNAME ; }
  SmartDataPtr<Condition> cond (  det , condName() ) ;
  Assert ( !(!cond) , "Unable to locate CONDITION='" + condName() + "'" ) ;
  //
  m_condition = cond ;
  //
  // update condition & register it
  //
  updateCondition   () ;
  registerCondition () ;
  //
}  
// ============================================================================
// Constructor from bound
// ============================================================================
LoKi::BeamSpot::BeamSpot 
( const double bound ) 
  : LoKi::AuxFunBase (                       )
  , m_resolverBound  ( bound                 ) 
  , m_veloClosed     ( false                 ) 
  , m_beamSpotX      ( -1 * Gaudi::Units::km ) 
  , m_beamSpotY      ( -1 * Gaudi::Units::km ) 
  , m_condName       ( s_CONDNAME            ) 
  , m_condition      (                       ) 
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
  SmartDataPtr<Condition> cond (  det , condName() ) ;
  Assert ( !(!cond) , "Unable to locate CONDITION='" + condName() + "'" ) ;
  //
  m_condition = cond ;
  //
  // update condition & register it
  //
  updateCondition   () ;
  registerCondition () ;
  //
}  
// ============================================================================
// Copy Constructor 
// ============================================================================
LoKi::BeamSpot::BeamSpot ( const LoKi::BeamSpot& right ) 
  : LoKi::AuxFunBase ( right                  ) 
  , m_resolverBound  ( right.m_resolverBound  ) 
  , m_veloClosed     ( right.m_veloClosed     ) 
  , m_beamSpotX      ( right.m_beamSpotX      ) 
  , m_beamSpotY      ( right.m_beamSpotY      ) 
  , m_condName       ( right.m_condName       )  
  , m_condition      ( right.m_condition      )  
{
  //
  // update condition & register it
  //
  updateCondition   () ;
  registerCondition () ;
  //
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::BeamSpot::~BeamSpot() { unregisterCondition () ; }
// ============================================================================
// register the condition
// ============================================================================
StatusCode LoKi::BeamSpot::registerCondition () 
{
  //
  LoKi::ILoKiSvc* svc = lokiSvc() ;  
  Assert ( 0 != svc , "Unable to reach LoKi Service!"  ) ;
  //
  SmartIF<IUpdateManagerSvc> upd  ( svc ) ;
  Assert ( upd.isValid() , "Unable to reach Update Manager Service" ) ;
  //
  LoKi::BeamSpot* this_ = this ;
  upd -> registerCondition
    ( this_                            , 
      m_condName                       ,
      &LoKi::BeamSpot::updateCondition ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// unregister the condition
// ============================================================================
StatusCode LoKi::BeamSpot::unregisterCondition () 
{
  //
  LoKi::ILoKiSvc* svc = lokiSvc() ;  
  if ( 0 == svc  ) return StatusCode::SUCCESS;
  //
  SmartIF<IUpdateManagerSvc> upd  ( svc ) ;
  Assert ( upd.isValid() , "Unable to reach Update Manager Service" ) ;
  //
  LoKi::BeamSpot* this_ = this ;
  upd -> unregister ( this_ ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// update the condition
// ============================================================================
StatusCode LoKi::BeamSpot::updateCondition () 
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
  //
  m_veloClosed =
    std::abs ( xRC - m_beamSpotX ) < m_resolverBound && 
    std::abs ( xLA - m_beamSpotX ) < m_resolverBound ;
  //
  return StatusCode::SUCCESS  ;
}
// ============================================================================
// The END 
// ============================================================================


