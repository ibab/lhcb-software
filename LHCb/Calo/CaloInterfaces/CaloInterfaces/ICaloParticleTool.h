// $Id: ICaloParticleTool.h,v 1.1.1.1 2001-11-01 13:17:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOPARTICLETOOL_H 
#define CALOINTERFACES_ICALOPARTICLETOOL_H 1
// Include files
#include "GaudiKernel/IAlgtool.h"
// local
#include "CaloInterfaces/IIDICaloParticleTool.h"

class    CaloParticle                ;  ///< from CaloEvent 

/** @class ICaloParticleTool ICaloParticleTool.h 
 *                                 CaloInterfaces/ICaloParticleTool.h
 *  
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloParticle objects, the potential candidates are: 
 *
 *    - any global scoring algorithms
 *    - PID 
 *    - dispatching 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/10/2001
 */

class ICaloParticleTool: public IAlgTool
{
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloParticleTool ; }
  
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
   *  @param  particle pointer to CaloParticle object to be processed
   *  @return status code 
   */  
  StatusCode process    ( CaloParticle* particle ) = 0 ;
  
protected:
  
  /** destructor, virtual and protected  
   */
  virtual ~ICaloParticleTool(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
