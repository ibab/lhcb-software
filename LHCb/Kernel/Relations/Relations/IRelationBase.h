// $Id: IRelationBase.h,v 1.3 2006-06-11 19:37:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/07 09:22:23  ibelyaev
//  update for Win32
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
#include "GaudiKernel/System.h"

/** @class IRelationBase IRelationBase.h Relations/IRelationBase.h
 *  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-14
 */
class IRelationBase 
  : public IInterface 
  , public IUpdateable  
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
  
 protected:
  
  /// virtual destructor 
  virtual ~IRelationBase() ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONBASE_H
// ============================================================================
