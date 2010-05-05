// $Id: GetTools.cpp,v 1.1 2010-05-05 15:45:02 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GetAlgs.h"
// ============================================================================
// DaVinciKernel 
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/GetDVAlgorithm.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/AuxFunBase.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/GetTools.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace  LoKi::GetTools
 *  @date 2010-05-05 
 *  @author Vanya Belyaev Ivan.Belyev@itep.ru
 */
// ============================================================================
namespace 
{
  const std::string s_DistanceCalculator = "LoKi::DistanceCalculator" ;
  const std::string s_LifetimeFitter     = "LoKi::LifetimeFitter"     ;
}
// ============================================================================
/*  get the distance calculator 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDistanceCalculator* 
LoKi::GetTools::distanceCalculator
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IDistanceCalculator* dc = distanceCalculator ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IDistanceCalculator '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the distance calculator 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDistanceCalculator* 
LoKi::GetTools::distanceCalculator
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IDistanceCalculator* dc = distanceCalculator ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() )
  { return distanceCalculator ( base , s_DistanceCalculator ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IDistanceCalculator* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool 
    ( nick.empty() ? s_DistanceCalculator  : nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the distance calculator 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDistanceCalculator* 
LoKi::GetTools::distanceCalculator
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  DVAlgorithm* dv = Gaudi::Utils::getDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IDistanceCalculator* geo = dv -> distanceCalculator ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IDistanceCalculator* geo = alg -> tool<IDistanceCalculator>
      ( nick.empty() ? s_DistanceCalculator : nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ========================================================================
/*  get the lifetime fitter 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ========================================================================
const ILifetimeFitter* 
LoKi::GetTools::lifetimeFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick )
{
  const ILifetimeFitter* fitter = lifetimeFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == fitter ) 
  { base.Error ( "Unable to locate ILifetimeFitter '" + nick + "'" ) ; }
  //
  return fitter ;
}
// ========================================================================
/*  get the lifetime fitter 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ========================================================================
const ILifetimeFitter* 
LoKi::GetTools::lifetimeFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick )
{
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const ILifetimeFitter* dc = lifetimeFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                           // RETURN 
  //
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if  ( !tsvc  ) { return 0 ; }                            // RETURN
  //
  ILifetimeFitter* fitter  = 0 ;
  StatusCode sc = tsvc->retrieveTool 
    ( nick.empty() ? s_LifetimeFitter  : nick  , fitter  ) ;
  if ( sc.isSuccess() && 0 != fitter ) { return fitter ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the lifetime fitter 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter 
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const ILifetimeFitter* 
LoKi::GetTools::lifetimeFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  // ========================================================================
  // 1. get DVAlgorithm from the context 
  DVAlgorithm* dv = Gaudi::Utils::getDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const ILifetimeFitter* geo = dv -> lifetimeFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  // ========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool form the algorithm
    const ILifetimeFitter* geo = alg -> tool<ILifetimeFitter>
      ( nick.empty() ? s_LifetimeFitter : nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                  // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
// The END 
// ============================================================================
