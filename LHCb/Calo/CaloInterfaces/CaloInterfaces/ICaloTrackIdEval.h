// $Id: ICaloTrackIdEval.h,v 1.7 2006-03-22 18:21:51 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.6  2005/10/14 14:12:18  odescham
// adapt to new track model
//
// updates for CLHEP 1.9
// Revision 1.5  2005/01/25 14:12:18  cattanem
// updates for CLHEP 1.9
//
// Revision 1.4  2004/09/02 18:52:51  ibelyaev
//  make preparation for HLT/Trigger
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKIDEVAL_H 
#define CALOINTERFACES_ICALOTRACKIDEVAL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration
namespace LHCb{
  class Track      ;
};


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

class ICaloTrackIdEval : 
  public virtual                                 IAlgTool ,
  public std::unary_function<const LHCb::Track*,double>
{
  
public:
  
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
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
  
  
protected:
  
  /// protected destructor 
  virtual ~ICaloTrackIdEval();
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOTRACKIDEVAL_H
// ============================================================================
