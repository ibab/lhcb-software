// $Id: IRelationBase.h,v 1.4 2004-05-03 15:15:38 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/01/15 10:54:39  cattanem
// fix for Windows
//
// Revision 1.2  2004/01/14 16:30:25  ibelyaev
//  update for new interface IUpdateable
//
// Revision 1.1  2004/01/14 15:13:02  ibelyaev
//  few tricks to make POOL/ROOT happy
// 
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
#include "GaudiKernel/IUpdateable.h"

static const InterfaceID IID_IRelationBase ( "IRelationBase", 1, 0 );

/** @class IRelationBase IRelationBase.h Relations/IRelationBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-14
 */
class IRelationBase : public virtual IInterface , 
                      public virtual IUpdateable  
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
   *  @return status code
   */
  virtual StatusCode   clear ()  = 0 ;
  
  /** the unique interface ID (static)
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
  /// virtual destructor 
  virtual ~IRelationBase() {};
};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONBASE_H
// ============================================================================
