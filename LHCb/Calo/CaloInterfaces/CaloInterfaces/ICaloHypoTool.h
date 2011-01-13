// $Id: ICaloHypoTool.h,v 1.9 2008-09-22 00:50:30 odescham Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALOHYPOTOOL_H 
#define CALOINTERFACES_ICALOHYPOTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
namespace LHCb{
  class    CaloHypo                ;  
}
static const InterfaceID IID_ICaloHypoTool( "ICaloHypoTool" , 2 , 0 );
/** @class ICaloHypoTool ICaloHypoTool.h CaloInterfaces/ICaloHypoTool.h
 *
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloHypo objects, the potential candidates are: 
 *
 *    \li hypothesis processing 
 *    \li dispatching 
 *    \li subcomponent of CaloParticle processing  
 *   
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */

class ICaloHypoTool: 
  public virtual IAlgTool ,
  public std::unary_function<LHCb::CaloHypo*,StatusCode>
{
  
 public:
  
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloHypoTool; };
  
  /** The main processing method 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const = 0 ;
  
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
