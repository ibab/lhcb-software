// $Id: RelationWeighted1D.h,v 1.1 2002-03-18 19:32:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_RELATIONWeighted1D_H 
#define RELATIONS_RELATIONWeighted1D_H 1
// Include files
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relation
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"

/** @class RelationWeighted1D RelationWeighted1D.h 
 *                        Relations/RelationWeighted1D.h
 * 
 *  The simplest implementation of unidirectional weighted relations.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 */
template<class FROM, class TO, class WEIGHT>
class RelationWeighted1D :
  public virtual IRelationWeighted<FROM,TO,WEIGHT> ,
  public         DataObject                            
{
  
public:
  
  /// shortcut for own type 
  typedef RelationWeighted1D<FROM,TO,WEIGHT>              OwnType ;
  /// shortcut for inverse type 
  typedef RelationWeighted1D<TO,FROM,WEIGHT>              InvType ;
  /// short cut for interface
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase   ;
  /// short cut for teh actual implementation type 
  typedef Relations::RelationWeightedBase<FROM,TO,WEIGHT> Base    ;
  
public:
  
  /// the standard/default constructor
  RelationWeighted1D ( const typename Base::size_type reserve = 0 ) 
    : DataObject     () , m_base ( reserve ) {};
  
  /** constructor from inverse object 
   *  @param inv relation object to be inverted 
   */
  RelationWeighted1D( const InvType& inv ) 
    : DataObject() , m_base( inv.m_base ) {};
  
  /// destructor (virtual)
  virtual ~RelationWeighted1D (){};
  
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
                       WeightTypeTraits:: id() ) ;
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
    // serialize the base class 
    DataObject::serialize( s );
    // serialize number of relations
    s << m_base.i_entries().size() ;
    // serialize all relations 
    for( Base::CIT it = m_base.i_entries().begin() ; 
         m_base.i_entries().end() != it ; ++it )
      { 
        s << Relations::apply( (*it).first.first  , this ) 
          << Relations::apply( (*it).second       , this ) 
          << Relations::apply( (*it).first.second , this ) ;
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
    m_base.i_entries().clear();
    // serialize the base class 
    DataObject::serialize( s );
    // get the total number of relations 
    typename Base::Entries::size_type num ;
    s >> num ; 
    while( num-- > 0 )
      {
        // serialize all relations 
        From   from   ; 
        To     to     ; 
        Weight weight ;
        s >> Relations::apply ( from   , this )
          >> Relations::apply ( to     , this )
          >> Relations::apply ( weight , this );
        m_base.i_relate ( from , to , weight );  // make relation!
      }
    ///
    return s ;
  };
  
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
  virtual Range       relations
  ( const From&       object    ) const 
  {
    Base::IP ip = m_base.i_relations( object );
    return Range( ip.first , ip.second );
  };
  
  /** retrive all relations from the object which has weigth 
   *  larger/smaller than the threshold value 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return pair of iterators for output relations   
   */
  virtual  Range      relations
  ( const  From&      object    , 
    const  Weight&    threshold , 
    const  bool       flag      ) const 
  {
    Base::IP ip = m_base.i_relations( object , threshold , flag );
    return Range( ip.first , ip.second );    
  };
  
  
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
  { return m_base.i_relate( object1 , object2 , weight ); }
  
  /** remove the concrete relation between objects
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status  code 
   */
  virtual  StatusCode remove 
  ( const  From&      object1 , 
    const  To&        object2 ) 
  { return m_base.i_remove( object1 , object2 ) ; }
  
  /** remove all relations FROM the defined object
   *  @param  object  smart reference to the object
   *  @return status code 
   */
  virtual  StatusCode remove 
  ( const  From&      object )
  { return m_base.i_remove( object ) ; }
  
  /** remove all relations TO the defined object
   *  @param object  smart reference to the object
   *  @return status code 
   */
  virtual  StatusCode remove 
  ( const  To&        object )
  { return m_base.i_remove( object ) ; }
  
  /** filter out the relations FROM the defined object, which
   *  have a weight larger(smaller)than the threshold weight 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filter 
  ( const  From&      object    ,
    const  Weight&    threshold , 
    const  bool       flag      )  
  { return m_base.i_filter( object , threshold , flag ) ; }
  
  /** filter out the relations TO the defined object, which
   *  have a weight larger/smaller than the threshold weight 
   *  @param  object    the object
   *  @param  threshold threshold value for the weight 
   *  @param  flag      flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filter 
  ( const  To&        object    , 
    const  Weight&    threshold , 
    const  bool       flag      )  
  { return m_base.i_filter( object , threshold , flag ) ; }
  
  /** filter out all relations which
   *  have a weight larger/smaller than the threshold weight 
   *  @param  threshold  threshold value for the weight 
   *  @param  flag       flag for larger/smaller
   *  @return status code 
   */
  virtual  StatusCode filter 
  ( const  Weight&    threshold , 
    const  bool       flag      )  
  { return m_base.i_filter( threshold , flag ) ; }

  /** query the interface 
   *  @see    IRelationWeighted1D
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
    else if( IBase::interfaceID() == iID ) 
      { *ppI = static_cast<IBase*>      ( this ); }
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
  
  Base m_base ; ///< the actual relation store  
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_RELATIONWeighted1D_H
// ============================================================================
