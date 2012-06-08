// $Id: GetTools.cpp,v 1.3 2010-05-29 15:24:17 ibelyaev Exp $
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
// DaVinciInterafces 
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
// ============================================================================
// DaVinciKernel 
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IMassFit.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IDirectionFit.h"
#include "Kernel/IParticleFilter.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IDecayTreeFit.h"
#include "Kernel/ITriggerTisTos.h"
// ============================================================================
// TrackInterfaces 
// ============================================================================
#include "TrackInterfaces/ITrackSelector.h"
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
  // ===========================================================================
  const std::string s_DistanceCalculator  = "LoKi::DistanceCalculator"          ;
  const std::string s_LifetimeFitter      = "LoKi::LifetimeFitter"              ;
  const std::string s_ParticleTransporter = "ParticleTransporter"               ;
  const std::string s_MassFitter          = "LoKi::MassFitter"                  ;
  const std::string s_VertexFitter        = "OfflineVertexFitter"               ;
  const std::string s_DirectionFitter     = "LoKi::DirectionFitter"             ;
  const std::string s_ProtoParticleFilter = "LoKi::Hybrid::ProtoParticleFilter" ;
  const std::string s_ParticleFilter      = "LoKi::Hybrid::FilterCriterion"     ;
  const std::string s_DecayTreeFit        = "LoKi::DecayTreeFit"                ;
  // ===========================================================================
}
// ============================================================================
/*  get the distance calculator 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
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
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
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
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the distance calculator 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
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
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IDistanceCalculator* geo = dv -> distanceCalculator ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  //
  if ( nick.empty() ) { return distanceCalculator ( cntx , s_DistanceCalculator ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IDistanceCalculator* geo = alg -> tool<IDistanceCalculator>
      ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ========================================================================
/*  get the lifetime fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
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
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
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
  if ( nick.empty() ) { return lifetimeFitter ( cntx , s_LifetimeFitter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if  ( !tsvc  ) { return 0 ; }                            // RETURN
  //
  ILifetimeFitter* fitter  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , fitter  ) ;
  if ( sc.isSuccess() && 0 != fitter ) { return fitter ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the lifetime fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter 
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
  // 1. get IDVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const ILifetimeFitter* geo = dv -> lifetimeFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  if ( nick.empty() ) { return lifetimeFitter ( cntx , s_LifetimeFitter ) ; }
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool form the algorithm
    const ILifetimeFitter* geo = alg -> tool<ILifetimeFitter> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                  // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/* get the particle transporter 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::tool 
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IParticleTransporter* 
LoKi::GetTools::particleTransporter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  if ( nick.empty() ) 
  { return particleTransporter ( base , s_ParticleTransporter ) ; }
  //
  IParticleTransporter* transporter = 
    particleTransporter ( base.lokiSvc() , nick ) ;
  //
  if ( 0 == transporter ) 
  { base.Error ( "Unable to locate IParticleTranporter'" + nick + "'" ) ; }
  //
  return transporter  ;
}
// ============================================================================
/* get the particle transporter 
 *  1. try to locate DVAlgorithm and rely on DVAlgorithm::tool
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IParticleTransporter* 
LoKi::GetTools::particleTransporter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{ 
  if ( nick.empty() ) 
  { return particleTransporter ( base , s_ParticleTransporter ) ; }
 //
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  IParticleTransporter* dc = particleTransporter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                           // RETURN 
  //
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if  ( !tsvc  ) { return 0 ; }                            // RETURN
  //
  IParticleTransporter* transporter = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , transporter ) ;
  if ( sc.isSuccess() && 0 != transporter ) { return transporter ; }
  //
  return 0 ;
}
// ============================================================================
/* get the particle transporter 
 *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IParticleTransporter* 
LoKi::GetTools::particleTransporter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick ) 
{
  if ( 0 == cntx    ) { return 0 ; }
  if ( nick.empty() ) 
  { return particleTransporter ( cntx , s_ParticleTransporter ) ; }
  // ========================================================================
  // get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    IParticleTransporter* transporter = 
      alg -> tool<IParticleTransporter>( nick  , alg ) ;
    if ( 0 != transporter ) { return transporter ; }  // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the particle refitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleReFitter* 
LoKi::GetTools::particleReFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IParticleReFitter* dc = particleReFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IParticleReFitter'" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the particle refitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleReFitter* 
LoKi::GetTools::particleReFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IParticleReFitter* dc = particleReFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IParticleReFitter* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the particle refitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleReFitter* 
LoKi::GetTools::particleReFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IParticleReFitter* geo = dv -> particleReFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IParticleReFitter* geo = alg -> tool<IParticleReFitter> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the mass fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::massFitter 
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IMassFit* 
LoKi::GetTools::massFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IMassFit* dc = massFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IMassFit '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the mass fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::massFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IMassFit* 
LoKi::GetTools::massFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IMassFit* dc = massFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) { return massFitter ( cntx , s_MassFitter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IMassFit* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the mass fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::massFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IMassFit* 
LoKi::GetTools::massFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IMassFit* geo = dv -> massFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  //
  if ( nick.empty() ) { return massFitter( cntx , s_MassFitter ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IMassFit* geo = alg -> tool<IMassFit> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the direction fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter 
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDirectionFit* 
LoKi::GetTools::directionFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IDirectionFit* dc = directionFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IDirectionFit '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the direction fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDirectionFit* 
LoKi::GetTools::directionFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IDirectionFit* dc = directionFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) { return directionFitter ( cntx , s_DirectionFitter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IDirectionFit* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the direction fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IDirectionFit* 
LoKi::GetTools::directionFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IDirectionFit* geo = dv -> directionFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  //
  if ( nick.empty() ) { return directionFitter ( cntx , s_DirectionFitter ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IDirectionFit* geo = alg -> tool<IDirectionFit> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the vertex fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter 
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IVertexFit* 
LoKi::GetTools::vertexFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IVertexFit* dc = vertexFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IVertexFit '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the direction fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IVertexFit* 
LoKi::GetTools::vertexFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IVertexFit* dc = vertexFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) { return vertexFitter ( cntx , s_VertexFitter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IVertexFit* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the vertex fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IVertexFit* 
LoKi::GetTools::vertexFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IVertexFit* geo = dv -> vertexFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  //
  if ( nick.empty() ) { return vertexFitter( cntx , s_VertexFitter ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IVertexFit* geo = alg -> tool<IVertexFit> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the protoparticle filter 
 *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  2. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IProtoParticleFilter* 
LoKi::GetTools::protoParticleFilter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IProtoParticleFilter* dc = protoParticleFilter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IProtoParticleFilter '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the protoparticle filter 
 *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  2. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IProtoParticleFilter* 
LoKi::GetTools::protoParticleFilter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IProtoParticleFilter* dc = protoParticleFilter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) 
  { return protoParticleFilter ( cntx , s_ProtoParticleFilter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IProtoParticleFilter* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/* get the protoparticle filter 
 *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  2. use IToollSvc::retrieveTool 
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IProtoParticleFilter* 
LoKi::GetTools::protoParticleFilter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick ) 
{
  if ( 0 == cntx ) { return 0 ; }
  //
  if ( nick.empty() ) 
  { return protoParticleFilter ( cntx , s_ProtoParticleFilter ) ; }
  //
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IProtoParticleFilter* geo = 
      alg -> tool<IProtoParticleFilter> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ========================================================================

// ============================================================================
/*  get the  particle filter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToolSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleFilter* 
LoKi::GetTools::particleFilter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const IParticleFilter* dc = particleFilter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error("Unable to locate IParticleFilter '" + nick + "'") ; }
  //
  return dc ;
}
// ============================================================================
/*  get the  particle filter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToolSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleFilter* 
LoKi::GetTools::particleFilter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const IParticleFilter* dc = particleFilter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) { return particleFilter ( cntx , s_ParticleFilter ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IParticleFilter* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the  particle filter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const IParticleFilter* 
LoKi::GetTools::particleFilter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    const IParticleFilter* geo = dv -> particleFilter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  // ==========================================================================
  if ( nick.empty() ) { return particleFilter ( cntx , s_ParticleFilter ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    const IParticleFilter* geo = alg -> tool<IParticleFilter> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/* get the decay tree fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToolSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IDecayTreeFit*
LoKi::GetTools::decayTreeFitter
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  IDecayTreeFit* dc = decayTreeFitter ( base.lokiSvc() , nick ) ;
  if ( 0 == dc ) 
  { base.Error ( "Unable to locate IDecayTreeFit '" + nick + "'" ) ; }
  //
  return dc ;
}
// ============================================================================
/*  get the decay tree fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  3. use IToollSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IDecayTreeFit*
LoKi::GetTools::decayTreeFitter
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{  
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  IDecayTreeFit* dc = decayTreeFitter ( cntx , nick ) ;
  if ( 0 != dc ) { return dc ; }                          // RETURN 
  //
  if ( nick.empty() ) { return decayTreeFitter ( cntx , s_DecayTreeFit ) ; }
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  IDecayTreeFit* calc  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , calc ) ;
  if ( sc.isSuccess() && 0 != calc ) { return calc ; }
  //
  return 0 ;
}
// ============================================================================
/*  get the decay tree fitter 
 *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
 *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
IDecayTreeFit*
LoKi::GetTools::decayTreeFitter
( const IAlgContextSvc*   cntx ,
  const std::string&      nick )
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // 1. get DVAlgorithm from the context 
  const IDVAlgorithm* dv = Gaudi::Utils::getIDVAlgorithm ( cntx ) ;
  if ( 0 != dv  ) 
  {
    // get the tool from the algorithm
    IDecayTreeFit* geo = dv -> decayTreeFitter ( nick ) ;
    if ( 0 != geo ) { return geo ; }                                // RETURN 
  }
  // ==========================================================================
  if ( nick.empty() ) { return decayTreeFitter ( cntx , s_DecayTreeFit ) ; }
  // ==========================================================================
  // 2. get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 != alg  ) 
  {
    // get the tool from the algorithm
    IDecayTreeFit* geo = alg -> tool<IDecayTreeFit> ( nick  , alg ) ;
    if ( 0 != geo ) { return geo ; }                                 // RETURN 
  }
  //
  return 0 ;
}
// ============================================================================
/*  get the track selector tool 
 *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  -  use IToolSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const ITrackSelector* 
LoKi::GetTools::trackSelector
( const LoKi::AuxFunBase& base , 
  const std::string&      nick ) 
{
  // 
  const ITrackSelector* ts = trackSelector ( base.lokiSvc() , nick ) ;
  if ( 0 == ts ) 
  { base.Error ( "Unable to locate ITrackSelector '" + nick + "'" ) ; }
  //
  return ts ;
}
// ============================================================================
/* get the track selector tool 
 *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  -  use IToolSvc::retrieveTool 
 *  @param (INPUT) base    the base to be used 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const ITrackSelector* 
LoKi::GetTools::trackSelector
( const LoKi::ILoKiSvc*   base , 
  const std::string&      nick ) 
{
  LoKi::ILoKiSvc* svc = const_cast<LoKi::ILoKiSvc*> ( base ) ;
  // get the context service: 
  SmartIF<IAlgContextSvc> cntx ( svc ) ;
  // use it! 
  const ITrackSelector* ts = trackSelector ( cntx , nick ) ;
  if ( 0 != ts ) { return ts ; }                          // RETURN 
  //
  // try tool -service 
  SmartIF<IToolSvc> tsvc ( svc ) ;
  if ( !tsvc ) { return 0 ; }                             // REUTRN 
  //
  ITrackSelector* selector  = 0 ;
  StatusCode sc = tsvc->retrieveTool ( nick  , selector ) ;
  if ( sc.isSuccess() && 0 != selector ) { return selector ; }
  //
  return 0 ;
}
// =========================================================================
/*  get the track selector tool 
 *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  -  use IToolSvc::retrieveTool 
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// =========================================================================
const ITrackSelector* 
LoKi::GetTools::trackSelector
( const IAlgContextSvc*   cntx ,
  const std::string&      nick ) 
{
  //
  if ( 0 == cntx ) { return 0 ; }                        // RETURN 
  //
  // get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 == alg  ) { return 0 ; }                        // RETURN
  //
  // get the tool from the algorithm :
  return alg -> tool<ITrackSelector> ( nick  , alg ) ;   // RETURN 
}
// ============================================================================
/* get the track selector tool 
 *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
 *  @param (INPUT) cntx context service 
 *  @param (INPUT) nick tool typename/nick 
 *  @return the tool 
 */
// ============================================================================
const ITriggerTisTos*
LoKi::GetTools::triggerTisTos 
( const IAlgContextSvc*   cntx ,
  const std::string&      nick ) 
{
  if ( 0 == cntx ) { return 0 ; }
  //
  // get 'simple' algorithm from the context:
  GaudiAlgorithm* alg = Gaudi::Utils::getGaudiAlg ( cntx ) ;
  if ( 0 == alg  ) { return 0 ; }                        // RETURN
  //
  // get the tool from the algorithm :
  return alg -> tool<ITriggerTisTos> ( nick  , alg ) ;   // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
