// $Id: ICaloHypoTool.h,v 1.1.1.1 2001-11-01 13:17:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOHYPOTOOL_H 
#define CALOINTERFACES_ICALOHYPOTOOL_H 1
// Include files
#include "GaudiKernel/IAlgtool.h"
// local
#include "CaloInterfaces/IIDICaloHypoTool.h"

class    CaloHypo                ;  ///< from CaloEvent 

/** @class ICaloHypoTool ICaloHypoTool.h CaloInterfaces/ICaloHypoTool.h
 *  
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloHypo objects, the potential candidates are: 
 *
 *    - hypothesis processing 
 *    - dispatching 
 *    - subcomponent of CaloParticle processing  
 *   
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/10/2001
 */

class ICaloHypoTool: public IAlgTool
{
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloHypoTool ; }
  
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
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  StatusCode process    ( CaloHypo* hypo  ) = 0 ;
  
protected:
  
  /** destructor, virtual and protected  
   */
  virtual ~ICaloHypoTool(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
