// $Id: ICaloDigitTool.h,v 1.7 2006-03-22 18:21:51 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.5  2005/01/25 14:12:17  cattanem
// updates for CLHEP 1.9
//
// Revision 1.4  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
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
#ifndef CALOINTERFACES_ICALODIGITTOOL_H
#define CALOINTERFACES_ICALODIGITTOOL_H 1
// Include files
// STD & STL
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
namespace LHCb{
  class    CaloDigit                ;  
};

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
  static const InterfaceID& interfaceID() ;

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
  
  /// destructor, virtual  
  virtual ~ICaloDigitTool() ;

};

// ============================================================================
#endif // CALOINTERFACES_ICALODIGITTOOL_H
// ============================================================================
