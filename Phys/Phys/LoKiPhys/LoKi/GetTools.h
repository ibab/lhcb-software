// $Id: GetTools.h,v 1.1 2010-05-05 15:45:01 ibelyaev Exp $
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
class IAlgContextSvc      ;
class IDistanceCalculator ;
class ILifetimeFitter     ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace GetTools 
  {
    // ========================================================================
    /** get the distance calculator 
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
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
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
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
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::distanceCalculator
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
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter
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
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter
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
     *  1. try to locate DVAlgorithm and rely on DVAlgorithm::lifetimeFitter
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
  } //                                          end of namespace LoKi::GetTools 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETTOOLS_H
// ============================================================================
