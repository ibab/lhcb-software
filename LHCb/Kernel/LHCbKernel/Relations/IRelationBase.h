// $Id: IRelationBase.h,v 1.1 2004-01-14 15:13:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_IRELATIONBASE_H 
#define RELATIONS_IRELATIONBASE_H 1
// ============================================================================
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ClassID.h"

static const InterfaceID IID_IRelationBase ( "IRelationBase", 1, 0 );

/** @class IRelationBase IRelationBase.h Relations/IRelationBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-14
 */
class IRelationBase : public virtual IInterface 
{
public:
  
  /** rebuild/resort  existing relations 
   *
   *  @return status code 
   *
   */
  virtual StatusCode rebuild() = 0 ;
  
  /** remove ALL relations from ALL to ALL objects
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   clear ()  = 0 ;
  
  /** the unique interface ID (static)
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
protected:
  
  /// virtual destructor 
  virtual ~IRelationBase();

};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONBASE_H
// ============================================================================
