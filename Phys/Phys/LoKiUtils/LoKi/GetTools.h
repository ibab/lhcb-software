// $Id: GetTools.h,v 1.2 2010-05-29 15:15:09 ibelyaev Exp $
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
class IDecayTreeFit        ;
class IVertexFit           ;
class IParticleFilter      ;
class IProtoParticleFilter ;
class ITrackSelector       ;
class ITriggerTisTos       ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  class GAUDI_API GetTools 
  {
    // ========================================================================
  public: // distance calculator 
    // ========================================================================
    /** get the distance calculator 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDistanceCalculator* 
    distanceCalculator
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the distance calculator 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDistanceCalculator* 
    distanceCalculator
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the distance calculator 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::distanceCalculator
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDistanceCalculator* 
    distanceCalculator
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // lifetime fitter 
    // ========================================================================
    /** get the lifetime fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ILifetimeFitter* 
    lifetimeFitter
    ( const LoKi::AuxFunBase& base , 
      const std::string&      nick = "" );
    // ========================================================================
    /** get the lifetime fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ILifetimeFitter* 
    lifetimeFitter
    ( const LoKi::ILoKiSvc*   base , 
      const std::string&      nick = "" );
    // ========================================================================
    /** get the lifetime fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::lifetimeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ILifetimeFitter* 
    lifetimeFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // particle transporter 
    // ========================================================================
    /** get the particle transporter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IParticleTransporter* 
    particleTransporter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle transporter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToollSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IParticleTransporter* 
    particleTransporter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle transporter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IParticleTransporter* 
    particleTransporter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // particle refitter  
    // ========================================================================
    /** get the particle refitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleReFitter* 
    particleReFitter
    ( const LoKi::AuxFunBase& base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the particle refitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleReFitter* 
    particleReFitter
    ( const LoKi::ILoKiSvc*   base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the particle refitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleReFitter* 
    particleReFitter
    ( const IAlgContextSvc*   cntx ,
      const std::string&      nick ) ;
    // ========================================================================
  public: // mass fitter   
    // ========================================================================
    /** get the mass fitter  
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::massFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IMassFit* 
    massFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle refitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IMassFit* 
    massFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle refitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleReFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IMassFit* 
    massFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // direction fitter 
    // ========================================================================
    /** get the direction fitter  
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDirectionFit* 
    directionFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDirectionFit* 
    directionFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::directionFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IDirectionFit* 
    directionFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // vertex fitter 
    // ========================================================================
    /** get the vertex fitter  
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IVertexFit* 
    vertexFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the vertex fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IVertexFit* 
    vertexFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the direction fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::vertexFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IVertexFit* 
    vertexFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // protoparticle filter 
    // ========================================================================
    /** get the protoparticle filter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IProtoParticleFilter* 
    protoParticleFilter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the protoparticle filter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IProtoParticleFilter* 
    protoParticleFilter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the protoparticle filter 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IProtoParticleFilter* 
    protoParticleFilter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // particle filter 
    // ========================================================================
    /** get the  particle filter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleFilter* 
    particleFilter
    ( const LoKi::AuxFunBase& base       , 
      const std::string&      nick  = "" ) ;
    // ========================================================================
    /** get the particle filter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleFilter* 
    particleFilter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the particle filter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::particleFilter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const IParticleFilter* 
    particleFilter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public: // decay tree fitter 
    // ========================================================================
    /** get the decay tree fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IDecayTreeFit* 
    decayTreeFitter
    ( const LoKi::AuxFunBase& base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the decay tree fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IDecayTreeFit* 
    decayTreeFitter
    ( const LoKi::ILoKiSvc*   base      , 
      const std::string&      nick = "" ) ;
    // ========================================================================
    /** get the decay tree fitter 
     *  -  try to locate IDVAlgorithm and rely on IDVAlgorithm::decayTreeFitter
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    IDecayTreeFit* 
    decayTreeFitter
    ( const IAlgContextSvc*   cntx      ,
      const std::string&      nick = "" ) ;
    // ========================================================================
  public:
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITrackSelector* 
    trackSelector
    ( const LoKi::AuxFunBase& base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) base    the base to be used 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITrackSelector* 
    trackSelector
    ( const LoKi::ILoKiSvc*   base , 
      const std::string&      nick ) ;
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  -  use IToolSvc::retrieveTool 
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITrackSelector* 
    trackSelector
    ( const IAlgContextSvc*   cntx ,
      const std::string&      nick ) ;
    // ========================================================================
  public:
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITriggerTisTos*
    triggerTisTos 
    ( const LoKi::AuxFunBase& cntx ,
      const std::string&      nick ) ;
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITriggerTisTos*
    triggerTisTos 
    ( const LoKi::ILoKiSvc*   cntx ,
      const std::string&      nick ) ;
    // ========================================================================
    /** get the track selector tool 
     *  -  try to locate GaudiAlgorithm and rely on GaudiAlgorithm::tool
     *  @param (INPUT) cntx context service 
     *  @param (INPUT) nick tool typename/nick 
     *  @return the tool 
     */
    static 
    const ITriggerTisTos*
    triggerTisTos 
    ( const IAlgContextSvc*   cntx ,
      const std::string&      nick ) ;
    // ========================================================================
  } ; //
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETTOOLS_H
// ============================================================================
