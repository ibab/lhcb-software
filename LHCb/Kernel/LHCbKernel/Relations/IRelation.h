// $Id: IRelation.h,v 1.2 2002-04-03 15:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATION_H
#define RELATIONS_IRELATION_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
/// local
#include "Relations/RelationTypeTraits.h"
#include "Relations/RelationUtils.h"
/// forward declarations
template <class OBJECT>
class SmartRef            ; // from GaudiKernel

/** @class IRelation IRelation.h Relations/IRelation.h
 *
 *  An absract interface for unidirectional templated relations
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template<class FROM, class TO>
class IRelation :
  public virtual IInterface
{
public:
  
  /// the type traits structure
  typedef Relations::RelationTypeTraits<FROM,TO> TypeTraits     ;
  /// "FROM" traits
  typedef TypeTraits::FromTypeTraits             FromTypeTraits ;
  /// "TO" traits
  typedef TypeTraits::ToTypeTraits               ToTypeTraits   ;
  /// actual "FROM" type
  typedef TypeTraits::From                       From           ;
  /// actual "TO" type
  typedef TypeTraits::To                         To             ;
  /// iterator type
  typedef TypeTraits::iterator                   iterator       ;
  /// iterator range
  typedef TypeTraits::Range                      Range          ;
  /// shortcut to own type
  typedef IRelation<FROM,TO>                     OwnType        ;
  /// shortcut to "direct" type
  typedef IRelation<FROM,TO>                     DirectType     ;
  /// shortcut to "inverse" type
  typedef IRelation<TO,FROM>                     InverseType    ;

public:

  /** retrive all relations from the given object object
   *
   *  - relations are returned in the form of iterator pair:
   *     \n IRelation<FROM,TO>* irel   = ... ;
   *     \n From                object = ... ;
   *     \n Range r = irel->relations( object );
   *
   *  - the number of related object is:
   *     \n    const unsigned nRel = r.end()  - r.begin() ;
   *     \n // const unsigned nRel = r.second - r.first  ; // the same!
   *
   *  - the related elements could be retrieved using the iterations:
   *     \n for( iterator it = r.begin() ; r.end() != it ; ++it ){
   *     \n /// extract and use the relation
   *     \n To    to    = it->to()   ; // get the object
   *     \n // To to    = *it        ; // the same
   *     \n From  from  = it->from() ; // again "from" object!
   *     \n };
   *
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations
  ( const From&      object    ) const = 0 ;

  /** retrive ALL relations from ALL objects  
   *
   *  - relations are returned in the form of iterator pair:
   *     \n IRelation<FROM,TO>* irel   = ... ;
   *     \n Range r = irel->relations();
   *
   *  - the number of related object is:
   *     \n    const unsigned nRel = r.end()  - r.begin() ;
   *     \n // const unsigned nRel = r.second - r.first  ; // the same!
   *
   *  - the related elements could be retrieved using the iterations:
   *     \n for( iterator it = r.begin() ; r.end() != it ; ++it ){
   *     \n /// extract and use the relation
   *     \n To    to    = it->to()   ; // get the object
   *     \n // To to    = *it        ; // the same
   *     \n From  from  = it->from() ; // again "from" object!
   *     \n };
   *
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations () const = 0 ;

  /** make the relation between 2 objects
   *
   *  - if the relation between given 2 object is already exist
   *    the error code will be returned
   *
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode relate
  ( const From&      object1 ,
    const To&        object2 ) = 0 ;

  /** remove the concrete relation between objects
   *
   *   - if there are no relations between the given object
   *     the error code will be returned
   *
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode   remove
  ( const From&        object1 ,
    const To&          object2 ) = 0 ;

  /** remove all relations FROM the defined object
   *
   *   - if there are no relations from the given onject
   *     the error code will be returned
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeFrom
  ( const From&        object )  = 0 ;
  
  /** remove all relations TO the defined object
   *
   *   - if there are no relations to the given object
   *     the error code will be returned
   *
   *  @attention the method potentially could be relatively slow
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeTo
  ( const To&          object )  = 0 ;

  /** remove ALL relations from ALL to ALL objects
   *
   *  @attention the method potentially could be relatively slow
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   clear ()  = 0 ;
  
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
      Relations::interfaceID( "IRelation"          ,
                              FromTypeTraits::id() ,
                              ToTypeTraits::id  () , 
                              0                    ,
                              TypeTraits::version  , 0 );
    return s_iid ;
  };


protected:
  
  /// virtual destructor
  virtual ~IRelation() {};

};


#ifndef WIN32
template<class T>
class IRelation<T,T> : public virtual IInterface {};
#endif

// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_IRELATION_H
// ============================================================================
