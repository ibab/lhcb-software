// $Id: IRelationWeightedBase.h,v 1.1.1.1 2004-07-21 07:57:25 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/05/03 15:15:38  cattanem
// v4r6
//
// Revision 1.2  2004/01/22 12:35:32  cattanem
// fix for windows
//
// Revision 1.1  2004/01/14 15:13:02  ibelyaev
//  few tricks to make POOL/ROOT happy
// 
// ============================================================================
#ifndef RELATIONS_IRELATIONWEIGHTEDBASE_H 
#define RELATIONS_IRELATIONWEIGHTEDBASE_H 1
// ============================================================================
// Include files 
// local
#include "Relations/IRelationBase.h"

//  the unique identifier of interface IRelationWeightedBase
static const InterfaceID IID_IRelationWeightedBase 
( "IRelationWeightedBase", 1, 0 );

/** @class IRelationWeightedBase Relations/IRelationWeightedBase.h
 *  
 *  The helper base class for weighted relations 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-14
 */
class IRelationWeightedBase : public virtual IRelationBase
{
public:
  
  /** the unique interface ID (static)
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
protected:

  /// virtual destructor 
  virtual ~IRelationWeightedBase() {};
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONWEIGHTEDBASE_H
// ============================================================================
