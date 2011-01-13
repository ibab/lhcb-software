// $Id: ICaloDigitTool.h,v 1.9 2008-09-22 00:50:30 odescham Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALODIGITTOOL_H
#define CALOINTERFACES_ICALODIGITTOOL_H 1
// Include files
// STD & STL
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
namespace LHCb{
  class    CaloDigit                ;  
}

static const InterfaceID IID_ICaloDigitTool( "ICaloDigitTool" , 2 , 0 );

/** @class ICaloDigitTool ICaloDigitTool.h CaloInterfaces/ICaloDigitTool.h
 *
 *  The generic interface for "Calorimeter tools" , which deals with
 *  CaloDigit objects, the potential candidates are:
 *
 *    - digit calibration
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/10/2001
 */

class ICaloDigitTool:
  public virtual IAlgTool ,
  public std::unary_function<LHCb::CaloDigit*,StatusCode>
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloDigitTool; };

  /** The main processing method
   *  @param digit pointer to CaloCluster object to be processed
   *  @return status code
   */
  virtual StatusCode process    ( LHCb::CaloDigit* digit ) const = 0 ;

  /** The main processing method (functor interface)
   *  @param digit pointer to CaloCluster object to be processed
   *  @return status code
   */
  virtual StatusCode operator() ( LHCb::CaloDigit* digit ) const = 0 ;
  
 protected:
  

};

// ============================================================================
#endif // CALOINTERFACES_ICALODIGITTOOL_H
// ============================================================================
