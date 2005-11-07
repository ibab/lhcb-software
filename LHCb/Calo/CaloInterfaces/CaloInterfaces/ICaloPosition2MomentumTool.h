// $Id: ICaloPosition2MomentumTool.h,v 1.4 2005-11-07 12:08:25 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOINTERFACES_ICALOPOSITION2MOMENTUMTOOL_H 
#define CALOINTERFACES_ICALOPOSITION2MOMENTUMTOOL_H 1
// Include files
// STD & STL 
#include <functional> 
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
// forward declarations 
class       CaloPosition;         // CaloEvent/Event 
class       CaloMomentum;         // CaloEvent/Event 


/** @class ICaloPosition2MomentumTool ICaloPosition2MomentumTool.h
 *  
 *  An abstract interface for tool which evaluates CaloMomentum object 
 *  from CaloPosition object
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   21/03/2002
 */
class ICaloPosition2MomentumTool:
  public virtual IAlgTool ,
  public std::unary_function<const CaloPosition*, CaloMomentum*>
{
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;

  /** the main processing method 
   *  @param position pointer to CaloPosition object
   *  @return pointer to CaloMomentum object evaluated form CaloPosition object
   */
  virtual CaloMomentum* 
  process( const CaloPosition* position ) const = 0 ;
  
  /** The main processing method 
   *  @param position pointer to CaloPosition object
   *  @param momentum pointer to CaloMomentum object evaluated 
   *                  from CaloPosition object
   */
  virtual StatusCode  
  process( const CaloPosition* position , 
           CaloMomentum*&      momentum ) const = 0 ;
  
  /** the main processing method (functor interface)
   *  @param position pointer to CaloPosition object
   *  @return pointer to CaloMomentum object evaluated form CaloPosition object
   */
  virtual CaloMomentum* 
  operator() ( const CaloPosition* position ) const = 0 ;

protected:
  
  /// destructor (virtual)
  virtual ~ICaloPosition2MomentumTool();
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOPOSITION2MOMENTUMTOOL_H
// ============================================================================
