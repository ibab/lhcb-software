// $Id: IAssociatorWeighted.h,v 1.2 2002-04-25 08:44:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/08 14:26:01  ibelyaev
//  new version of 'Relations'-subpackage
//
// Revision 1.1  2002/04/03 15:35:17  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// ============================================================================
#ifndef RELATIONS_IAssociatorWeighted_H 
#define RELATIONS_IAssociatorWeighted_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelationWeighted.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IAssociatorWeighted IAssociatorWeighted.h
 *  
 *  Generic interface (tool) to access the weighted 
 *  relations from Transient Store 
 *  @see IRelationWeighted
 *  @see IRelationWeighted2D
 *  @see IAssociator
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep
 *  @date   24/03/2002
 */

template<class FROM,class TO,class WEIGHT>
class IAssociatorWeighted:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IAssociatorWeighted<FROM,TO,WEIGHT>   OwnType       ;
  /// shortcut for "direct relations
  typedef IRelationWeighted<FROM,TO,WEIGHT>     DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelationWeighted<TO,FROM,WEIGHT>     InverseType   ;
  
public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)   (non const version)
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType*   direct  ()       = 0 ;
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)   (const version)
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct  () const = 0 ;

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object)   (non const version)
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse ()       = 0 ;
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object)   (const version)
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse () const = 0 ;
  
  /** accessor to relation builder algorithm
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
      Relations::interfaceID( "IAssociatorWeighted"                ,
                              DirectType::FromTypeTraits::   id () ,
                              DirectType::ToTypeTraits::     id () , 
                              DirectType::WeightTypeTraits:: id () , 
                              DirectType::TypeTraits::     version , 0 );
    return s_iid ;
  };

protected:

  /// destructor (virtual and protected) 
  virtual ~IAssociatorWeighted(){}; 

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IAssociatorWeighted_H
// ============================================================================
