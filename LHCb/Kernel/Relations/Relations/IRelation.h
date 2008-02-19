// $Id: IRelation.h,v 1.6 2008-02-19 15:26:09 ibelyaev Exp $
// ============================================================================
#ifndef RELATIONS_IRELATION_H
#define RELATIONS_IRELATION_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/RelationTypeTraits.h"
#include "Relations/RelationUtils.h"
#include "Relations/IRelationBase.h"
// ============================================================================
/** @class IRelation IRelation.h Relations/IRelation.h
 *  An absract interface for unidirectional templated relations
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 *  @date   2006-06-09
 */
template<class FROM, class TO>
class IRelation : public IRelationBase
{
public:
  /// the type traits structure
  typedef Relations::RelationTypeTraits<FROM,TO> TypeTraits  ;
  /// "FROM" traits
  typedef typename TypeTraits::From_             From_       ;
  typedef typename TypeTraits::From              From        ;
  /// "TO" traits
  typedef typename TypeTraits::To_               To_         ;
  typedef typename TypeTraits::To                To          ;
  /// iterator type
  typedef typename TypeTraits::iterator          iterator    ;
  /// iterator range
  typedef typename TypeTraits::Range             Range       ;
  /// shortcut to own type
  typedef IRelation<FROM,TO>                     OwnType     ;
  /// shortcut to "direct" type
  typedef IRelation<FROM,TO>                     DirectType  ;
  /// shortcut to "inverse" type
  typedef IRelation<TO,FROM>                     InverseType ;
public:
  /** retrive all relations from the given object object
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel   = ...    ;
   *     From_               object = ...    ;
   *     Range r = irel->relations( object ) ;
   *  @endcode 
   *  - the number of related objects is:
   *  @code 
   *     const unsigned nRel    = r.size() ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *  - the related elements could be retrieved using the explicit loop 
   *  @code 
   *     for( iterator it = r.begin() ; r.end() != it ; ++it )
   *        {
   *          // extract and use the relation
   *          To    to    = it->to()   ; // get the object
   *          // To to    = *it        ; // the same
   *          From  from  = it->from() ; // again "from" object!
   *        };
   *  @endcode 
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations ( From_ object ) const = 0 ;  
  /** retrive ALL relations from ALL objects  
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel   = ...    ;
   *     Range r = irel->relations()         ;
   *  @endcode 
   *  - the total number of relations is:
   *  @code 
   *     const unsigned nRel    = r.size()             ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *  @return pair of iterators for output relations
   */
  virtual Range      relations () const = 0 ;  
  /** make the relation between 2 objects
   *
   *  - if the relation between given 2 object is already exist
   *    the error code will be returned
   *
   *  Example:
   *  @code
   *    From_ object1 = ... ;
   *    To_  object2 = ... ;
   *    irel->relate( object1 , object2 );
   *  @endcode
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode relate ( From_ object1 , To_ object2 ) = 0 ;  
  /** remove the concrete relation between objects
   *   - if there are no relations between the given object
   *     the error code will be returned
   *  Example:
   *  @code
   *    From_ object1 = ... ;
   *    To_   object2 = ... ;
   *    irel->remove( object1 , object2 );
   *  @endcode
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode   remove ( From_ object1 , To_ object2 ) = 0 ;
  /** remove all relations FROM the defined object
   *   - if there are no relations from the given onject
   *     the error code will be returned
   *  Example:
   *  @code
   *    From_ object1 = ... ;
   *    irel->removeFrom( object1 );
   *  @endcode
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeFrom ( From_ object )  = 0 ;  
  /** remove all relations TO the defined object
   *   - if there are no relations to the given object
   *     the error code will be returned
   *  Example:
   *  @code
   *    To_ object1 = ... ;
   *    irel->removeTo( object1 );
   *  @endcode
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeTo ( To_ object )  = 0 ;  
public:
  /** interface identification (static)
   *  The unique interface identifier is constructed "on-fly"
   *  using hash-technique from the generic interface name and 
   *  unique identifiers of related objects 
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid  =
      Relations::interfaceID( System::typeinfoName( typeid(OwnType) ) ) ;
    return s_iid ;
  };
protected:
  /// destructor (virtual and protected)
  virtual ~IRelation() {};
} ;
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_IRELATION_H
// ============================================================================
