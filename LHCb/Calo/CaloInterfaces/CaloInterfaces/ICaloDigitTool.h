// $Id: ICaloDigitTool.h,v 1.1.1.1 2001-11-01 13:17:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================ 
#ifndef CALOINTERFACES_ICALODIGITTOOL_H 
#define CALOINTERFACES_ICALODIGITTOOL_H 1
// Include files
#include "GaudiKernel/IAlgtool.h"
// local
#include "CaloInterfaces/IIDICaloDigitTool.h"

class    CaloDigit                ;  ///< from CaloEvent 

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

class ICaloDigitTool: public IAlgTool
{
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloDigitTool ; }
  
  /** Standard initialization of the tool
   *  @att It is not invoked  automatically! 
   *  @return status code 
   */ 
  StatusCode initialize ()  = 0 ;
  
  /** Standard finalization of the tool
   *  @att It is not invoked  automatically! 
   *  @return status code 
   */ 
  StatusCode finalize   ()  = 0 ;
  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  StatusCode process    ( CaloDigit* digit ) = 0 ;
  
protected:
  
  /** destructor, virtual and protected  
   */
  virtual ~ICaloDigitTool(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALODIGITTOOL_H
// ============================================================================
