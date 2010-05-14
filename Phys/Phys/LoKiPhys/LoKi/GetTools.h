// $Id: GetTools.h,v 1.3 2010-05-14 15:28:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GETTOOLS_H 
#define LOKI_GETTOOLS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/AuxFunBase.h"
#include "LoKi/ILoKiSvc.h"
// ============================================================================
class IAlgContextSvc       ;
class IDistanceCalculator  ;
class ILifetimeFitter      ;
class IParticleTransporter ;
class IParticleReFitter    ;
class IMassFit             ;
class IDirectionFit        ;
class IVertexFit           ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace GetTools 
  {
    // ========================================================================
    /** get the distance calculator 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDistanceCalculator* 
    distanceCalculator
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the distance calculator 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDistanceCalculator* 
    distanceCalculator
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the distance calculator 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDistanceCalculator* 
    distanceCalculator
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the lifetime fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const ILifetimeFitter* 
    lifetimeFitter
    ( const LoKi::AuxFunBase& base , 
      const std::string&      nick = "" );
    // ========================================================================
    /** get the lifetime fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const ILifetimeFitter* 
    lifetimeFitter
    ( const LoKi::ILoKiSvc*   base , 
      const std::string&      nick = "" );
    // ========================================================================
    /** get the lifetime fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const ILifetimeFitter* 
    lifetimeFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle transporter 
     *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  2. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    IParticleTransporter* 
    particleTransporter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle transporter 
     *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  2. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    IParticleTransporter* 
    particleTransporter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle transporter 
     *  1. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    IParticleTransporter* 
    particleTransporter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle refitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IParticleReFitter* 
    particleReFitter
    ( const LoKi::AuxFunBase& base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the particle refitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IParticleReFitter* 
    particleReFitter
    ( const LoKi::ILoKiSvc*   base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the particle refitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IParticleReFitter* 
    particleReFitter
    ( const IAlgContextSvc*   cntx ,
      const std::string&      nick ) ;
    // ========================================================================
    /** get the mass fitter  
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::massFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IMassFit* 
    massFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle refitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IMassFit* 
    massFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle refitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IMassFit* 
    massFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter  
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDirectionFit* 
    directionFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDirectionFit* 
    directionFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IDirectionFit* 
    directionFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the vertex fitter  
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IVertexFit* 
    vertexFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the vertex fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  3. use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IVertexFit* 
    vertexFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  1. try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  2. try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    const IVertexFit* 
    vertexFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  } //                                          end of namespace LoKi::GetTools 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETTOOLS_H
// ============================================================================
