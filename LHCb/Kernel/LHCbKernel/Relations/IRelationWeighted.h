// $Id: IRelationWeighted.h,v 1.1 2002-03-18 19:32:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATIONWeighted_H
#define RELATIONS_IRELATIONWeighted_H 1
// Include files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
// Relation
#include "Relations/Relations.h"
#include "Relations/RelationUtils.h"

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
  public virtual IInterface
{

public:

  /// the type traits structure
  typedef Relations::RelationWeightedTypeTraits<FROM,TO,WEIGHT> TypeTraits ;
  /// "FROM" traits
  typedef TypeTraits::FromTypeTraits         FromTypeTraits   ;
  /// "TO" traits
  typedef TypeTraits::ToTypeTraits           ToTypeTraits     ;
  /// "WEIGHT" traits
  typedef TypeTraits::WeightTypeTraits       WeightTypeTraits ;
  /// actual "FROM" type
  typedef TypeTraits::From                   From             ;
  /// actual "TO" type
  typedef TypeTraits::To                     To               ;
  /// actual "WEIGHT" type
  typedef TypeTraits::Weight                 Weight           ;
  /// iterator type
  typedef TypeTraits::iterator               iterator         ;
  /// iterator range
  typedef TypeTraits::Range                  Range            ;
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
   *     \n IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *     \n From                       object = ... ;
   *     \n Range r = irel->relations( object );
   *
   *  - the number of related object is:
   *     \n    const unsigned nRel = r.begin() - r.end() ;
   *     \n // const unsigned nRel = r.second  - r.first ; // the same
   *
   *  - the related elements could be retrieved using the iterations:
   *     \n for( iterator it = r.begin() ; r.end() != it ; ++it ){
   *     \n /// extract and use the relation
   *     \n To     to     = it->to()      ; // get the object
   *     \n // To  to     = *it           ; // the same
   *     \n Weight weight = it->weight () ; // get the weight
   *     \n From   from   = it->from   () ; // again get the "from" object
   *     \n };
   *
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations
  ( const From&      object    ) const = 0 ;
  
  /** retrive all relations from the object which has weigth
   *  larger/smaller than the threshold value
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
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode remove
  ( const From&      object1 ,
    const To&        object2 ) = 0 ;

  /** remove all relations FROM the defined object
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode remove
  ( const From&      object )  = 0 ;

  /** remove all relations TO the defined object
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode remove
  ( const To&        object )  = 0 ;

  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filter
  ( const From&      object    ,
    const Weight&    threshold ,
    const bool       flag      )  = 0 ;
  
  /** filter out the relations TO the defined object, which
   *  have a weight larger/smaller than the threshold weight
   *  @param  object    the object
   *  @param  threshold threshold value for the weight
   *  @param  flag      flag for larger/smaller
   *  @return status code
   */
  virtual StatusCode filter
  ( const To&        object    ,
    const Weight&    threshold ,
    const bool       flag      )  = 0 ;
  
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight
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
  //  virtual StatusCode filter
  // ( const bool       flag      )  = 0 ;

  /** static interface identification
   *  @attention the unique interface identifier is 
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
  
  /// destructor (virtual)
  virtual ~IRelationWeighted(){};

};


// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_IRELATIONWeighted_H
// ============================================================================
