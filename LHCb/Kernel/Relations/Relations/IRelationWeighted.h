// $Id: IRelationWeighted.h,v 1.6 2007-03-23 16:40:17 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.6 $
// ============================================================================
#ifndef RELATIONS_IRELATIONWeighted_H
#define RELATIONS_IRELATIONWeighted_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeightedBase.h"
#include "Relations/RelationWeightedTypeTraits.h"
#include "Relations/RelationUtils.h"
// ============================================================================

/** @class IRelationWeighted IRelationWeighted.h
 *
 *  The templated interface to represent the
 *  the unidirectional relationso with the weight
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template< class FROM , class TO , class WEIGHT>
class IRelationWeighted : public IRelationWeightedBase
{
public:
  /// the type traits structure
  typedef Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> TypeTraits ;
  /// actual "FROM" type
  typedef typename TypeTraits::From_                  From_            ;
  typedef typename TypeTraits::From                   From             ;
  /// actual "TO" type
  typedef typename TypeTraits::To_                    To_              ;
  typedef typename TypeTraits::To                     To               ;
  /// actual "WEIGHT" type
  typedef typename TypeTraits::Weight_                Weight_          ;
  typedef typename TypeTraits::Weight                 Weight           ;
  /// iterator type
  typedef typename TypeTraits::iterator               iterator         ;
  /// iterator range
  typedef typename TypeTraits::Range                  Range            ;
  /// shortcut to own type
  typedef IRelationWeighted<FROM,TO,WEIGHT>  OwnType          ;
  /// shortcut to "direct" type
  typedef IRelationWeighted<FROM,TO,WEIGHT>  DirectType       ;
  /// shortcut to "inverse" type
  typedef IRelationWeighted<TO,FROM,WEIGHT>  InverseType      ;
public:
  /** retrive all relations from the object
   *  - relations are returned in the form of iterator pair:
   *  @code 
   *      IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *      From_                      object = ... ;
   *      Range r = irel->relations( object );
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
   *          To     to     = it->to()     ; // get the object
   *          // To  to     = *it          ; // the same
   *          Weight weight = it->weight() ; // "weight" of the relation
   *          From   from   = it->from()   ; // again "from" object!
   *        };
   *  @endcode 
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range relations ( From_ object ) const = 0 ;
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
  /** retrive all relations from the object which has weigth
   *  larger/smaller than the threshold value
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel      = ... ;
   *     From_               object    = ... ;
   *     Weight_             threshold = ... ;
   *     Range r = irel->relations( object , threshold , true );
   *  @endcode 
   *  - the total number of relations is:
   *  @code 
   *     const unsigned nRel    = r.size()             ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *  - the related elements could be retrieved using the explicit loop 
   *  @code 
   *     for( iterator it = r.begin() ; r.end() != it ; ++it )
   *        {
   *          // extract and use the relation
   *          To     to     = it->to()     ; // get the object
   *          // To  to     = *it          ; // the same
   *          Weight weight = it->weight() ; // "weight" of the relation
   *          From   from   = it->from()   ; // again "from" object!
   *        };
   *  @endcode 
   *  Here the obtained @p weight will always be larger that @p threshold 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return pair of iterators for output relations
   */
  virtual Range relations 
  ( From_ object , Weight_ threshold , const bool flag ) const = 0 ;
  /** retrive all relations from the object which has weigth
   *  withing the specified range 
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel      = ... ;
   *     From_               object    = ... ;
   *     Weight_             low       =  ... ;
   *     Weight_             high      = ... ;
   *     Range r = irel->inRange ( object , low , high );
   *  @endcode 
   *  - the total number of relations is:
   *  @code 
   *     const unsigned nRel    = r.size()             ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *  - the related elements could be retrieved using the explicit loop 
   *  @code 
   *     for( iterator it = r.begin() ; r.end() != it ; ++it )
   *        {
   *          // extract and use the relation
   *          To     to     = it->to()     ; // get the object
   *          // To  to     = *it          ; // the same
   *          Weight weight = it->weight() ; // "weight" of the relation
   *          From   from   = it->from()   ; // again "from" object!
   *        };
   *  @endcode 
   *  Here the obtained @p weight will always be larger that @p threshold 
   *  @param  object    the object
   *  @param  low       lower limit of range
   *  @param  high      upper limit of range
   *  @return pair of iterators for output relations
   */
  virtual Range inRange ( From_ object , Weight_ low , Weight_ high ) const = 0 ;
  /** make the relation between 2 objects
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel    = ... ;
   *    From_                      object1 = ... ;
   *    To_                        object2 = ... ;
   *    Weight_                    weight  = ... ; 
   *    irel->relate( object1 , object2 , weight );
   *  @endcode
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @param  weight  the weigth for this relation
   *  @return status  code
   */
  virtual StatusCode relate
  ( From_ object1 , To_ object2 , Weight_ weight ) = 0 ;
  /** remove the concrete relation between objects
   *   - if there are no relations between the given object
   *     the error code will be returned
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel    = ... ;
   *    From_                      object1 = ... ;
   *    To_                        object2 = ... ;
   *    irel->remove( object1 , object2 );
   *  @endcode
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode remove ( From_ object1 , To_ object2 ) = 0 ;
  /** remove all relations FROM the defined object
   *   - if there are no relations from the given onject
   *     the error code will be returned
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *    From_                      object = ... ;
   *    irel->removeFrom( object );
   *  @endcode
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode removeFrom ( From_ object )  = 0 ;
  /** remove all relations TO the defined object
   *   - if there are no relations to the given object
   *     the error code will be returned
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *    To_                        object = ... ;
   *    irel->removeTo( object );
   *  @endcode
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode removeTo ( To_ object )  = 0 ;
  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight
   *  (driven by boolean third argument) 
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    From_                      object    = ... ;
   *    Weight_                    threshold = ... ;
   *    irel->filterFrom( object , threshold , true  );
   *  @endcode
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filterFrom
  ( From_ object , Weight_ threshold , const bool flag )  = 0 ;
  /** filter out the relations TO the defined object, which
   *  have a weight larger/smaller than the threshold weight
   *  (driven by boolean third argument) 
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    To_                        object    = ... ;
   *    Weight_                    threshold = ... ;
   *    irel->filterTo( object , threshold , true  );
   *  @endcode
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filterTo
  ( To_ object , Weight_ threshold , const bool flag )  = 0 ;
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight
   *  (driven by boolean second argument) 
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    Weight_                    threshold = ... ;
   *    irel->filter( threshold , true  );
   *  @endcode
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filter ( Weight_ threshold , const bool flag )  = 0 ;
public:
  /** interface identification (static)
   *
   *  The unique interface identifier is
   *  constructed "on-fly" using hash-technique from
   *  the generic interface name and unique identifiers
   *  of related objects and teh identifier of the type of the weight
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid =
      Relations::interfaceID( System::typeinfoName(typeid(OwnType))) ;
    return s_iid ;
  };
protected:
  /// destructor (virtual and protected)
  virtual ~IRelationWeighted(){};
};
// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_IRELATIONWeighted_H
// ============================================================================
