// $Id: ICaloMomentumTool.h,v 1.3 2004-02-17 11:51:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOINTERFACES_ICALOMOMENTUMTOOL_H 
#define CALOINTERFACES_ICALOMOMENTUMTOOL_H 1
// Include files
// STD & STL 
#include <functional> 
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
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
  static const InterfaceID& interfaceID() ;
  
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
  
protected:
  
  /// destructor (virtual) 
  virtual ~ICaloMomentumTool() ;

};


// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOMOMENTUMTOOL_H
// ============================================================================
