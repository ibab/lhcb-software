// $Id: IRelationWeighted.h,v 1.8 2004-05-03 15:15:38 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2004/01/16 15:49:05  cattanem
// fix inheritance
//
// Revision 1.6  2004/01/14 15:13:02  ibelyaev
//  few tricks to make POOL/ROOT happy
//
// Revision 1.5  2003/01/17 14:07:01  sponce
// support for gcc 3.2
//
// Revision 1.4  2002/05/10 12:29:42  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes 10 May 2002
//
// ============================================================================
#ifndef RELATIONS_IRELATIONWeighted_H
#define RELATIONS_IRELATIONWeighted_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// Relation
#include "Relations/Relations.h"
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeightedBase.h"

/** @class IRelationWeighted IRelationWeighted.h
 *
 *  The templated interface to represent the
 *  the unidirectional relationso with the weight
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template< class FROM , class TO , class WEIGHT>
class IRelationWeighted :
  public virtual IRelationWeightedBase
{

public:

  /// the type traits structure
  typedef Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> TypeTraits ;
  /// "FROM" traits
  typedef typename TypeTraits::FromTypeTraits         FromTypeTraits   ;
  /// "TO" traits
  typedef typename TypeTraits::ToTypeTraits           ToTypeTraits     ;
  /// "WEIGHT" traits
  typedef typename TypeTraits::WeightTypeTraits       WeightTypeTraits ;
  /// actual "FROM" type
  typedef typename TypeTraits::From                   From             ;
  /// actual "TO" type
  typedef typename TypeTraits::To                     To               ;
  /// actual "WEIGHT" type
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
   *
   *  - relations are returned in the form of iterator pair:
   *  @code 
   *      IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *      From                       object = ... ;
   *      Range r = irel->relations( object );
   *  @endcode
   *
   *  - the number of related objects is:
   *  @code 
   *     const unsigned nRel    = r.size() ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *
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
   *
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations
  ( const From&      object    ) const = 0 ;

  /** retrive ALL relations from ALL objects
   *
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel   = ...    ;
   *     Range r = irel->relations()         ;
   *  @endcode 
   * 
   *  - the total number of relations is:
   *  @code 
   *     const unsigned nRel    = r.size()             ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *
   *  @return pair of iterators for output relations
   */
  virtual Range      relations () const = 0 ;

  /** retrive all relations from the object which has weigth
   *  larger/smaller than the threshold value
   *
   *  - relations are returned in the form of @p Range object 
   *  @code 
   *     IRelation<FROM,TO>* irel      = ... ;
   *     From                object    = ... ;
   *     Weight              threshold = ... ;
   *     Range r = irel->relations( object , threshold , true );
   *  @endcode 
   * 
   *  - the total number of relations is:
   *  @code 
   *     const unsigned nRel    = r.size()             ;
   *     // const unsigned nRel = r.end()  - r.begin() ; // the same!
   *     // const unsigned nRel = r.second - r.first   ; // the same!
   *  @endcode 
   *
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
   *
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return pair of iterators for output relations
   */
  virtual Range      relations
  ( const From&      object    ,
    const Weight&    threshold ,
    const bool       flag      ) const = 0 ;

  /** make the relation between 2 objects
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel    = ... ;
   *    From                       object1 = ... ;
   *    To                         object2 = ... ;
   *    Weight                     weight  = ... ; 
   *    irel->relate( object1 , object2 , weight );
   *  @endcode
   *
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @param  weight  the weigth for this relation
   *  @return status  code
   */
  virtual StatusCode relate
  ( const From&      object1 ,
    const To&        object2 ,
    const Weight&    weight  ) = 0 ;
  
  /** remove the concrete relation between objects
   *
   *   - if there are no relations between the given object
   *     the error code will be returned
   *
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel    = ... ;
   *    From                       object1 = ... ;
   *    To                         object2 = ... ;
   *    irel->remove( object1 , object2 );
   *  @endcode
   *
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode remove
  ( const From&      object1 ,
    const To&        object2 ) = 0 ;

  /** remove all relations FROM the defined object
   *
   *   - if there are no relations from the given onject
   *     the error code will be returned
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *    From                       object = ... ;
   *    irel->removeFrom( object );
   *  @endcode
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode removeFrom
  ( const From&      object )  = 0 ;
  
  /** remove all relations TO the defined object
   *
   *   - if there are no relations to the given object
   *     the error code will be returned
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *    To                         object = ... ;
   *    irel->removeTo( object );
   *  @endcode
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode removeTo
  ( const To&        object )  = 0 ;

  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight
   *  (driven by boolean third argument) 
   * 
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    From                       object    = ... ;
   *    Weight                     threshold = ... ;
   *    irel->filterFrom( object , threshold , true  );
   *  @endcode
   *
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filterFrom
  ( const From&      object    ,
    const Weight&    threshold ,
    const bool       flag      )  = 0 ;

  /** filter out the relations TO the defined object, which
   *  have a weight larger/smaller than the threshold weight
   *  (driven by boolean third argument) 
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    To                         object    = ... ;
   *    Weight                     threshold = ... ;
   *    irel->filterTo( object , threshold , true  );
   *  @endcode
   *
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filterTo
  ( const To&        object    ,
    const Weight&    threshold ,
    const bool       flag      )  = 0 ;
  
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight
   *  (driven by boolean second argument) 
   *
   *  Example:
   *  @code
   *    IRelation<FROM,TO,WEIGHT>* irel      = ... ;
   *    Weight                     threshold = ... ;
   *    irel->filter( threshold , true  );
   *  @endcode
   *
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filter
  ( const Weight&    threshold ,
    const bool       flag      )  = 0 ;
  
  /** filter out all "to-many" relations and convert them
   *  into "to-one" relations, keeping only the
   *  relations with the largest/smallest weight
   *  have a weight larger/smaller than the threshold weight
   *  @param  flag   flag for larger/smaller
   *  @return status code
   */
  // virtual StatusCode filter
  // ( const bool       flag      )  = 0 ;
  
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
      Relations::interfaceID( "IRelationWeighted"     ,
                              FromTypeTraits::   id() ,
                              ToTypeTraits::     id() ,
                              WeightTypeTraits:: id() ,
                              TypeTraits::version     , 0 ) ;
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
