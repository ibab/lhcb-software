// $Id: ICaloTrackIdEval.h,v 1.10 2009-10-25 14:40:11 ibelyaev Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKIDEVAL_H 
#define CALOINTERFACES_ICALOTRACKIDEVAL_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declaration
// ============================================================================
namespace LHCb
{
  class Track      ;
}
// ============================================================================
/** @class ICaloTrackIdEval ICaloTrackIdEval.h CaloInterfaces/ICaloTrackIdEval.h
 *  
 *  The abstract interface for generic track ID estimator 
 *  with calorimeters  
 *
 *  The obvious candidates for implementation
 *   - e+/e-   identification with Prs/Spd 
 *   - e+/e-   identification with Ecal  
 *   - e+/e-   identification with Hcal  
 *   - mu+/mu- identification with Hcal  
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
class ICaloTrackIdEval : public virtual IAlgTool 
{
public:
  // ==========================================================================
  /** The main processing method 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process   
  ( const LHCb::Track* track , 
    double&              value ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   */  
  virtual double     operator() 
    ( const LHCb::Track* track ) const = 0 ;
  // ==========================================================================  
public:
  // ==========================================================================
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~ICaloTrackIdEval() ;               // virtual & protected destructor 
  // ==========================================================================
};
// ============================================================================
#endif // CALOINTERFACES_ICALOTRACKIDEVAL_H
// ============================================================================
