// $Id: IRelationWeightedBase.h,v 1.2 2006-02-07 09:22:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATIONWEIGHTEDBASE_H 
#define RELATIONS_IRELATIONWEIGHTEDBASE_H 1
// ============================================================================
// Include files 
// local
#include "Relations/IRelationBase.h"


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
  virtual ~IRelationWeightedBase() ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_IRELATIONWEIGHTEDBASE_H
// ============================================================================
