// $Id: ICaloHypoTool.h,v 1.4 2004-02-17 11:51:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/17 11:42:28  ibelyaev
//  fix in Likel(i,y)hoods
//
// Revision 1.2  2002/04/02 11:08:12  ibelyaev
//  new interfaces
//
// Revision 1.1.1.2  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOHYPOTOOL_H 
#define CALOINTERFACES_ICALOHYPOTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"

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

class ICaloHypoTool: 
  public virtual IAlgTool ,
  public std::unary_function<CaloHypo*,StatusCode>
{
  
 public:
  
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const = 0 ;
  
 protected: 
  
  /// destructor, virtual 
  virtual ~ICaloHypoTool();
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
