// $Id: IRelationTool.h,v 1.1 2002-04-03 15:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATIONTOOL_H 
#define RELATIONS_IRELATIONTOOL_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelation.h"
#include "Relations/IRelation2D.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IRelationTool IRelationTool.h Relations/IRelationTool.h
 *  
 *  Generic interface (tool) to access the relations from Transient Store 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep
 *  @date   24/03/2002
 */

template<class FROM,class TO>
class IRelationTool:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IRelationTool<FROM,TO>  OwnType       ;
  /// shortcut for "direct relations
  typedef IRelation<FROM,TO>      DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelation<TO,FROM>      InverseType   ;
  /// shortcut for "direct 2D-relations"
  typedef IRelation2D<FROM,TO>    Direct2DType  ;


public:

  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)  (non const version)
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType*   direct    ()       = 0 ;
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (const version)
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct    () const = 0 ;

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (non const version)
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse   ()       = 0 ;
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (const version)
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse   () const = 0 ;
  
  /** accessor to relation builder algoritthm
   *  @see IAlgorithm
   *  @return pointer to builder algorithm
   */
  virtual IAlgorithm*         algorithm () const = 0 ;
  
  /** interface identification (static)
   *  @attention the unique interface identifier is constructed "on-fly"
   *  using hash-technique from the generic interface name and 
   *  unique identifiers of related objects 
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid  =
      Relations::interfaceID( "IRelationTool"                   ,
                              DirectType::FromTypeTraits::id () ,
                              DirectType::ToTypeTraits::id   () , 
                              0                                 ,
                              DirectType::TypeTraits::  version , 0 );
    return s_iid ;
  };

protected:

  /// destructor (virtual and protected) 
  virtual ~IRelationTool(){}; 

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IRELATIONTOOL_H
// ============================================================================
