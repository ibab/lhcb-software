// $Id: RelationWeighted1D.h,v 1.4 2006-01-27 13:25:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/02/16 19:59:35  ibelyaev
//  few minor fixes to enable 'lcgdict' processing
// 
// ============================================================================
#ifndef RELATIONS_RelationWeighted1D_H 
#define RELATIONS_RelationWeighted1D_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/PragmaWarnings.h"
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// From Relation
// ============================================================================
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeighted.h"
// ============================================================================

/** @class RelationWeighted1D RelationWeighted1D.h 
 * 
 *  The simplest implementation of unidirectional weighted relations.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 */
template<class FROM, class TO, class WEIGHT>
class RelationWeighted1D :
  public DataObject,
  public IRelationWeighted<FROM,TO,WEIGHT>
{
  
public:
  
  /// shortcut for own type 
  typedef RelationWeighted1D<FROM,TO,WEIGHT>              OwnType ;
  /// shortcut for inverse type 
  typedef RelationWeighted1D<TO,FROM,WEIGHT>              InvType ;
  /// short cut for interface
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase   ;
  /// import "Range" type from the base 
  typedef typename IBase::Range                           Range   ;
  /// import "From"  type from the base 
  typedef typename IBase::From                            From    ;
  /// import "To"    type from the base 
  typedef typename IBase::To                              To      ;
  /// import "Weight" type from the base 
  typedef typename IBase::Weight                          Weight  ;
  /// short cut for the actual implementation type 
  typedef typename Relations::RelationWeighted<FROM,TO,WEIGHT>     Base    ;
  // shortcut for "direct" interface 
  typedef typename IBase::DirectType                      IDirect        ;
  // shortcut for "inverse" interface 
  typedef typename IBase::InverseType                     IInverse       ;
  
public:
  
  /// the standard/default constructor
  RelationWeighted1D 
  ( const size_t reserve = 0 ) 
    : DataObject () 
    , IBase      () 
    , m_base     ( reserve ) 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** constructor from any direct interface
   *  @param copy object to be copied 
   */
  RelationWeighted1D 
  ( const IDirect& copy  )
    : DataObject () 
    , IBase      () 
    , m_base     ( copy )
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** constructor from "inverted interface"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  RelationWeighted1D 
  ( const IInverse& inv  , 
    const int       flag ) 
    : DataObject () 
    , IBase      () 
    , m_base     ( inv  , flag )
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** copy constructor 
   *  @param copy object to be copied 
   */
  RelationWeighted1D 
  ( const OwnType& copy  )
    : DataObject ( copy         ) 
    , IBase      ( copy         ) 
    , m_base     ( copy.m_base  )
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /// destructor (virtual)
  virtual ~RelationWeighted1D ()
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().decrement( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /// the type name 
  const std::string& type() const 
  {
    static const std::string s_type( System::typeinfoName( typeid(OwnType) ) ) ;
    return s_type ;
  };
  
  /** object identifier (static method)
   *  @see DataObject 
   *  @see ObjectTypeTraits
   *  @see RelationWeightedTypeTraits
   *  @return the unique class identifier 
   */
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "RelationWeighted1D"    , 
                       FromTypeTraits::   id() , 
                       ToTypeTraits::     id() , 
                       WeightTypeTraits:: id() ,
                       TypeTraits::    version , 0 ) ;
    return s_clid ;
  };
  
  /** object identification (virtual method)
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  virtual const CLID& clID()     const { return classID() ; }
  
  /** object serialization for writing
   *  @see DataObject 
   *  @param  s reference to the output stream
   *  @return   reference to the output stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const 
  {
    typedef typename FromTypeTraits::Apply        ApplyF     ;
    typedef typename WeightTypeTraits::Apply      ApplyW     ;
    typedef typename ToTypeTraits::Apply          ApplyT     ;
    typedef typename FromTypeTraits::Serializer   SerializeF ;
    typedef typename WeightTypeTraits::Serializer SerializeW ;
    typedef typename ToTypeTraits::Serializer     SerializeT ;
    // serialize the base class
    DataObject::serialize( s );
    // get all relations 
    typename RelationWeighted1D<FROM, TO, WEIGHT>::Range range =
      i_relations() ;
    // serialize the number of relations 
    unsigned long _size = range.size() ;
    s << _size ;
    // serialise all relations
    for (typename RelationWeighted1D<FROM, TO, WEIGHT>::iterator
           entry = range.begin() ; range.end() != entry ; ++entry ) 
      {    
        SerializeF::serialize 
          ( s , ApplyF::apply ( (*entry).first.first   , this ) );
        SerializeW::serialize 
          ( s , ApplyW::apply ( (*entry).first.second  , this ) );
        SerializeT::serialize 
          ( s , ApplyT::apply ( (*entry).second        , this ) );
      };
    ///
    return s ;
  };

  /** object serialization for reading
   *  @see DataObject 
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )       
  {
    typedef typename FromTypeTraits::Apply        ApplyF     ;
    typedef typename WeightTypeTraits::Apply      ApplyW     ;
    typedef typename ToTypeTraits::Apply          ApplyT     ;
    typedef typename FromTypeTraits::Serializer   SerializeF ;
    typedef typename WeightTypeTraits::Serializer SerializeW ;
    typedef typename ToTypeTraits::Serializer     SerializeT ;
    // clear all existing relations 
    i_clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    m_base.reserve( _size ) ;
    typename IBase::From   from   ;
    typename IBase::Weight weight ;
    typename IBase::To     to     ;
    while( _size-- > 0 )
      {
        //
        SerializeF::serialize ( s , ApplyF::apply ( from   , this ) ) ;
        SerializeW::serialize ( s , ApplyW::apply ( weight , this ) ) ;
        SerializeT::serialize ( s , ApplyT::apply ( to     , this ) ) ;
        //
        i_relate( from , to , weight ) ;
      }
    return s ;
  };
  
public:  // major functional methods (fast, 100% inline)
  
  /// retrive all relations from the object (fast,100% inline)
  inline   Range i_relations
  ( const  From& object) const 
  { return m_base.i_relations ( object ) ;} 
  
  /// retrive ALL relations from ALL objects (fast,100% inline)
  inline   Range i_relations() const
  { return m_base.i_relations (        ) ;}
  
  /// retrive all relations from the object (fast,100% inline)
  inline   Range      i_relations
  ( const  From&      object,
    const  Weight&    threshold ,
    const  bool       flag      ) const 
  { return m_base.i_relations ( object , threshold , flag ) ;}
  
  /// retrive all relations from the object (fast,100% inline)
  inline   Range      i_inRange
  ( const  From&      object ,
    const  Weight&    low    ,
    const  Weight&    high   ) const 
  { return m_base.i_inRange ( object , low , high ) ;}

  /// make the relation between 2 objects (fast,100% inline)
  inline   StatusCode i_relate 
  ( const  From&      object1 , 
    const  To&        object2 ,
    const  Weight&    weight  ) 
  { return m_base.i_relate ( object1 , object2 , weight ) ;}
  
  /// remove the concrete relation between objects (fast,100% inline)
  inline   StatusCode i_remove 
  ( const  From&      object1 , 
    const  To&        object2 ) 
  { return m_base.i_remove ( object1 , object2 ) ; }
  
  /// remove all relations FROM the defined object (fast,100% inline)
  inline   StatusCode i_removeFrom 
  ( const  From&      object )
  { return m_base.i_removeFrom ( object ) ; }
  
  /// remove all relations TO the defined object (fast,100% inline)
  inline   StatusCode i_removeTo 
  ( const  To&        object )
  { return m_base.i_removeTo ( object ) ; }
  
  /// filter out the relations FROM the defined object (fast,100% inline)
  inline   StatusCode i_filterFrom 
  ( const  From&      object    ,
    const  Weight&    threshold ,
    const  bool       flag      )  
  { return m_base.i_filterFrom ( object , threshold , flag ) ; }
  
  /// filter out the relations TO the defined object (fast,100% inline)
  inline   StatusCode i_filterTo 
  ( const  To&        object    ,
    const  Weight&    threshold ,
    const  bool       flag      )  
  { return m_base.i_filterTo ( object , threshold , flag ) ; }
  
  /// filter out all relations (fast,100% inline)
  inline   StatusCode i_filter 
  ( const  Weight&    threshold ,
    const  bool       flag      )  
  { return m_base.i_filter ( threshold , flag ) ; }
  
  /// remove ALL relations from ALL objects to ALL objects (fast,100% inline)
  inline   StatusCode i_clear () { return m_base.i_clear ( ) ; }
  
  /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
  inline  StatusCode i_rebuild() 
  { return m_base.i_rebuild() ; };
  
  /** make the relation between 2 objects (fast,100% inline)
   *  the subsequent call for i_sort is mandatory
   */
  inline   void i_push 
  ( const  From&      object1 , 
    const  To&        object2 ,
    const  Weight&    weight  ) 
  { m_base.i_push ( object1 , object2 , weight ) ;}
  
  /** (re)sort of the table 
   *   mandatory to use after i_push 
   */
  inline   void  i_sort() { m_base.i_sort () ; }

public:  // abstract methods from interface
  
  /** retrive all relations from the object
   *  
   *  - relations are returned in the form of iterator pair:
   *     \n IRelation<FROM,TO,WEIGHT>* irel   = ... ;
   *     \n From                       object = ... ; 
   *     \n Range r = irel->relations( object );
   *  
   *  - the number of related object is:
   *     \n    const unsigned nRel = r.end()  - r.begin() ;
   *     \n // const unsigned nRel = r.second - r.first   ; // the same!
   *
   *  - the related elements could be retrieved using the iterations:
   *     \n for( iterator it = r.begin() ; r.end() != it ; ++it ){
   *     \n /// extract and use the relation
   *     \n To     to     = it->to()     ; // get the "to" object  
   *     \n // To  to     = *it          ; // the same   
   *     \n Weight weight = it->weight() ; // get the weight 
   *     \n From   from   = it->weight() ; // again get the "from" object
   *     \n };
   *
   *  @see    IRelationWeighted1D 
   *  @see    RelationWeighted1DBase
   *  @see    RelationWeighted1DTypeTraits
   *  @param  object  the object
   *  @return pair of iterators for output relations   
   */
  virtual Range relations
  ( const From& object) const { return i_relations ( object ) ; }
  
  /** retrive ALL relations from ALL objects
   *  
   *  @see    IRelationWeighted1D 
   *  @see    RelationWeighted1DBase
   *  @see    RelationWeighted1DTypeTraits
   *  @return pair of iterators for output relations   
   */
  virtual Range relations() const { return i_relations () ; }
  
  /** retrive all relations from the object which has weigth 
   *  larger/smaller than the threshold value 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return pair of iterators for output relations   
   */
  virtual  Range      relations
  ( const  From&      object,
    const  Weight&    threshold ,
    const  bool       flag      ) const 
  { return i_relations ( object , threshold , flag ) ; }

  /** retrive all relations from the object which has weigth 
   *  withing the specified range 
   *  @param  object  the object
   *  @param  low     low  threshold value for the weight 
   *  @param  high    high threshold value for the weight 
   *  @return pair of iterators for output relations   
   */
  virtual  Range      inRange 
  ( const  From&      object ,
    const  Weight&    low    ,
    const  Weight&    high   ) const 
  { return i_inRange ( object , low , high ) ; }

  /** make the relation between 2 objects 
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @param  weight  the weigth for the relation 
   *  @return status  code 
   */
  virtual  StatusCode relate 
  ( const  From&      object1 , 
    const  To&        object2 ,
    const  Weight&    weight  ) 
  { return i_relate( object1 , object2 , weight ) ; }
  
  /** remove the concrete relation between objects
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status  code 
   */
  virtual  StatusCode remove 
  ( const  From&      object1 , 
    const  To&        object2 ) 
  { return i_remove ( object1 , object2 ) ; }
  
  /** remove all relations FROM the defined object
   *  @param  object  smart reference to the object
   *  @return status code 
   */
  virtual  StatusCode removeFrom 
  ( const  From&      object )
  { return i_removeFrom ( object ) ; }
  
  /** remove all relations TO the defined object
   *  @param object  smart reference to the object
   *  @return status code 
   */
  virtual  StatusCode removeTo 
  ( const  To&        object )
  { return i_removeTo( object ) ; }
  
  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filterFrom 
  ( const  From&      object    ,
    const  Weight&    threshold ,
    const  bool       flag      )  
  { return i_filterFrom ( object , threshold , flag ) ; }
  
  /** filter out the relations TO the defined object, which
   *  have a weight larger/smaller than the threshold weight 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filterTo 
  ( const  To&        object    ,
    const  Weight&    threshold ,
    const  bool       flag      )  
  { return i_filterTo ( object , threshold , flag ) ; }
  
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight 
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filter 
  ( const  Weight&    threshold ,
    const  bool       flag      )  
  { return i_filter ( threshold , flag ) ; }
  
  /** remove ALL relations from ALL objects to ALL objects 
   *  @return status code 
   */
  virtual  StatusCode clear () { return i_clear () ; }
  
  /** rebuild ALL relations from ALL  object to ALL objects 
   *
   *  @see IRelationBase 
   *  @return status code
   */
  virtual  StatusCode rebuild() { return i_rebuild () ; };
  
  /** update the object after POOL/ROOT reading 
   *  @see IUpdateable 
   *  @param flag    0 - update after read, 1 - update before write 
   *  @return status code
   */
  virtual StatusCode update( int flag ) 
  {
    if( 0 == flag ) { return i_rebuild() ; }
    return StatusCode::SUCCESS ;
  };
  
public:
  
  /** query the interface
   *  @see    IRelation
   *  @see    IInterface
   *  @param  id  interface identifier
   *  @param  ret placeholder for returned interface 
   *  @return status code
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& id , void** ret )
  {
    if( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
    if( IInterface::interfaceID() == id )
      { *ret = static_cast<IInterface*> ( this ); }
    else if( IBase::interfaceID() == id )
      { *ret = static_cast<IBase*>      ( this ); }
    else                     { return StatusCode::FAILURE ; } //  RETURN !!!
    ///
    addRef() ;
    return StatusCode::SUCCESS ;
  };
  
  /** increase the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long release () { return  DataObject::release () ; }
  
private:
  
  Base m_base ;

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RelationWeighted1D_H
// ============================================================================
