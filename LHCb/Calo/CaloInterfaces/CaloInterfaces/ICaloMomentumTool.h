// $Id: ICaloMomentumTool.h,v 1.1 2002-04-02 11:08:12 ibelyaev Exp $
// ============================================================================
// ============================================================================
// ============================================================================
#ifndef CALOINTERFACES_ICALOMOMENTUMTOOL_H 
#define CALOINTERFACES_ICALOMOMENTUMTOOL_H 1
// Include files
// STD & STL 
#include <functional> 
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
// Local 
#include "CaloInterfaces/IIDICaloMomentumTool.h"
// forward declarations 
class       CaloMomentum;         // CaloEvent/Event

/** @class ICaloMomentumTool ICaloMomentumTool.h 
 *
 *  An abstract interface for tools wich deal with CaloMomentum class
 * 
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   21/03/2002
 */

class ICaloMomentumTool:
  public virtual IAlgTool ,
  public std::unary_function<CaloMomentum*,StatusCode>
{
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloMomentumTool ; }
  
  /** The main processing method 
   *  @param  momentum pointer to CaloMomentum object  
   *  @return status code 
   */  
  virtual StatusCode 
  process ( CaloMomentum*  momentum ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  momentum pointer to CaloMomentum object  
   *  @return status code 
   */  
  virtual StatusCode 
  operator() ( CaloMomentum*  momentum  ) const = 0 ;
  
  /** destructor (virtual) 
   */
  virtual ~ICaloMomentumTool(){};

};


// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOMOMENTUMTOOL_H
// ============================================================================
