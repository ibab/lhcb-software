// $Id: RelationWeighted2D.h,v 1.1 2002-03-18 19:32:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_BIRELATIONWITHWEIGHTOBJECT_H 
#define RELATIONS_BIRELATIONWITHWEIGHTOBJECT_H 1
// Include files
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// From Relations
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"
// Forward declaration
class StreamBuffer;

/** @class RelationWeighted2D RelationWeighted2D.h 
 *  
 *  Templated object, which implements the bidirectional 
 *  relations with the weight
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */
template<class FROM, class TO, class WEIGHT>
class RelationWeighted2D :
  public virtual IRelationWeighted<FROM,TO,WEIGHT> ,
  public virtual IRelationWeighted<TO,FROM,WEIGHT> ,
  public         DataObject
{
public:
  
  /// shortcut for own type 
  typedef RelationWeighted2D<FROM,TO,WEIGHT>              OwnType ;
  /// shortcut for inverse type 
  typedef RelationWeighted2D<TO,FROM,WEIGHT>              InvType ;
  /// shortcut for "main"/"direct" interface    
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase   ;
  /// shortcut for "main"/"direct" interface    
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase1  ;
  /// shortcut for "inverse"       interface    
  typedef IRelationWeighted<TO,FROM,WEIGHT>               IBase2  ;
  /// shortcut for actual implementation of direct relations 
  typedef Relations::RelationWeightedBase<FROM,TO,WEIGHT> Base1   ;
  /// shortcut for actual implementation of inverse relations 
  typedef Relations::RelationWeightedBase<TO,FROM,WEIGHT> Base2   ;
  
public:
  
  /// Standard/default constructor
  RelationWeighted2D ( const typename Base1::size_type reserve = 0 ) 
    : DataObject () , m_base1 ( reserve ) , m_base2 ( reserve ) {};
  
  /** constructor from the inverse type!
   *  @attention it is the way to "invert" all relations!
   *  @param inv the inverse relation object
   */
  RelationWeighted2D ( const InvType& inv ) 
    : DataObject ( inv         ) 
    , m_base1    ( inv.m_base2 ) 
    , m_base2    ( inv.m_base1 ) {};
  
  /// destructor (virtual)
  virtual ~RelationWeighted2D(){};
  
  /** object identification (static method) 
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "RelationWeighted2D"            ,
                       IBase1::FromTypeTraits::   id() , 
                       IBase1::ToTypeTraits::     id() , 
                       IBase1::WeightTypeTraits:: id() ) ;
    return s_clid ;
  };

  /** object identification (virtual method)
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  virtual const CLID& clID()     const { return classID() ; }
  
  /** object serialization for writing
   *  only direct relations are serialized 
   *  @see DataObject 
   *  @param  s reference to the output stream
   *  @return   reference to the output stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const 
  {
    // serialize the base class 
    DataObject::serialize( s );
    // serialize total number of relations
    s << m_base1.i_entries().size() ;
    // serialize all relations
    for( Base1::CIT it = m_base1.i_entries().begin() ; 
         m_base1.i_entries().end() != it ; ++it )
      {
        s << Relations::apply( (*it).first.first  , this ) 
          << Relations::apply( (*it).second       , this ) 
          << Relations::apply( (*it).first.second , this );
      }
    return s ;
  };
  
  /** object serialization for reading
   *  @see DataObject 
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )       
  {
    // reset existing relations
    m_base1.i_entries().clear();
    m_base2.i_entries().clear();
    // serialize the base class 
    DataObject::serialize( s );
    // get the total number of relations 
    typename Base1::Entries::size_type num;
    s >> num ; 
    while( num-- > 0 )
      {
        // serialize all relations 
        IBase1::From   from   ; 
        IBase1::To     to     ; 
        IBase1::Weight weight ;
        s >> Relations::apply ( from   , this )
          >> Relations::apply ( to     , this )
          >> Relations::apply ( weight , this ) ;
        m_base1.i_relate ( from , to   , weight  ); // make relations 
        m_base2.i_relate ( to   , from , weight  ); // make relations 
      }
    ///
    return s ;
  };
  
  /** retrive all relations from the object
   *  
   *  @attention direct interface!
   *  @see    IRelationWithWeight 
   *  @see    RelationWithWeightBase
   *  @see    RelationWithWeightObject
   *  @param  object  smart reference to the object
   *  @return pair of iterators for output relations   
   */
  virtual  IBase1::Range relations
  ( const  IBase1::From& object    ) const 
  { 
    Base1::IP ip = m_base1.i_relations( object ) ;
    return IBase1::Range( ip.first , ip.second );
  };
  
  /** retrive all relations from the object
   *  
   *  @attention inverse interface interface!
   *  @see    IRelationWithWeight 
   *  @see    RelationWithWeightBase
   *  @see    RelationWithWeightObject
   *  @param  object  smart reference to the object
   *  @return pair of iterators for output relations   
   */
  virtual  IBase2::Range relations
  ( const  IBase2::From& object    ) const 
  { 
    Base2::IP ip = m_base2.i_relations( object ) ;
    return IBase2::Range( ip.first , ip.second );
  };
  
  /** retrive all relations from the object which has weigth 
   *  larger/smaller than the threshold value 
   *  @attention direct interface!
   *  @param object  smart reference to the object
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return pair of iterators for output relations   
   */
  virtual  IBase1::Range   relations
  ( const  IBase1::From&   object    , 
    const  IBase1::Weight& threshold , 
    const  bool            flag      ) const 
  {
    Base1::IP ip = m_base1.i_relations( object , threshold, flag );
    return IBase1::Range( ip.first , ip.second );
  };
  
  /** retrive all relations from the object which has weigth 
   *  larger/smaller than the threshold value 
   *  @attention inverse interface!
   *  @param  object  smart reference to the object
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return pair of iterators for output relations   
   */
  virtual  IBase2::Range   relations
  ( const  IBase2::From&   object    , 
    const  IBase2::Weight& threshold , 
    const  bool            flag      ) const 
  {
    Base2::IP ip = m_base2.i_relations( object , threshold, flag );
    return IBase2::Range( ip.first , ip.second );
  };
  
  /** make the relation between 2 objects 
   *  @attention direct interface!
   *  @param  object1  smart reference to the first object
   *  @param  object2  smart reference to the second object 
   *  @param  weight   the weigth for the relation 
   *  @return status code 
   */
  virtual StatusCode      relate 
  ( const IBase1::From&   object1 , 
    const IBase1::To&     object2 ,
    const IBase1::Weight& weight  ) 
  {
    StatusCode sc = m_base1.i_relate( object1 , object2 , weight );
    if( sc.isSuccess() ) 
      { sc = m_base2.i_relate( object2 , object1 , weight ); }
    return sc ;
  };
  
  /** make the relation between 2 objects 
   *  @attention inverse interface!
   *  @param  object1  smart reference to the first object
   *  @param  object2  smart reference to the second object 
   *  @param  weight   the weigth for the relation 
   *  @return status code 
   */
  virtual StatusCode      relate 
  ( const IBase2::From&   object1 , 
    const IBase2::To&     object2 ,
    const IBase2::Weight& weight  ) 
  {
    StatusCode sc = m_base2.i_relate( object1 , object2 , weight );
    if( sc.isSuccess() ) 
      { sc = m_base1.i_relate( object2 , object1 , weight ); }
    return sc ;
  };
  
  /** remove the concrete relation between objects
   *  @attention direct interface
   *  @param  object1  smart reference to the first object
   *  @param  object2  smart reference to the second object 
   *  @return status code 
   */
  virtual StatusCode      remove 
  ( const IBase1::From&   object1 , 
    const IBase1::To&     object2 ) 
  {
    StatusCode             sc = m_base1.i_remove( object1 , object2 );
    if( sc.isSuccess() ) { sc = m_base2.i_remove( object2 , object1 ); }
    return sc ;    
  };
  
  /** remove the concrete relation between objects
   *  @attention inverse interface
   *  @param  object1  smart reference to the first object
   *  @param  object2  smart reference to the second object 
   *  @return status code 
   */
  virtual StatusCode      remove 
  ( const IBase2::From&   object1 , 
    const IBase2::To&     object2 ) 
  {
    StatusCode             sc = m_base2.i_remove( object1 , object2 );
    if( sc.isSuccess() ) { sc = m_base1.i_remove( object2 , object1 ); }
    return sc ;    
  };
  
  /** remove all relations FROM the defined object
   *  @attention direct interface
   *  @param  object  smart reference to the object
   *  @return status code 
   */
  virtual StatusCode      remove 
  ( const IBase1::From&   object )
  {
    StatusCode             sc = m_base1.i_remove( object );
    if( sc.isSuccess() ) { sc = m_base2.i_remove( object ); }
    return sc ;    
  };
  
  /** remove all relations FROM the defined object
   *  @attention inverse interface
   *  @param  object  smart reference to the object
   *  @return status code 
   */
  virtual StatusCode      remove 
  ( const IBase2::From&   object )
  {
    StatusCode             sc = m_base2.i_remove( object );
    if( sc.isSuccess() ) { sc = m_base1.i_remove( object ); }
    return sc ;    
  };
  
  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight 
   *  @attention direct interface
   *  @param  object     smart reference to the object
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return status code 
   */
  virtual StatusCode      filter 
  ( const IBase1::From&   object    ,
    const IBase1::Weight& threshold , 
    const bool            flag      )  
  {
    StatusCode             sc = m_base1.i_filter( object , threshold , flag );
    if( sc.isSuccess() ) { sc = m_base2.i_filter( object , threshold , flag ); }
    return sc ;    
  };
  
  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight 
   *  @attention inverse interface
   *  @param  object     smart reference to the object
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return status code 
   */
  virtual StatusCode      filter 
  ( const IBase2::From&   object    ,
    const IBase2::Weight& threshold , 
    const bool            flag      )  
  {
    StatusCode             sc = m_base2.i_filter( object , threshold , flag );
    if( sc.isSuccess() ) { sc = m_base1.i_filter( object , threshold , flag ); }
    return sc ;    
  };
  
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight 
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return status code 
   */
  virtual StatusCode      filter 
  ( const IBase1::Weight& threshold , 
    const bool            flag      )  
  {
    StatusCode             sc = m_base1.i_filter( threshold , flag );
    if( sc.isSuccess() ) { sc = m_base2.i_filter( threshold , flag ); }
    return sc ;    
  };
  
  /** query the interface 
   *  @see    IRelationWithWeight
   *  @see    IInterface 
   *  @param  iid interface identifier 
   *  @param  ppi placeholder 
   *  @return status code
   */
  virtual StatusCode queryInterface( const InterfaceID& iID , void** ppI )
  {
    if( 0 == ppI  )          { return StatusCode::FAILURE ; }   // RETURN !!!
    if( IInterface::interfaceID() == iID ) 
      { *ppI = static_cast<IInterface*> ( this ); }
    else if( IBase1::interfaceID() == iID ) 
      { *ppI = static_cast<IBase1*>     ( this ); }
    else if( IBase2::interfaceID() == iID ) 
      { *ppI = static_cast<IBase2*>     ( this ); }
    else                     { return StatusCode::FAILURE ; }   //  RETURN !!!
    ///
    addRef() ;
    return StatusCode::SUCCESS ;
  };
  
  /** increase the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long addRef  () 
  { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long release () 
  { return  DataObject::release () ; }

private:
  
  Base1 m_base1 ; ///< the actual store of direct  relations   
  Base2 m_base2 ; ///< the actual store of inverse relations   
  
};

// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RelationWeighted2D_H
// ============================================================================
