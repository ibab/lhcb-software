// $Id: IAssociator.h,v 1.1 2002-04-08 14:26:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/03 15:35:17  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// ============================================================================
#ifndef RELATIONS_IAssociator_H 
#define RELATIONS_IAssociator_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelation.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IAssociator IAssociator.h Relations/IAssociator.h
 *  
 *  Generic interface (tool) to access the relations from Transient Store 
 *
 *  @see IRelation
 *  @see IRelation2D
 *  @see IAssociatorWeighted
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep
 *  @date   24/03/2002
 */

template<class FROM,class TO>
class IAssociator:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IAssociator<FROM,TO>    OwnType       ;
  /// shortcut for "direct relations
  typedef IRelation<FROM,TO>      DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelation<TO,FROM>      InverseType   ;

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
      Relations::interfaceID( "IAssociator"                     ,
                              DirectType::FromTypeTraits::id () ,
                              DirectType::ToTypeTraits::id   () , 
                              0                                 ,
                              DirectType::TypeTraits::  version , 0 );
    return s_iid ;
  };

protected:
  
  /// destructor (virtual and protected) 
  virtual ~IAssociator(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IAssociator_H
// ============================================================================
