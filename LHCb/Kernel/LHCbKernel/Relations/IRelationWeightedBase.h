// $Id: IRelationWeightedBase.h,v 1.1 2004-01-14 15:13:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_IRELATIONWEIGHTEDBASE_H 
#define RELATIONS_IRELATIONWEIGHTEDBASE_H 1
// ============================================================================
// Include files 
// local
#include "Relations/IRelationBase.h"

/** @var IID_IRelationWeightedBase
 *  the unique identifier of interface IRelationWeightedBase
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-14
 */
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
  virtual ~IRelationWeightedBase();
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONWEIGHTEDBASE_H
// ============================================================================
